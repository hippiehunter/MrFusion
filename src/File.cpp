#include "IFile.h"

#include <string>
#include <vector>
#include <tr1/shared_ptr.h>
#include <fstream>

using std::tr1::shared_ptr;
using std::string;
using std::vector;
using std::ifstream;
using MrFusion::IFile;
using MrFusion::IFileFactory;


class SimpleFile : public IFile
{
public:
  SimpleFile(const std::string& fileName,
	     shared_ptr<string> contents)
  {
    _fileName = fileName;
    _contents = contents;
    
    int filePos = 0;
    for(auto ch : *contents)
    {
      //not looking for mac encoding here, only windows and linux
      if(ch == '\n')
      {
	//building a list of line ends so that we can 
	//work with positions instead of file and column
	//then show the user a more friendly message
	_lineEnds.push_back(++filePos);
      }
      else
	filePos++;
    }
  }
  
  const string& fileName()
  {
    return _fileName;
  }
  
  tuple<int, int> lineAndColumnFromPos(int position)
  {
    //find the line index so we can get its start
    auto lineNumber = binarySearchForLineRange(0, _lineEnds.size() - 1, position);
    //if we're already at the begining of the line the start is 0
    int lineStart = lineNumber == 0 ? 0 : (_lineEnds[lineNumber - 1] + 1);
    auto column = position - lineStart;
    return tuple<int,int>(lineNumber, column);
  }
  
  shared_ptr<string> contents()
  {
    return _contents;
  }
private:
  string _fileName;
  shared_ptr<string> _contents;
  vector<uint32_t> _lineEnds;
  
  size_t binarySearchForLineRange(size_t start, size_t end, uint32_t target)
  {
    size_t midPos = 0;
    //while there is more then one line between us and our target range
    //we can just bump in the start/end to midPos
    while(midPos = (end - start) / 2)
    {
      if(_lineEnds[midPos + start] >= target)
	start = midPos + start;
      else
	end = midPos + start;
    }
    //we've found it, lets see what side its on
    if(_lineEnds[start] >= target)
      return start;
    else if(_lineEnds[end] <= target)
      return end;
    else
      return start + 1;
  }
};

shared_ptr<IFile> IFileFactory::makeFile(const string& fileName)
{
  ifstream file;
  file.open(str);
  shared_ptr<string> contents(new string());
  (*contents) << file;
  return IFileFactory::makeFile(fileName, contents);
}

shared_ptr<IFile> IFileFactory::makeFile(const string& fileName,
					 shared_ptr<string> contents)
{
  return shared_ptr<IFile>(new SimpleFile(fileName, contents));
}

