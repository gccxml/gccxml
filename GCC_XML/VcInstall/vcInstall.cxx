/**
 * Program to copy and patch MSVC header files to work with GCC-XML.
 */

#include "gxWinSystem.h"

#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>
#include <process.h>
#include <windows.h>
#include <direct.h>

int main(int argc, char* argv[])
{
  if(argc < 3)
    {
    std::cout << "Usage:" << std::endl
              << "  " << argv[0] << " patch_file destination_path [source_path]" << std::endl
              << "The source_path will be guessed from the registry if it is not given." << std::endl
              << "All arguments should be given as full paths." << std::endl;
    return 0;
    }

  std::string patchFile = argv[1];
  std::string destPath = argv[2];
  
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  const char* vcRegistry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";
  
  std::string sourcePath;

  // If the source_path argument was not given, look it up in the registry.
  if(argc < 4)
    {
    if(!gxWinSystem::ReadRegistryValue(vcRegistry, sourcePath))
      {
      std::cerr << "source_path not specified and VC98 could not be found in the registry!" << std::endl;
      return 1;
      }
    sourcePath += "/Include";
    }
  else
    {
    sourcePath = argv[3];
    }

  // Make sure the destination path exists before trying to put files
  // there.
  _mkdir(destPath.c_str());
  
  // Look at the patch file to see what headers need to be copied.
  std::ifstream patch(patchFile.c_str());
  if(!patch)
    {
    std::cerr << "Error opening patch file." << std::endl;
    return 1;
    }
  
  char buf[4096];
  for(patch.getline(buf, 4096); !patch.eof(); patch.getline(buf, 4096))
    {
    std::string line = buf;
    if(line.substr(0,6) == "Index:")
      {
      std::string source = sourcePath;
      source += "/"+line.substr(7);
      std::string dest = destPath.c_str();
      dest += "/"+line.substr(7);
      gxWinSystem::gxCopyFile(source.c_str(), dest.c_str());
      }
    }

  std::string msvcFlagsFile = destPath+"/FLAGS.txt";
  std::ofstream msvcFlags(msvcFlagsFile.c_str());
  if(msvcFlags)
    {
    msvcFlags <<
      "-quiet -o /dev/null -nostdinc -I- -w -fsyntax-only "
      "-D__stdcall= -D__cdecl= -D_stdcall= -D_cdecl= -D__declspec(x)= "
      "-D_inline=inline -D__uuidof(x)=IID() -D__int64='long long' "
      "-D__cplusplus "
      "-D_MSC_VER=1200 -D_MSC_EXTENSIONS "
      "-D_WIN32 -D_M_IX86 -D_WCHAR_T_DEFINED "
      "-DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT -DSHSTDAPI_(x)=x ";
      msvcFlags <<
        " -I\"" << destPath.c_str() << "\"" <<
        " -I\"" << sourcePath.c_str() << "\"";
      msvcFlags << std::endl;
    }
  else
    {
    std::cerr << "Error opening MSVC flags FLAGS readme file, skipping: "
              << msvcFlagsFile.c_str() << std::endl;
    }
  
  destPath = "\""+destPath+"\"";
  patchFile = "\""+patchFile+"\"";
  
  // The arguments for executing the patch program.
  const char* patchOptions[] =
  {
    "patch",
    "-p0",
    "-t",
    "-d ", destPath.c_str(),
    "-i ", patchFile.c_str(),
    0
  };
  
  // Patch the copies of the header files.
  if(_execvp("patch", patchOptions) < 0)
    {
    exit(errno);
    }

  return 0;
}
