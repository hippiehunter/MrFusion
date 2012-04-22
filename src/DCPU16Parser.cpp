#include "DCPU16Parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/fusion/sequence.hpp>
#include <algorithm>
#include <boost/typeof/typeof.hpp>

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
using boost::spirit::eol;
using boost::spirit::unused_type;
using boost::spirit::qi::skip_flag;
using boost::variant;
using boost::bind;
using std::copy;
using std::back_inserter;
using namespace Parser;

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

template<typename DataType, typename Context>
struct concat
{
  void operator()(DataType& data, Context& context, unused_type&)
  {
    auto enclosingAttribute = boost::fusion::at_c<0>(context.attributes);
    copy(data.begin(), data.end(), back_inserter(enclosingAttribute));
  }
};


template<typename Iterator>
struct DCPU16AssemblyGrammar : grammar<Iterator, Program*, space_type>
{
  DCPU16AssemblyGrammar(Program* ast) : DCPU16AssemblyGrammar::base_type(start)
  {
    _ast = ast;
    start = *(line);
    
    label = lit(':') >> symbol[[&](std::string const& symbolName) { return _ast->addLabel(symbolName); }];
    
    line = newLine
      || comment
      || statement >> -(comment) >> newLine
      || label >> statement >> -(comment) >> newLine;
      
    statement = dat
      || instr;
      
    instr = binaryOpcodes >> operand >> ',' >> operand
      || unaryOpcodes >> operand;
      
    operand = specialOperands
      || gpRegisters
      || expr
      || operand_deref;
      
    operand_deref = lit('[') >> gpRegisters >> ']'
      | lit('[') >> expr >> ']'
      | lit('[') >> expr >> '+' >> gpRegisters >> ']'
      | lit('[') >> gpRegisters >> '+' >> expr >> ']';
      
    symbol = alpha >> *(alnum);
    
    expr = symbol
      | hex
      | ushort_;
      
    dat = lit("dat") >> datlist;
    
    datlist = dat_elem[concat<vector<uint16_t>, BOOST_TYPEOF(_val)>()]
      | quoted_string[concat<string, BOOST_TYPEOF(_val)>()]
      | dat_elem[concat<vector<uint16_t>, BOOST_TYPEOF(_val)>()] % ',';
      
    dat_elem = hex
      | ushort_;
      
    newLine = eol[[&](unused_type const& unused) { _ast->addNewLine(); }];
    comment = ';' >> *(char_) >> '\n';
  }
  
  rule<Iterator, string(), space_type> comment, symbol, quoted_string;
  rule<Iterator, Program*, space_type> start;
  rule<Iterator, Label*, space_type> label;
  rule<Iterator, variant<uint16_t, string>(), space_type> expr;
  rule<Iterator, vector<uint16_t>, space_type> datlist, dat_elem;
  rule<Iterator, unused_type, space_type> line, dat, newLine, instr, operand, statement, operand_deref;
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