#ifndef MRFUSION_ERROR_REPORTING_H
#define MRFUSION_ERROR_REPORTING_H
#include <string>

namespace MrFusion
{
  class IMessage
  {
  public:
    enum Importance
    {
      Low,
      Medium,
      High,
      Critical
    };
    virtual ~IMessage() = 0;
    virtual std::string message() = 0;
    virtual Importance importance() = 0;
  };
  
  class IPositionedMessage : IMessage
  {
  public:
    virtual ~IPositionedMessage() = 0;
    virtual std::string message() = 0;
    virtual IMessage::Importance importance() = 0;
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
    virtual IMessage::Importance importance();
    virtual std::tuple<int, int> lineAndColumn();
    virtual std::string fileName();
  }
  
  class Warning : public IPositionedMessage
  {
  private:
    std::string _baseMessage;
    std::string _fileName;
    int _line;
    int _column;
    IMessage::Importance _importance;
  public:
    Warning(const std::string& message, int line, int column, 
	    const std::string& fileName, IMessage::Importance importance);
    virtual ~Warning();
    virtual std::string message();
    virtual IMessage::Importance importance();
    virtual std::tuple<int, int> lineAndColumn();
    virtual std::string fileName();
  }
  
  class Info : public IMessage
  {
  private:
    std::string _message;
    IMessage::Importance _importance;
  public:
    Info(const std::string& message, IMessage::Importance importance);
    virtual ~Info();
    virtual std::string message();
    virtual IMessage::Importance importance();
  }
  
  class ErrorReporting
  {
  public:
    void minumumImportance(IMessage::Importance importance);
    IMessage::Importance minimumImportance();
    
    const std::vector<std::tr1::shared_ptr<IMessage>>& messages();
    void addError(const std::string& baseMessage, int line, int column, 
		  const std::string& fileName);
    void addWarning(const std::string& baseMessage, int line, int column, 
		    const std::string& fileName, IMessage::Importance importance);
  
    void addInfo(const std::string& message, IMessage::Importance importance);
  private:
    IMessage::Importance _importance;
    std::vector<std::tr1::shared_ptr<IMessage>> _messages;
  }
}

#endif