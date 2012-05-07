#ifndef MRFUSION_PARSER_H
#define MRFUSION_PARSER_H

#include "AST.h"

namespace MrFusion
{
  class AsmParser
  {
  public:
    ~AsmParser();
    AsmParser();
    void parseIt(const std::string& data, const std::string& fileName);
    MrFusion::Ast::GlobalContext* context();
  private:
    MrFusion::Ast::GlobalContext* _context;
  };
}

#endif