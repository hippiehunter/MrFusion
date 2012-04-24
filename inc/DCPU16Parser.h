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
    AsmParser();
    std::vector<std::string> parseIt(const std::string& data, const std::string& fileName);
    Program* ast();
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
  
  struct Message
  {
    enum Kind
    {
      Warning1,
      Warning2,
      Warning3,
      Warning4,
      Error,
      SevereError,
      Info
    };
    
    Kind kind;
    std::string message;
    std::string fileName;
    int filePosStart;
    int filePosEnd;
    
    std::string prettyMessage(Program* program);
  };
  
  struct Program
  {
  public:
    ~Program();
    Line* currentLine();
    std::string* currentFileName();
    
    void addNewFile(const std::string& fileName);
    Line* addNewLine();
    Label* addLabel(const std::string& labelName);
    Data* addData(std::vector<uint16_t> data);
    Instruction* addInstruction(Opcode op);
    
    void addError(const std::string& message, bool severe);
    void addWarning(const std::string& message, int level);
    void addInfo(const std::string& message);
    
  private:
    std::vector<std::string*> _fileNames;
    std::map<std::string*, std::vector<Line*>> _lines;
    std::map<std::string, Label*> _labels;
    std::vector<Data*> _dataLiterals;
    std::vector<Instruction*> _instructions;
    std::vector<Message*> _errors;
  };
 
}

#endif