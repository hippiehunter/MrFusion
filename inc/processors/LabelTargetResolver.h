#ifndef MRFUSION_PROCESSORS_LABEL_NAME_RESOLVER_H
#define MRFUSION_PROCESSORS_LABEL_NAME_RESOLVER_H
#include "AST.h"

#include <stack>

namespace MrFusion
{
  namespace Processors
  {
    class LabelTargetResolver
    {
    public:
      void operator()(MrFusion::Ast::Line* line);
    private:
      std::stack<MrFusion::Ast::Label*> _encounteredUntargetedLabels;
      std::tr1::shared_ptr<MrFusion::ErrorReporting> _errorReporting;
    };
  }
}

#endif