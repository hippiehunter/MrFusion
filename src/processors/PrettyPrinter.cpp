#include "processors/PrettyPrinter.h"

#include <iomanip>
#include <boost/variant.hpp>

using MrFusion::Processors::PrettyPrinter;
using namespace MrFusion::Ast;

using std::tr1::shared_ptr;
using std::endl;
using std::ostream;
using std::hex;
using std::dec;
using std::string;

using boost::apply_visitor;
using boost::static_visitor;

namespace
{
  class ProcOperand : public static_visitor<void>
  {
  private:
    ostream& _output;
    uint _indentation;
    std::string _preIndentation;
    
  public:
    ProcOperand(ostream& output, int indentation) : _output(output), _indentation(indentation) 
    {
      _preIndentation = string('\t', _indentation);
    }
    
    void operator()(DerefOperand& op)
    {
      _output << _preIndentation << "found deref operand:" << endl;
      
      ProcOperand proc(_output, _indentation + 1);
      apply_visitor(proc, op.target);
    }
    
    void operator()(ExpressionOperand& op)
    {
      _output << _preIndentation << "found expression operand:" << endl
		<< _preIndentation << "first operand:" << endl;
		
      ProcOperand proc(_output, _indentation + 1);
      apply_visitor(proc, op.first);
      
      _output << _preIndentation << "expression operator:" << expressionKindNames[op.kind] << endl;
      apply_visitor(proc, op.second);
    }
    
    void operator()(LabelOperand& op)
    {
      _output << _preIndentation << "found label operand:" << endl
	      << _preIndentation << "\tsymbol name:" << op.symbolName << endl;
    }
    
    void operator()(LiteralOperand& op)
    {
      _output << _preIndentation << "found literal operand:" << endl
	      << _preIndentation << "\tvalue:" << op.value << endl;
    }
    
    void operator()(RegisterOperand& op)
    {
      _output << _preIndentation << "found register operand:" << endl
	      << _preIndentation << "\tvalue:" << registerNames[op.reg] << endl;
    }
    
    void operator()(SpecialOperand& op)
    {
      _output << _preIndentation << "found special operand:" << endl
	      << _preIndentation << "\tvalue:" << specialOperandTypeNames[op.value] << endl;
    }
  };

  class ProcLineContents : public static_visitor<void>
  {
  private:
    ostream& _output;
  public:
    ProcLineContents(ostream& output) : _output(output) {}
    
    void operator()(Data*& data)
    {
      _output << "found data:" << endl
	      << "\tlength:" << data->value.size()
	      << "\traw:"<< hex;
	      
	      
      for(auto word : data->value)
      {
	_output << word;
      }
      
      _output << dec << endl;
    }
    
    void operator()(Instruction*& instruction)
    {
      _output << "found instruction:" << endl
	      << "\topcode:" << opcodeNames[instruction->opCode] << endl;

      ProcOperand proc(_output, 1);
      apply_visitor(proc, instruction->first);
	    
      if(instruction->second)
      {
	apply_visitor(proc, instruction->second.get());
      }
    }
    
    void operator()(Label*& label)
    {
      _output << "found label:" << endl
	      << "\tname:" << label->name << endl;
      if(label->target)
      {
	ProcLineContents proc(_output);
	apply_visitor(proc, label->target.get());
      }
    }
  };
}

void PrettyPrinter::operator()(Line* line)
{
  if(line->file.lock() != _currentFile.lock())
  {
    _currentFile = line->file;
    _output << "file:"
	    << _currentFile.lock()->fileName()
	    << endl;
  }
  
  _output << "line:"
	  << line->lineNumber
	  << endl;
	  
	  
  //this is to check if we had a blank line
  if(line->contents)
  {
    auto tmp = ProcLineContents(_output);
    apply_visitor(tmp, line->contents.get());
  }
}