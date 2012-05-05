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
	("JSR", Opcode::JSR)
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
      //phx::function<make_label_impl> make_label(;
      
      start = *(line);
      
      line = newLine[qi::_val = make_line_empty(phx::ref(c))]
      | comment[qi::_val = make_line_empty(phx::ref(c))] >> -(newLine)
      
      | statement[qi::_val = make_line(qi::_1, phx::ref(c))] >> 
	-(comment) >> -(newLine)
	
      | label[qi::_a = qi::_1] >> 
	statement[qi::_val = make_line(attach_statement(qi::_a, qi::_1), phx::ref(c))] >>
	-(comment) >> -(newLine)
	
      | label[qi::_val = make_line(qi::_1, phx::ref(c))] >> 
	-(newLine);
	
      //label = lit(':') > symbol[qi::_val = make_label(qi::_1)];
    }
  };
}

AsmParser::~AsmParser()
{
}

AsmParser::AsmParser()
{
  
}

vector<string> AsmParser::parseIt(const string& data, const string& fileName)
{
  auto file = IFileFactory::makeFile(fileName);
  qi::phrase_parse(file->contents()->begin(), file->contents()->end(), DCPU16AssemblyGrammar<string::iterator>(_context), space_type(), skip_flag::postskip);
}
