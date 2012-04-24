#include "ErrorReporting.h"

#include <string>
#include <tr1/shared_ptr.h>
#include <vector>

using std::string;
using std::vector;
using std::tr1::shared_ptr;

using MrFusion::IMessage;
using MrFusion::IPositionedMessage;
using MrFusion::ErrorReporting;
using MrFusion::Error;
using MrFusion::Warning;
using MrFusion::Info;


void ErrorReporting::minumumImportance(IMessage::Importance importance)
{
}

IMessage::Importance ErrorReporting::minimumImportance()
{
}

vector<shared_ptr<IMessage>> ErrorReporting::messages()
{
}

void ErrorReporting::addError(const string& baseMessage, int line, int column, 
	      const string& fileName)
{
}

void ErrorReporting::addWarning(const string& baseMessage, int line, int column, 
		const string& fileName, IMessage::Importance importance)
{
}

void ErrorReporting::addInfo(const string& message, IMessage::Importance importance)
{
}