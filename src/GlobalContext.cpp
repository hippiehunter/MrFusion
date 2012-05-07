#include <vector>
#include <string>
#include <tr1/memory>

#include "AST.h"

using MrFusion::Ast::GlobalContext;

using std::tr1::shared_ptr;
using MrFusion::IFile;
using MrFusion::Ast::Data;
using MrFusion::Ast::Instruction;
using MrFusion::Ast::Label;
using MrFusion::Ast::Line;
using MrFusion::ErrorReporting;
using MrFusion::Ast::Opcode;
using MrFusion::Ast::Operand;

using std::vector;
using std::string;

using boost::variant;


shared_ptr<IFile> GlobalContext::currentFile()
{
  return _files.back();
}

void GlobalContext::addFile(const shared_ptr<IFile>& file)
{
  _files.push_back(file);
}

int GlobalContext::nextLineNumber()
{
  return _currentLine++;
}

Line* GlobalContext::makeLine(shared_ptr<IFile> file, int lineNumber)
{
  auto line = _linePool.construct();
  line->file = file;
  line->lineNumber = lineNumber;
  return line;
}

Line* GlobalContext::makeLine(shared_ptr<IFile> file, int lineNumber, variant<Data*, Instruction*, Label*> contents)
{
  auto line = _linePool.construct();
  line->file = file;
  line->lineNumber = lineNumber;
  line->contents = contents;
  return line;
}

Instruction* GlobalContext::makeInstruction(Opcode opCode, Operand first)
{
  auto instruction = _instructionPool.construct();
  instruction->opCode = opCode;
  instruction->first = first;
  return instruction;
}

Instruction* GlobalContext::makeInstruction(Opcode opCode, Operand first, Operand second)
{
  auto instruction = _instructionPool.construct();
  instruction->opCode = opCode;
  instruction->first = first;
  instruction->second = second;
  return instruction;
}

Data* GlobalContext::makeData(const vector<uint16_t>& data)
{
  auto madeData = _dataPool.construct();
  madeData->value = data;
  return madeData;
}

Label* GlobalContext::makeLabel(const string& name)
{
  auto label = _labelPool.construct();
  label->name = name;
  return label;
}

shared_ptr<ErrorReporting> GlobalContext::errorReporting()
{
  return _errorReporting;
}