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
  struct Operand;
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

  struct Program
  {
  public:
    ~Program();
    
    Line* currentLine();
    std::string* currentFileName();
    
    void addNewLine();
    Label* addLabel(const std::string& labelName);
    void addData(Data* data);
    void addInstruction(Instruction* instruction);
    void addError(const std::string& message);
    
    void writeSymbols(std::ofstream& file);
    void writeBinary(std::ofstream& file);
  private:
    std::vector<std::string*> _fileNames;
    std::vector<Line*> _lines;
    std::map<std::string, Label*> _labels;
    std::vector<Data*> _dataLiterals;
    std::vector<Instruction*> _instructions;
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
    boost::variant<RegisterOperand, LiteralOperand, LabelOperand, ExpressionOperand> target;
  };
  
  struct Data
  {
    std::vector<uint16_t> value;
    boost::optional<uint16_t> computedAddress;
  };
  
  
  
  struct Instruction
  {
      Opcode opCode;
      boost::variant<RegisterOperand, LiteralOperand, LabelOperand, ExpressionOperand, DerefOperand, SpecialOperand> first;
      boost::variant<RegisterOperand, LiteralOperand, LabelOperand, ExpressionOperand, DerefOperand, SpecialOperand> second;
      boost::optional<uint16_t> computedAddress;
  };
 
}

#endif