#include "processors/LabelNameResolver.h"
#include "ErrorReporting.h"

#include <boost/variant.hpp>
#include <boost/format.hpp>

using MrFusion::Processors::LabelNameResolver;
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
      
    }
    
    void operator()(Label*& label)
    {
      //check if we have any duplicate labels
      //if we do, we need to report the error and continue
      auto existingLabel = _labelTable->find(label->name);
      if(existingLabel != _labelTable->end())
      {
	_errorReporting->addError(format("duplicate label detected: %s, first encountered at %s(%d)", 
					 label->name, existingLabel->line->file.lock()->fileName(), existingLabel->line->lineNumber),
				  label->line->lineNumber, 0, label->line->file.lock()->fileName());
      }
      else
      {
	_labelTable[label->name] = label;
      }
    }
  };
}

void LabelNameResolver::operator()(Line* line)
{  
  //this is to check if we had a blank line
  if(line->contents)
  {
    auto tmp = ProcLineContents(_labelTable, _errorReporting);
    apply_visitor(tmp, line->contents.get());
  }
}