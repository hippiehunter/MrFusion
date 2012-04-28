#ifndef MRFUSION_PROCESSORS_PRETY_PRINT_H
#define MRFUSION_PROCESSORS_PRETY_PRINT_H
#include "AST.h"

#include <tr1/shared_ptr.h>
#include <iostream>

namespace MrFusion
{
  namespace Processors
  {
    class PrettyPrinter
    {
    public:
      void operator()(Line* line);
    private:
      std::ostream& _output;
      std::tr1::share_ptr<IFile> _currentFile;
    };
  }
}

#endif