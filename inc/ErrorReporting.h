#ifndef MRFUSION_ERROR_REPORTING_H
#define MRFUSION_ERROR_REPORTING_H
#include <string>
#include <vector>
#include <tr1/memory>

namespace MrFusion
{
  enum MessageImportance
  {
    Low,
    Medium,
    High,
    Critical
  };
    
  class IMessage
  {
  public:
    virtual ~IMessage() = 0;
    virtual std::string message() = 0;
    virtual MessageImportance importance() = 0;
  };
  
  class IPositionedMessage : public IMessage
  {
  public:
    virtual ~IPositionedMessage() = 0;
    virtual std::string message() = 0;
    virtual MessageImportance importance() = 0;
    virtual std::tuple<int, int> lineAndColumn() = 0;
    virtual std::string fileName() = 0;
  };
  
  class Error : public IPositionedMessage
  {
  private:
    std::string _baseMessage;
    std::string _fileName;
    int _line;
    int _column;
  public:
    Error(const std::string& baseMessage, int line, int column, const std::string& fileName);
    virtual ~Error();
    virtual std::string message();
    virtual MessageImportance importance();
    virtual std::tuple<int, int> lineAndColumn();
    virtual std::string fileName();
  };
  
  class Warning : public IPositionedMessage
  {
  private:
    std::string _baseMessage;
    std::string _fileName;
    int _line;
    int _column;
    MessageImportance _importance;
  public:
    Warning(const std::string& message, int line, int column, 
	    const std::string& fileName, MessageImportance importance);
    virtual ~Warning();
    virtual std::string message();
    virtual MessageImportance importance();
    virtual std::tuple<int, int> lineAndColumn();
    virtual std::string fileName();
  };
  
  class Info : public IMessage
  {
  private:
    std::string _message;
    MessageImportance _importance;
  public:
    Info(const std::string& message, MessageImportance importance);
    virtual ~Info();
    virtual std::string message();
    virtual MessageImportance importance();
  };
  
  class ErrorReporting
  {
  public:
    void minumumImportance(MessageImportance importance);
    MessageImportance minimumImportance();
    
    const std::vector<std::tr1::shared_ptr<IMessage>>& messages();
    void addError(const std::string& baseMessage, int line, int column, 
		  const std::string& fileName);
    void addWarning(const std::string& baseMessage, int line, int column, 
		    const std::string& fileName, MessageImportance importance);
  
    void addInfo(const std::string& message, MessageImportance importance);
  private:
    MessageImportance _importance;
    std::vector<std::tr1::shared_ptr<IMessage>> _messages;
  };
}

#endif