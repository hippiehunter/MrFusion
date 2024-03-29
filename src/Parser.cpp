#include "Parser.h"
#include "AST.h"
#include "IFile.h"
#include "ParserUtils.h"

#include <vector>
#include <string>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>


using namespace MrFusion::Ast;
using MrFusion::IFileFactory;
using MrFusion::IFile;
using MrFusion::AsmParser;
using MrFusion::ParserUtils::attach_statement_impl;
using MrFusion::ParserUtils::make_line_empty_impl;
using MrFusion::ParserUtils::make_line_impl;
using MrFusion::ParserUtils::make_label_impl;
using MrFusion::ParserUtils::make_unary_instruction_impl;
using MrFusion::ParserUtils::make_binary_instruction_impl;
using MrFusion::ParserUtils::make_operand_impl;
using MrFusion::ParserUtils::make_deref_operand_impl;
using MrFusion::ParserUtils::make_compound_expr_impl;
using MrFusion::ParserUtils::make_datlist_impl;

using boost::spirit::qi::symbols;
using boost::spirit::qi::grammar;
using boost::spirit::qi::rule;
using boost::spirit::qi::space_type;
using boost::spirit::qi::skip_flag;
using boost::spirit::unused_type;
using boost::variant;

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

using std::vector;
using std::string;

namespace
{
  struct gpRegisters_ : symbols<char, Register>
  {
    gpRegisters_()
    {
      add
	("A", Register::A)
	("B", Register::B)
	("C", Register::C)
	("I", Register::I)
	("J", Register::J)
	("X", Register::X)
	("Y", Register::Y)
	("Z", Register::Z)
      ;
    }

  } gpRegisters;

  struct specialOperands_ : symbols<char, SpecialOperandType>
  {
    specialOperands_()
    {
      add
	("PUSH", SpecialOperandType::PUSH)
	("POP", SpecialOperandType::POP)
	("PEEK", SpecialOperandType::PEEK)
      ;
    }

  } specialOperands;

  struct specialRegisters_ : symbols<char, Register>
  {
    specialRegisters_()
    {
      add
	("PC", Register::PC)
	("O", Register::OF)
	("SP", Register::SP)
      ;
    }
  } specialRegisters;

  struct binaryOpcodes_ : symbols<char, Opcode>
  {
    binaryOpcodes_()
    {
      
      add
	("SET", Opcode::SET)
	("ADD", Opcode::ADD)
	("SUB", Opcode::SUB)
	("MUL", Opcode::MUL)
	("DIV", Opcode::DIV)
	("MOD", Opcode::MOD)
	("SHL", Opcode::SHL)
	("SHR", Opcode::SHR)
	("AND", Opcode::AND)
	("BOR", Opcode::BOR)
	("XOR", Opcode::XOR)
	("IFE", Opcode::IFE)
	("IFN", Opcode::IFN)
	("IFG", Opcode::IFG)
	("IFB", Opcode::IFB)
      ;
    }
  } binaryOpcodes;

  struct unaryOpcodes_ : symbols<char, Opcode>
  {
    unaryOpcodes_()
    {
      add
	("JSR", Opcode::JSR)
      ;
    }
  } unaryOpcodes;


