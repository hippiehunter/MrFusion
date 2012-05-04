#ifndef MRFUSION_PROCESSORS_PRETY_PRINT_H
#define MRFUSION_PROCESSORS_PRETY_PRINT_H
#include "AST.h"

#include <tr1/memory>
#include <iostream>

namespace MrFusion
{
  namespace Processors
  {
    class PrettyPrinter
    {
    public:
      void operator()(MrFusion::Ast::Line* line);
    private:
      std::ostream& _output;
      std::tr1::weak_ptr<MrFusion::IFile> _currentFile;
    };
  }
}

#endif