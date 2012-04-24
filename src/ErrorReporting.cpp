#include "ErrorReporting.h"

#include <string>
#include <tr1/shared_ptr.h>
#include <vector>
#include <tuple>

#include <boost/format.hpp>


using std::string;
using std::vector;
using std::tr1::shared_ptr;
using std::tuple;

using boost::format;

using MrFusion::IMessage;
using MrFusion::IPositionedMessage;
using MrFusion::ErrorReporting;
using MrFusion::Error;
using MrFusion::Warning;
using MrFusion::Info;


void ErrorReporting::minumumImportance(IMessage::Importance importance)
{
  _importance = importance;
}

IMessage::Importance ErrorReporting::minimumImportance()
{
  return _importance;
}

vector<shared_ptr<IMessage>> ErrorReporting::messages()
{
  return _messages;
}

void ErrorReporting::addError(const string& baseMessage, int line, int column, 
	      const string& fileName)
{
  _messages.push_back(shared_ptr<IMessage>(new Error(baseMessage, line, column, fileName)));
}

void ErrorReporting::addWarning(const string& baseMessage, int line, int column, 
		const string& fileName, IMessage::Importance importance)
{
  _messages.push_back(shared_ptr<IMessage>(new Warning(baseMessage, line, column, fileName, importance)));
}

void ErrorReporting::addInfo(const string& message, IMessage::Importance importance)
{
  _messages.push_back(shared_ptr<IMessage>(new Info(message, importance)));
}


Error::Error(const string& baseMessage, int line, int column, 
	     const string& fileName)
{
  _baseMessage = baseMessage;
  _fileName = fileName;
  _line = line;
  _column = column;
}

Error::~Error() { }

string Error::message() 
{
  return _baseMessage;
}
IMessage::Importance Error::importance()
{
  return _importance;
}

string Error::fileName()
{
  return _fileName;
}

tuple<int, int> Error::lineAndColumn()
{
  return tuple<int, int>(_line, _column);
}

Warning::~Warning() { }

Warning::Warning(const string& baseMessage, int line, int column,
		 const string& fileName, IMessage::Importance importance)
{
  _baseMessage = baseMessage;
  _line = line;
  _column = column;
  _importance = importance;
}

string Warning::message()
{
  return _baseMessage;
}

tuple<int, int> Warning::lineAndColumn()
{
  return tuple<int, int>(_line, _column);
}

string Warning::fileName()
{
  return _fileName;
}

IMessage::Importance Warning::importance()
{
  return _importance;
}

Info::~Info() { }
Info::Info(string message, IMessage::Importance importance)
{
  _message = message;
  _importance = importance;
}

string Info::message()
{
  return _message;
}

IMessage::Importance Info::importance()
{
  return _importance;
}
