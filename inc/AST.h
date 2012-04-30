#ifndef MRFUSION_AST
#define MRFUSION_AST

#include "ErrorReporting.h"
#include "IFile.h"

#include <tr1/memory>
#include <vector>
#include <string>
#include <algorithm>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/pool/object_pool.hpp>


namespace MrFusion
{
  namespace Ast
  {
    struct Data;
    struct Line;
    struct Instruction;
    struct Label;
    
    enum SpecialOperandType
    {
      POP,
      PEEK,
      PUSH,
      SPECIAL_OPERAND_COUNT
    };
    
    extern std::map<SpecialOperandType, const char*> specialOperandTypeNames;
    
    enum Register
    {
      A,
      B,
      C,
      X,
      Y,
      Z,
      I,
      J,
      PC,
      SP,
      OF,
      REGISTER_COUNT
    };
    
    extern std::map<Register, const char*> registerNames;
    
    enum Opcode {
      SET,
      ADD,
      SUB,
      MUL,
      DIV,
      MOD,
      SHL,
      SHR,
      AND,
      BOR,
      XOR,
      IFE,
      IFN,
      IFG,
      IFB,
      JSR,
      OPCODE_COUNT
    };
    
    extern std::map<Opcode, const char*> opcodeNames;
    
    struct Line
    {
      int lineNumber;
      std::tr1::weak_ptr<MrFusion::IFile> file;
      boost::optional<boost::variant<Data*, Instruction*, Label*>> contents;
    };
    
    struct Label
    {
      Line* line;
      std::string name;
      boost::optional<boost::variant<Data*, Instruction*>> target;
      boost::optional<uint16_t> computedAddress;
    };
    
    struct SpecialOperand
    {
      SpecialOperandType value;
    };
    
    struct RegisterOperand
    {
      Register reg;
    };
    
    struct LiteralOperand
    {
      uint16_t value;
    };
    
    struct LabelOperand
    {
      std::string symbolName;
      Label* target;
    };
    
    enum ExpressionKind
    {
      Plus,
      Minus
    };
    
    extern std::map<ExpressionKind, const char*> expressionKindNames;
    
    struct ExpressionOperand
    {
      typedef boost::variant<RegisterOperand, LiteralOperand, LabelOperand> element_type;
      element_type first;
      element_type second;
      ExpressionKind kind;
    };
    
    struct DerefOperand
    {
      typedef boost::variant<RegisterOperand, LiteralOperand, LabelOperand, ExpressionOperand> element_type;
      element_type target;
    };
    
    struct Data
    {
      std::vector<uint16_t> value;
      boost::optional<uint16_t> computedAddress;
    };
    
    typedef boost::variant<RegisterOperand, LiteralOperand, LabelOperand, ExpressionOperand, DerefOperand, SpecialOperand> Operand;
    
    struct Instruction
    {
      Opcode opCode;
      Operand first;
      boost::optional<Operand> second;
      boost::optional<uint16_t> computedAddress;
    };
    
    class GlobalContext
    {
    public:
      template<typename F>
      void processLines(F& f)
      {
	std::for_each(_lines.begin(), _lines.end(), f);
      }
      std::tr1::shared_ptr<IFile> currentFile();
      int nextLineNumber();
      Line* makeLine(std::tr1::shared_ptr<MrFusion::IFile> file, int lineNumber);
      Line* makeLine(std::tr1::shared_ptr<MrFusion::IFile> file, int lineNumber, boost::variant<Data*, Instruction*, Label*> contents);
      Instruction* makeInstruction(Opcode opCode, Operand first);
      Instruction* makeInstruction(Opcode opCode, Operand first, Operand second);
      Data* makeData(std::vector<uint16_t> data);
      Label* makeLabel(std::string name);
      std::tr1::shared_ptr<MrFusion::ErrorReporting> errorReporting();
    private:
      std::vector<std::tr1::shared_ptr<MrFusion::IFile>> _files;
      std::vector<Line*> _lines;
      
      boost::object_pool<Line> _linePool;
      boost::object_pool<Instruction> _instructionPool;
      boost::object_pool<Data> _dataPool;
      boost::object_pool<Label> _labelPool;
    };
  }
}

#endif