#ifndef DCPU16_PARSER
#define DCPU16_PARSER

#include <vector>
#include <map>
#include <string>
#include <boost/variant/variant.hpp>
#include <boost/optional.hpp>

namespace Parser
{
  struct Program;
  struct Line;
  struct Label;
  struct Data;
  struct Instruction;
  
  class AsmParser
  {
  public:
    ~AsmParser();
    Program* parseIt(std::string data);
    
  private:
    Program* program;
  };
  
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
    std::string* fileName;
  };
  
  struct Label
  {
    Line* line;
    Instruction* targetInstruction;
    Data* targetData;
    boost::optional<uint16_t> computedAddress;
  };
  
  struct SpecialOperand
  {
    SpecialOperandType value;
  };
  
  struct RegisterOperand
  {
    RegisterOperand() {}
    RegisterOperand(Register reg_) : reg(reg_) {}
    Register reg;
  };
  
  struct LiteralOperand
  {
    uint16_t value;
  };
  
  struct LabelOperand
  {
    Label* target;
  };
  
  enum ExpressionKind
  {
    Plus,
    Minus
  };
  
  struct ExpressionOperand
  {
    boost::variant<RegisterOperand, LiteralOperand, LabelOperand> first;
    boost::variant<RegisterOperand, LiteralOperand, LabelOperand> second;
    ExpressionKind kind;
  };
  
  struct DerefOperand
  {
    DerefOperand() {}
    DerefOperand(Register reg)
    {
      target = RegisterOperand(reg);
    }
    boost::variant<RegisterOperand, LiteralOperand, LabelOperand, ExpressionOperand> target;
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
    Operand second;
    boost::optional<uint16_t> computedAddress;
  };
  
  struct Program
  {
  public:
    ~Program();
    
    Line* currentLine();
    std::string* currentFileName();
    
    Line* addNewLine();
    Label* addLabel(const std::string& labelName);
    Data* addData(std::vector<uint16_t> data);
    Data* addData(std::string data);
    Instruction* addInstruction(Opcode op, Operand& opr1);
    Instruction* addInstruction(Opcode op, Operand& opr1, Operand& opr2);
    void addError(const std::string& message);
    
    void writeSymbols(std::ofstream& file);
    void writeBinary(std::ofstream& file);
  private:
    std::vector<boost::variant<Line*, Label*, Data*, Instruction*>> _parsedStream;
    std::vector<std::string*> _fileNames;
    std::vector<Line*> _lines;
    std::map<std::string, Label*> _labels;
    std::vector<Data*> _dataLiterals;
    std::vector<Instruction*> _instructions;
  };
 
}

#endif