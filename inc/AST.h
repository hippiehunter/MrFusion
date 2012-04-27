#ifndef MRFUSION_AST
#define MRFUSION_AST

#include <tr1/shared_ptr.h>
#include <vector>
#include <string>
#include <algorithm>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/pool/object_pool.hpp>

class IFile;

namespace MrFusion
{
  namespace Ast
  {
    enum SpecialOperandType
    {
      POP,
      PEEK,
      PUSH
    };
    
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
      JSR
    };

    struct Line
    {
      int lineNumber;
      IFile* fileName;
      boost::optional<boost::variant<Data*, Instruction*, Label*>> contents;
    };
    
    struct Label
    {
      Line* line;
      std::string name;
      boost::variant<Data*, Instruction*> target;
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
    private:
      std::vector<std::tr1::shared_ptr<IFile>> _files;
      std::vector<Line*> _lines;
      
      boost::object_pool<Line> _linePool;
      boost::object_pool<Instruction> _instructionPool;
      boost::object_pool<Data> _dataPool;
      boost::object_pool<Label> _labelPool;
    };
  }
}

#endif