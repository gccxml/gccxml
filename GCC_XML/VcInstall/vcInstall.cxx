/**
 * Program to copy and patch MSVC header files to work with GCC-XML.
 */

#include "gxSystemTools.h"

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
    if(!gxSystemTools::ReadRegistryValue(vcRegistry, sourcePath))
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
      gxSystemTools::FileCopy(source.c_str(), dest.c_str());
      }
    }

  destPath = "\""+destPath+"\"";
  patchFile = "\""+patchFile+"\"";

  // Find the patch executable.
  std::string patchCommand = "patch.exe";
  if(!gxSystemTools::FileExists(patchCommand.c_str()))
    {
    // The registry key to use to find the patch executable.
    const char* cygwinRegistry =
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/bin;native";
    if(gxSystemTools::ReadRegistryValue(cygwinRegistry, patchCommand) &&
       gxSystemTools::FileExists((patchCommand+"/patch.exe").c_str()))
      {
      // Found the binary location from cygwin's registry entry.
      patchCommand += "/patch.exe";
      }
    else
      {
      // Just hope the command will be found in the path.
      patchCommand = "patch";
      }
    }
  
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
  
  std::cout << "Executing " << patchCommand.c_str() << std::endl;
  // Patch the copies of the header files.
  if(_execvp(patchCommand.c_str(), patchOptions) < 0)
    {
    exit(errno);
    }

  return 0;
}
