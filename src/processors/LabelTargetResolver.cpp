#include "processors/LabelTargetResolver.h"
#include "ErrorReporting.h"

#include <boost/variant.hpp>
#include <boost/format.hpp>

using MrFusion::Processors::LabelTargetResolver;
using MrFusion::ErrorReporting;
using namespace MrFusion::Ast;

using std::tr1::shared_ptr;
using std::string;
using std::stack;

using boost::apply_visitor;
using boost::static_visitor;
using boost::format;



namespace
{
  class ProcLineContents : public static_visitor<void>
  {
  private:
    stack<Label*>& _encounteredUntargetedLabels;
    shared_ptr<ErrorReporting>& _errorReporting;
  public:
    ProcLineContents(stack<Label*>& encounteredUntargetedLabels, shared_ptr<ErrorReporting>& errorReporting) : 
      _encounteredUntargetedLabels(encounteredUntargetedLabels), _errorReporting(errorReporting) {}
    
    void operator()(Data*& data)
    {
      for(;!_encounteredUntargetedLabels.empty(); _encounteredUntargetedLabels.pop())
      {
	_encounteredUntargetedLabels.top()->target = data;
      }
    }
    
    void operator()(Instruction*& instruction)
    {
      for(;!_encounteredUntargetedLabels.empty(); _encounteredUntargetedLabels.pop())
      {
	_encounteredUntargetedLabels.top()->target = instruction;
      }
    }
    
    void operator()(Label*& label)
    {
      _encounteredUntargetedLabels.push(label);
    }
  };
}

void LabelTargetResolver::operator()(Line* line)
{  
  //this is to check if we had a blank line
  if(line->contents)
  {
    auto tmp = ProcLineContents(_encounteredUntargetedLabels, _errorReporting);
    apply_visitor(tmp, line->contents.get());
  }
}