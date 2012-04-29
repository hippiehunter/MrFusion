#ifndef MRFUSION_IFILE
#define MRFUSION_IFILE

#include <tuple>
#include <string>
#include <tr1/memory>


namespace MrFusion
{
  class IFile
  {
  public:
    virtual ~IFile() = 0;
    virtual const std::string& fileName() = 0;
    virtual std::tuple<int, int> lineAndColumnFromPos(int position) = 0;
    virtual int posFromLineAndColumn(int line, int column) = 0;
    virtual std::tr1::shared_ptr<std::string> contents() = 0;
  };
  
  class IFileFactory
  {
  public:
    static std::tr1::shared_ptr<IFile> makeFile(const std::string& fileName);
    static std::tr1::shared_ptr<IFile> makeFile(const std::string& fileName,
						std::tr1::shared_ptr<std::string>& contents);
    
  };
}

#endif