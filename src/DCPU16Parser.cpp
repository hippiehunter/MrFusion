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
  typedef rule<Iterator, variant<string, vector<uint16_t>>(), space_type> datlist_rule_type;
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
  
  template<typename OP = Operand>
  struct expressionAssign
      : public boost::static_visitor<OP>
  {
  public:
    OP operator()( string & operand ) const
    {
      return LabelOperand { operand, nullptr};
    }
    OP operator()( uint16_t operand ) const
    {
      return LiteralOperand { operand };
    }
  };
  
  template<int pos>
  struct expressionOpAssign
    : public boost::static_visitor<ExpressionOperand&>
  {
  public:
    expressionOpAssign(ExpressionOperand& target_) : target(target_) {}
    ExpressionOperand& target;
    ExpressionOperand& operator()( string & operand ) const
    {
      if(pos == 0)
	target.first = LabelOperand { operand, nullptr};
      else
	target.second = LabelOperand { operand, nullptr};
      return target;
    }
    ExpressionOperand& operator()( uint16_t operand ) const
    {
	if(pos == 0)
	  target.first = LiteralOperand { operand };
	else
	  target.second = LiteralOperand { operand };
	return target;
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
      
    auto attachOpcode = [&](Opcode op, typename instr_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes) = ast->addInstruction(op);
      };
      
    auto attachOperand1 = [](Operand& op, typename instr_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->first = op;
      };
      
    auto attachOperand2 = [](Operand& op, typename instr_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes)->second = op;
      };
      
    auto attachDatList = [&](variant<string, vector<uint16_t>>& raw, typename dat_rule_type::context_type& context)
      {
	struct convertToRaw
	  : public boost::static_visitor<vector<uint16_t>>
	{
	public:
	  vector<uint16_t> operator()( string & operand ) const
	  {
	    vector<uint16_t> result;
	    for(auto ch : operand)
	      result.push_back(ch);
	    return result;
	  }
	  vector<uint16_t> operator()( vector<uint16_t> operand ) const
	  {
	    return operand;
	  }
	};
	auto cleanDat = boost::apply_visitor(convertToRaw(), raw);
	boost::fusion::at_c<0>(context.attributes) = ast->addData(cleanDat); 
      };
      
    auto operand_makeRegister = [](Register const& reg, typename operand_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes) = RegisterOperand(reg);
      };
    auto operand_makeSpecialOp = [](SpecialOperandType const& spOp, typename operand_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes) = SpecialOperand { spOp };
      };
    auto operand_makeDerefOp = [](DerefOperand const& opr, typename operand_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes) = opr;
      };
    auto operand_makeExpr = [](variant<uint16_t, string>& var, typename operand_rule_type::context_type& context)
    {
      boost::fusion::at_c<0>(context.attributes) = boost::apply_visitor(expressionAssign<Operand>(), var); 
    };
    
    auto opd_makeExpr = [](variant<uint16_t, string>& var, typename operand_deref_rule_type::context_type& context)
    {
      boost::fusion::at_c<0>(context.attributes).target = 
	boost::apply_visitor(expressionAssign<typename DerefOperand::element_type>(), var); 
    };
    
    auto opd_makeRegister = [](Register const& reg, typename operand_deref_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes) = DerefOperand { RegisterOperand(reg) };
      };
    auto opd_makeRegister0 = [](Register const& reg, typename operand_deref_rule_type::context_type& context)
      {
	boost::fusion::at_c<0>(context.attributes) = DerefOperand { ExpressionOperand { reg } };
      };

    auto opd_makeRegister1 = [](Register const& reg, typename operand_deref_rule_type::context_type& context)
      {
	auto derefOpr = boost::get<DerefOperand>(boost::fusion::at_c<0>(context.attributes));
	auto expResult = boost::get<ExpressionOperand>(derefOpr.target);
	expResult.second = RegisterOperand(reg);
      };

    auto opd_makeExpr0 = [](variant<uint16_t, string>& var, typename operand_deref_rule_type::context_type& context) 
      {
	ExpressionOperand expResult;
	boost::apply_visitor(expressionOpAssign<0>(expResult), var); 
	boost::fusion::at_c<0>(context.attributes) = DerefOperand { expResult };
      };
      
    auto opd_makeExpr1 = [](variant<uint16_t, string>& var, typename operand_deref_rule_type::context_type& context) 
      {
	auto derefOpr = boost::get<DerefOperand>(boost::fusion::at_c<0>(context.attributes));
	auto expResult = boost::get<ExpressionOperand>(derefOpr.target);
	
	boost::apply_visitor(expressionOpAssign<1>(expResult), var); 
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
      
    operand = no_case[specialOperands[operand_makeSpecialOp]]
      | no_case[gpRegisters[operand_makeRegister]]
      | expr[operand_makeExpr]
      | operand_deref[operand_makeDerefOp];
      
    operand_deref = lit('[') >> no_case[gpRegisters[opd_makeRegister]] >> ']'
      | lit('[') >> expr[opd_makeExpr] >> ']'
      | lit('[') >> expr[opd_makeExpr0] >> '+' >> no_case[gpRegisters[opd_makeRegister1]] >> ']'
      | lit('[') >> no_case[gpRegisters[opd_makeRegister0]] >> '+' >> expr[opd_makeExpr1] >> ']';
      
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


vector<string> AsmParser::parseIt(const string& data, const string& fileName)
{
  program->addNewFile(fileName);
  phrase_parse(data.begin(), data.end(), DCPU16AssemblyGrammar<string::iterator>(program), space_type(), skip_flag::postskip, program);
}

AsmParser::AsmParser()
{
  program = new Program();
}

AsmParser::~AsmParser()
{
  delete program;
}

Program* AsmParser::ast()
{
  return program;
}

Line* Program::addNewLine()
{
  auto newLine = new Line();
  newLine->fileName = currentFileName();
  newLine->lineNumber = _lines.size();
  _lines[newLine->fileName].push_back(newLine);
  return newLine;
}

Label* Program::addLabel(const std::string& labelName)
{
  if(_labels.find(labelName) != _labels.end())
    addError("duplicate label declared");
  
  auto newLabel = new Label();
  newLabel->name = labelName;
  newLabel->line = currentLine();
  _labels[labelName] = newLabel;
  return newLabel;
}
Data* Program::addData(std::vector<uint16_t> data)
{
  auto newData = new Data();
  newData->value = data;
  _dataLiterals.push_back(newData);
  return newData;
}

Instruction* Program::addInstruction(Opcode op)
{
  auto newInstruction = new Instruction();
  newInstruction->opCode = op;
  _instructions.push_back(newInstruction);
  return newInstruction;
}

string* Program::currentFileName()
{
  return _fileNames.back();
}

Line* Program::currentLine()
{
  return _lines[currentFileName()].back();
}

void Program::addNewFile(const string& fileName)
{
  auto fileNamePtr = new string(fileName);
  _lines[fileNamePtr] = vector<Line*>();
  _fileNames.push_back(fileNamePtr);
  addNewLine();
}

void Program::addError(const string& message)
{
  _errors.push_back(message);
}

Program::~Program()
{
  
}