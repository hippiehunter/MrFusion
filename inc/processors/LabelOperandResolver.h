#ifndef MRFUSION_PROCESSORS_LABEL_OPERAND_RESOLVER_H
#define MRFUSION_PROCESSORS_LABEL_OPERAND_RESOLVER_H
#include "AST.h"

#include <string>
#include <map>
#include <tr1/memory>

namespace MrFusion
{
  namespace Processors
  {
    class LabelOperandResolver
    {
    public:
      void operator()(MrFusion::Ast::Line* line);
      
      
    private:
      std::tr1::shared_ptr<std::map<std::string, MrFusion::Ast::Label*>> _labelTable;
      std::tr1::shared_ptr<MrFusion::ErrorReporting> _errorReporting;
    };
  }
}

#endif