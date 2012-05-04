#include "processors/LabelOperandResolver.h"
#include "ErrorReporting.h"

#include <boost/variant.hpp>
#include <boost/format.hpp>

using MrFusion::Processors::LabelOperandResolver;
using MrFusion::ErrorReporting;
using namespace MrFusion::Ast;

using std::tr1::shared_ptr;
using std::string;
using std::map;

using boost::apply_visitor;
using boost::static_visitor;
using boost::format;



namespace
{
  class ProcOperand : public static_visitor<void>
  {
  private:
    shared_ptr<map<string, Label*>>& _labelTable;
    shared_ptr<ErrorReporting>& _errorReporting;
  public:
    ProcOperand(shared_ptr<map<string, Label*>>& labelTable, shared_ptr<ErrorReporting>& errorReporting) : 
      _labelTable(labelTable), _errorReporting(errorReporting) {}
    
    void operator()(DerefOperand& op)
    {
      ProcOperand proc(_labelTable, _errorReporting);
      apply_visitor(proc, op.target);
    }
    
    void operator()(ExpressionOperand& op)
    {
      ProcOperand proc(_labelTable, _errorReporting);
      apply_visitor(proc, op.first);
      
      if(op.second)
	apply_visitor(proc, op.second);
    }
    
    void operator()(LabelOperand& op)
    {
      //generate and unresolved label error if we cant find this
      auto foundLabel = _labelTable.find(op.symbolName);
      if(foundLabel != _labelTable->end())
      {
	op.target = *foundLabel;
      }
      else
      {
      }
    }
    
    void operator()(LiteralOperand& op)
    {
      
    }
    
    void operator()(RegisterOperand& op)
    {
      
    }
    
    void operator()(SpecialOperand& op)
    {
      
    }
  };
  
  
  
  class ProcLineContents : public static_visitor<void>
  {
  private:
    shared_ptr<map<string, Label*>>& _labelTable;
    shared_ptr<ErrorReporting>& _errorReporting;
  public:
    ProcLineContents(shared_ptr<map<string, Label*>>& labelTable, shared_ptr<ErrorReporting>& errorReporting) : 
      _labelTable(labelTable), _errorReporting(errorReporting) {}
    
    void operator()(Data*& data)
    {
      
    }
    
    void operator()(Instruction*& instruction)
    {
      ProcOperand proc(_labelTable, _errorReporting);
      apply_visitor(proc, instruction->first);
	    
      if(instruction->second)
      {
	apply_visitor(proc, instruction->second.get());
      }
    }
    
    void operator()(Label*& label)
    {
      
    }
  };
}

void LabelOperandResolver::operator()(Line* line)
{  
  //this is to check if we had a blank line
  if(line->contents)
  {
    auto tmp = ProcLineContents(_labelTable, _errorReporting);
    apply_visitor(tmp, line->contents.get());
  }
}