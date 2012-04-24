#ifndef MRFUSION_IFILE
#define MRFUSION_IFILE

#include <tuple>
#include <string>
#include <tr1/shared_ptr.h>


namespace MrFusion
{
  class IFile
  {
  public:
    const std::string& fileName() = 0;
    std::tuple<int, int> lineAndColumnFromPos(int position) = 0;
    std::tr1::shared_ptr<std::string> contents() = 0;
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