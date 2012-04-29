#include "Parser.h"
#include "AST.h"

#include <boost/spirit/include/qi.hpp>

using namespace MrFusion::Ast;

using boost::spirit::qi::symbols;
using boost::spirit::qi::grammar;
using boost::spirit::qi::rule;
using boost::spirit::qi::space_type;
using boost::spirit::unused_type;

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
  
  public:
    DCPU16AssemblyGrammar(GlobalContext* globalContext)
      : DCPU16AssemblyGrammar::base_type(start)
    {
    }
  };
}
