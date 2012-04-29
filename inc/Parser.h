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
    std::vector<std::string> parseIt(const std::string& data, const std::string& fileName);
    Program* ast();
  private:
    Program* program;
  };
}

#endif