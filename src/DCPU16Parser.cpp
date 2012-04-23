#include "DCPU16Parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/fusion/sequence.hpp>
#include <algorithm>
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/home/phoenix/bind.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>

using std::string;
using std::vector;
using boost::spirit::qi::ascii::alpha;
using boost::spirit::qi::ascii::alnum;
using boost::spirit::qi::rule;
using boost::spirit::qi::eps;
using boost::spirit::qi::lit;
using boost::spirit::qi::hex;
using boost::spirit::qi::ushort_;
using boost::spirit::qi::ascii::char_;
using boost::spirit::qi::ascii::space_type;
using boost::spirit::qi::symbols;
using boost::spirit::qi::grammar;
using boost::spirit::qi::no_case;
using boost::spirit::qi::lexeme;
using boost::spirit::eol;
using boost::spirit::unused_type;
using boost::spirit::qi::skip_flag;
using boost::variant;
using std::copy;
using std::back_inserter;
using namespace Parser;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

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

struct specialOperands_ : symbols<char, Register>
{
  specialOperands_()
  {
    add
      ("PUSH", Register::A)
      ("POP", Register::B)
      ("PEEK", Register::C)
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
struct DCPU16AssemblyGrammar : grammar<Iterator, Program*(), space_type>
{
  typedef rule<Iterator, Program*(), space_type> start_rule_type;
  typedef rule<Iterator, Label*(), space_type> label_rule_type;
  typedef rule<Iterator, variant<uint16_t, string>(), space_type> expr_rule_type;
  typedef rule<Iterator, vector<uint16_t>(), space_type> datlist_rule_type;
  typedef rule<Iterator, uint16_t(), space_type> dat_elem_rule_type;
  typedef rule<Iterator, DerefOperand(), space_type> operand_deref_rule_type;
  typedef rule<Iterator, Operand(), space_type> operand_rule_type;
  typedef rule<Iterator, Instruction*(), space_type> instr_rule_type;
  typedef rule<Iterator, Data*(), space_type> dat_rule_type;
  typedef rule<Iterator, Line*(), space_type> line_rule_type;
  typedef rule<Iterator, variant<Data*, Instruction*>(), space_type> statement_rule_type;
  typedef rule<Iterator, unused_type, space_type> newLine_rule_type;
  typedef rule<Iterator, string(), space_type> simple_string_rule_type;
  
  DCPU16AssemblyGrammar(Program* ast) : DCPU16AssemblyGrammar::base_type(start)
  {
    _ast = ast;
    start = *(line[[](unused_type) {}]);
    label = lit(':') > symbol[[&](string& name, typename label_rule_type::context_type& context) 
				  {  
				   boost::fusion::at_c<0>(context.attributes) = _ast->addLabel(name);
				  }];
    
    line = newLine
      | comment
      | statement >> -(comment) >> newLine
      | label >> statement >> -(comment) >> newLine;
      
    statement = dat
      | instr;
      
    instr = no_case[binaryOpcodes] > operand > ',' > operand
      | no_case[unaryOpcodes] > operand;
      
    operand = no_case[specialOperands]
      | no_case[gpRegisters]
      | expr
      | operand_deref;
      
    operand_deref = lit('[') >> no_case[gpRegisters] >> ']'
      | lit('[') >> expr >> ']'
      | lit('[') >> expr >> '+' >> no_case[gpRegisters] >> ']'
      | lit('[') >> no_case[gpRegisters] >> '+' >> expr >> ']';
      
    symbol %= alpha >> *(alnum);
    
    expr = symbol
      | lexeme[no_case["0x"] > hex]
      | ushort_;
      
    dat = lexeme[no_case["dat"]] > datlist;
    
    datlist = quoted_string
      | dat_elem % ',';
      
    dat_elem = lexeme[no_case["0x"] > hex]
      | ushort_;
      
    newLine = eol[[&](unused_type const& unused) { _ast->addNewLine(); }];
    comment = ';' >> *(char_) >> eol;
  }
  
  simple_string_rule_type comment, symbol, quoted_string;
  start_rule_type start;
  label_rule_type label;
  expr_rule_type expr;
  datlist_rule_type datlist;
  dat_elem_rule_type dat_elem;
  operand_deref_rule_type operand_deref;
  operand_rule_type operand;
  instr_rule_type instr;
  dat_rule_type dat;
  line_rule_type line;
  statement_rule_type statement;
  newLine_rule_type newLine;
private:
  Program* _ast;
};


Program* AsmParser::parseIt(string data)
{
  Program* newProgram = new Program();
  phrase_parse(data.begin(), data.end(), DCPU16AssemblyGrammar<string::iterator>(newProgram), space_type(), skip_flag::postskip, newProgram);
}
/*
      program 
	= program >> line >> '\n'
	| program >> '\n';
      
      line
	= statement
	| label
	| label >> statement;
      
      label
	= ':' << alpha << *(;

      statement
	= dat
	| instr;
      
      instr
	= OP2 << operand << ',' << operand
	| OP1 << operand
	| error;
    
      operand
	= XREG
	| gpreg
	| expr
	| deref
	| error;
      deref
	= '[' << deref_expr << ']';
	
      deref_expr
	= gpreg
	| expr
	| expr << '+' << gpreg
	| expr << '-' << expr
	| gpreg << '+' << expr
	| error;
	
      gpreg
	= GPREG;
      
      expr
	= CONSTANT
	| SYMBOL;
  
      dat*/