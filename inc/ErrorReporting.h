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
    virtual std::string message() = 0;
    virtual Importance importance() = 0;
  };
  
  class IPositionedMessage : IMessage
  {
  public:
    virtual std::string message() = 0;
    virtual IMessage::Importance importance() = 0;
    virtual std::tuple<int, int> lineAndColumn() = 0;
  };
  
  class Error : public IPositionedMessage
  {
  public:
    virtual std::string message();
    virtual IMessage::Importance importance();
    virtual std::tuple<int, int> lineAndColumn();
  }
  
  class Warning : public IPositionedMessage
  {
  public:
    virtual std::string message();
    virtual IMessage::Importance importance();
    virtual std::tuple<int, int> lineAndColumn();
  }
  
  class Info : public IMessage
  {
  public:
    virtual std::string message();
    virtual IMessage::Importance importance();
  }
  
  class ErrorReporting
  {
  public:
    void minumumImportance(IMessage::Importance importance);
    IMessage::Importance minimumImportance();
    
    std::vector<std::tr1::shared_ptr<IMessage>> messages();
    void addError(const std::string& baseMessage, int line, int column, 
		  const std::string& fileName);
    void addWarning(const std::string& baseMessage, int line, int column, 
		    const std::string& fileName, IMessage::Importance importance);
  
    void addInfo(const std::string& message, IMessage::Importance importance);
  }
}

#endif