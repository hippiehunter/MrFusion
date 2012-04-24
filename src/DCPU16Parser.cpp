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
  
  //partialy compatible variant assignment visitor
  //it takes a compatible subset variant and assigns it to a superset variant
  template<typename Target>
  struct pcv_assign
  : public boost::static_visitor<>
  {
  public:
      pcv_assign(boost::optional<Target>& target_) : target(target_) {}
    
      boost::optional<Target>& target;
      template <typename T>
      void operator()( T & operand ) const
      {
	  target.reset(operand);
      }

  };
  
  template<typename Context>
  struct operandUtil
  {
  public:
    void operator()(Register reg, typename Context::context_type& context, qi::unused_type)
    {
      boost::fusion::at_c<0>(context.attributes) = RegisterOperand(reg);
    }
    void operator()(SpecialOperandType spOp, typename Context::context_type& context, qi::unused_type)
    {
      boost::fusion::at_c<0>(context.attributes) = SpecialOperand { spOp };
    }
    void operator()(DerefOperand& opr, typename Context::context_type& context, qi::unused_type)
    {
      boost::fusion::at_c<0>(context.attributes) = opr;
    }
    void operator()(variant<uint16_t, string>& var, typename Context::context_type& context, qi::unused_type)
    {
      struct expressionAssign
      : public boost::static_visitor<>
      {
      public:
	  Operand& target;
	  void operator()( string & operand ) const
	  {
	      target = LabelOperand { operand, nullptr};
	  }
	  void operator()( uint16_t operand ) const
	  {
	    target = LiteralOperand { operand };
	  }

      };
      boost::apply_visitor(expressionAssign { boost::fusion::at_c<0>(context.attributes) }, var); 
    }
  };
  
  template<typename Context, int pos>
  struct exprUtil
  {
    void operator()(Register reg, typename Context::context_type& context, unused_type&)
    {
      boost::fusion::at_c<0>(context.attributes) = RegisterOperand { reg };
    }
    void operator()(variant<uint16_t, string> var, typename Context::context_type& context, unused_type&)
    {
      struct expressionAssign
      : public boost::static_visitor<>
      {
      public:
	  ExpressionOperand& target;
	  void operator()( string & operand ) const
	  {
	    if(pos == 0)
	      target.first = LabelOperand { operand, nullptr};
	    else
	      target.second = LabelOperand { operand, nullptr};
	  }
	  void operator()( uint16_t operand ) const
	  {
	      if(pos == 0)
		target.first = LiteralOperand { operand };
	      else
		target.second = LiteralOperand { operand };
	  }

      };
      ExpressionOperand expResult;
      boost::apply_visitor(expressionAssign { expResult }, var); 
      boost::fusion::at_c<0>(context.attributes) = DerefOperand { expResult };
    }
  };
  
  
  DCPU16AssemblyGrammar(Program* ast) : DCPU16AssemblyGrammar::base_type(start)
  {
    auto addLabel = [&](string& name, typename label_rule_type::context_type& context)
      {
        boost::fusion::at_c<0>(context.attributes) = ast->addLabel(name);
      };
      
    auto attachStatement = [](variant<Data*, Instruction*>& stmt, typename line_rule_type::context_type& context)
      {
	if(boost::fusion::at_c<0>(context.attributes)->contents)
	{
	  auto currentLabel = boost::get<Label*>(boost::fusion::at_c<0>(context.attributes)->contents.get());
	  currentLabel->target = stmt;
	}
	else
	{
	  boost::apply_visitor(pcv_assign<boost::variant<Data*, Instruction*, Label*>> ( boost::fusion::at_c<0>(context.attributes)->contents ), stmt);
	}
      };
      
    auto attachLabel = [](Label* lbl, typename line_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->contents = lbl;
      };
      
    auto attachOpcode = [](Opcode op, typename instr_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->opCode = op;
      };
      
    auto attachOperand1 = [](Operand& op, typename instr_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->first = op;
      };
      
    auto attachOperand2 = [](Operand& op, typename instr_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->second = op;
      };
      
    auto attachDatList = [](vector<uint16_t>& raw, typename dat_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->value = raw; 
      };
    
    _ast = ast;
    start = *(line[[](unused_type) {}]);
    label = lit(':') > symbol[addLabel];
    
    line = newLine
      | comment
      | statement[attachStatement] >> -(comment) >> -(newLine)
      | label[attachLabel] >> statement[attachStatement] >> -(comment) >> -(newLine)
      | label[attachLabel] >> -(newLine);
      
    statement = dat
      | instr;
      
    instr = no_case[binaryOpcodes[attachOpcode]] > operand[attachOperand1] > ',' > operand[attachOperand2]
      | no_case[unaryOpcodes[attachOpcode]] > operand[attachOperand1];
      
    operand = no_case[specialOperands[operandUtil<operand_rule_type>()]]
      | no_case[gpRegisters[operandUtil<operand_rule_type>()]]
      | expr[operandUtil<operand_rule_type>()]
      | operand_deref[operandUtil<operand_rule_type>()];
      
    //operand_deref = lit('[') >> no_case[gpRegisters[operandUtil<operand_deref_rule_type>()]] >> ']'
    //  | lit('[') >> expr[operandUtil<operand_rule_type>()] >> ']'
    //  | lit('[') >> expr[exprUtil<operand_rule_type, 0>()] >> '+' >> no_case[gpRegisters[exprUtil<operand_rule_type, 1>()]] >> ']'
    //  | lit('[') >> no_case[gpRegisters[exprUtil<operand_rule_type, 0>()]] >> '+' >> expr[exprUtil<operand_rule_type, 1>()] >> ']';
      
    symbol %= alpha >> *(alnum);
    
    expr = symbol
      | lexeme[no_case["0x"] > hex]
      | ushort_;
      
    dat = lexeme[no_case["dat"]] > datlist[attachDatList];
    
    datlist = quoted_string
      | dat_elem % ',';
      
    dat_elem = lexeme[no_case["0x"] > hex]
      | ushort_;
      
    newLine = eol[[&](unused_type const& unused) { _ast->addNewLine(); }];
    comment = ';' >> *(char_) >> -(newLine);
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