  template<typename Iterator>
  struct DCPU16AssemblyGrammar : grammar<Iterator, unused_type, space_type>
  {
  private:
    rule<Iterator, unused_type, space_type> start;
    rule<Iterator, Label*(), space_type> label;
    rule<Iterator, variant<uint16_t, string>(), space_type> expr;
    rule<Iterator, variant<string, vector<uint16_t>>(), space_type> datlist;
    rule<Iterator, uint16_t(), space_type> dat_elem;
    rule<Iterator, DerefOperand(), space_type> operand_deref;
    rule<Iterator, Operand(), space_type> operand;
    rule<Iterator, ExpressionOperand(), space_type> compound_expr;
    rule<Iterator, Instruction*(), space_type> instr;
    rule<Iterator, Data*(), space_type> dat;
    rule<Iterator, Line*(), qi::locals<Label*>, space_type> line;
    rule<Iterator, variant<Data*, Instruction*>(), space_type> statement;
    rule<Iterator, unused_type, space_type> newLine;
    rule<Iterator, string(), space_type> comment, symbol, quoted_string;
  public:
    DCPU16AssemblyGrammar(GlobalContext* c)
      : DCPU16AssemblyGrammar::base_type(start)
    {

      phx::function<make_line_empty_impl> make_line_empty;
      phx::function<make_line_impl> make_line;
      phx::function<attach_statement_impl> attach_statement;
      phx::function<make_label_impl> make_label;
      phx::function<make_unary_instruction_impl> make_unary_instruction;
      phx::function<make_binary_instruction_impl> make_binary_instruction;
      phx::function<make_operand_impl> make_operand;
      phx::function<make_deref_operand_impl> make_deref_operand;
      phx::function<make_compound_expr_impl> make_compound_expr;
      phx::function<make_datlist_impl> make_datlist;
      
      start = line % newLine;
      
      line = comment[qi::_val = make_line_empty(phx::ref(c))]
	
	| statement[qi::_val = make_line(qi::_1, phx::ref(c))] >> 
	  -(comment)
	  
	| label[qi::_a = qi::_1] >> 
	  statement[qi::_val = make_line(attach_statement(qi::_a, qi::_1), phx::ref(c))] >>
	  -(comment)
	  
	| label[qi::_val = make_line(qi::_1, phx::ref(c))]
	  
	| qi::eps[qi::_val = make_line_empty(phx::ref(c))];
	
      label = qi::lit(':') > symbol[qi::_val = make_label(qi::_1, phx::ref(c))];
      
      statement = dat
	| instr;
	
      instr = (qi::no_case[binaryOpcodes] > 
		operand > qi::lit(',') > 
		operand)[make_binary_instruction(qi::_1, qi::_2, qi::_3, phx::ref(c))]
		
	    | (qi::no_case[unaryOpcodes] > 
		operand)[make_unary_instruction(qi::_1, qi::_2, phx::ref(c))];
		
      operand = qi::no_case[specialOperands[make_operand(qi::_1)]]
	| qi::no_case[gpRegisters[make_operand(qi::_1)]]
	| expr[make_operand(qi::_1)]
	| compound_expr
	| operand_deref;
	
      operand_deref = qi::lit('[') >> qi::no_case[gpRegisters[make_deref_operand(qi::_1)]] >> ']'
	| qi::lit('[') >> expr[make_deref_operand(qi::_1)] >> ']'
	| qi::lit('[') >> compound_expr[make_deref_operand(qi::_1)] >> ']';
	
      compound_expr = 
	(expr >> '+' >> qi::no_case[gpRegisters])
	  [make_compound_expr(phx::val(ExpressionKind::Plus), qi::_1, qi::_2)]
	
	| (qi::no_case[gpRegisters] >> '+' >> expr)
	  [make_compound_expr(phx::val(ExpressionKind::Plus), qi::_2, qi::_1)];
	  
      symbol %= qi::alpha >> *(qi::alnum);
    
      expr = symbol
	| qi::lexeme[qi::no_case["0x"] > qi::hex]
	| qi::ushort_;
	
      dat = qi::lexeme[qi::no_case["dat"]] > datlist[make_datlist(qi::_1, phx::ref(c))];
      
      datlist = quoted_string
	| dat_elem % ',';
	
      dat_elem = qi::lexeme[qi::no_case["0x"] > qi::hex]
	| qi::ushort_;
	
      newLine = qi::eol;
      comment = ';' >> *(qi::char_) >> -(newLine);
    }
  };
}

AsmParser::~AsmParser()
{
  if(_context != nullptr)
    delete _context;
}

AsmParser::AsmParser()
{
  _context = new GlobalContext();
}

GlobalContext* AsmParser::context() { return _context; }

void AsmParser::parseIt(const string& data, const string& fileName)
{
  auto file = IFileFactory::makeFile(fileName);
  _context->addFile(file);
  qi::phrase_parse(file->contents()->begin(), file->contents()->end(), DCPU16AssemblyGrammar<string::iterator>(_context), space_type(), skip_flag::postskip);
}
