/**
 * Program to copy and patch MSVC header files to work with GCC-XML.
 */

#include "gxSystemTools.h"

#include <direct.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <process.h>
#include <string>
#include <vector>
#include <windows.h>

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
  
  // Look at the patch file to see what headers need to be copied.
  // Also extract the version of Visual Studio for which this patch is
  // intended.
  std::ifstream patch(patchFile.c_str());
  if(!patch)
    {
    std::cerr << "Error opening patch file." << std::endl;
    return 1;
    }
  
  std::string vcVersion;
  std::vector<std::string> files;
  char buf[4096];
  for(patch.getline(buf, 4096); !patch.eof(); patch.getline(buf, 4096))
    {
    std::string line = buf;
    if(line.substr(0,6) == "Index:")
      {
      files.push_back(line.substr(7));
      }
    else if((vcVersion.length() == 0) && (line.substr(0, 29) == "RCS file: /cvsroot/GxInclude/"))
      {
      vcVersion = line.substr(29);
      }
    }

  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  const char* vc6Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";
  const char* vc7Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.0;InstallDir";
  // "C:\Program Files\Microsoft Visual Studio .NET\Common7\IDE\"
  
  // If the source_path argument was not given, look it up in the registry.
  std::string sourcePath;  
  if(argc < 4)
    {  
    std::string spLookup;
    if(vcVersion.substr(0, 9) == "VcInclude")
      {
      if(!gxSystemTools::ReadRegistryValue(vc6Registry, spLookup))
        {
        std::cerr << "source_path not specified "
                  << "and MSVC6 could not be found in the registry!"
                  << std::endl;
        return 1;
        }
      spLookup += "/Include";
      }
    else if(vcVersion.substr(0, 11) == "Vc7/Include")
      {
      if(!gxSystemTools::ReadRegistryValue(vc7Registry, spLookup))
        {
        std::cerr << "source_path not specified "
                  << "and MSVC7 could not be found in the registry!"
                  << std::endl;
        return 1;
        }
      spLookup += "/../../Vc7/Include";
      }
    else if(vcVersion.substr(0, 15) == "Vc7/PlatformSDK")
      {
      if(!gxSystemTools::ReadRegistryValue(vc7Registry, spLookup))
        {
        std::cerr << "source_path not specified "
                  << "and MSVC7 could not be found in the registry!"
                  << std::endl;
        return 1;
        }
      spLookup += "/../../Vc7/PlatformSDK/Include";
      }
    else
      {
      std::cerr << "source_path not specified "
                << "and cannot determine MSVC version from patch file!\n";
      return 1;
      }
    
    // Collapse to a simplified form of the directory.
    sourcePath = gxSystemTools::CollapseDirectory(spLookup.c_str());
    }
  else
    {
    // Use the directory as specified on the command line.
    sourcePath = argv[3];
    }

  // Make sure the destination path exists before trying to put files
  // there.
  gxSystemTools::MakeDirectory(destPath.c_str());
  
  // Copy the files over before patching.
  for(unsigned int i=0; i < files.size(); ++i)
    {
    std::string source = sourcePath;
    source += "/"+files[i];
    std::string dest = destPath;
    dest += "/"+files[i];
    gxSystemTools::FileCopy(source.c_str(), dest.c_str());
    }
  
  destPath = "\""+destPath+"\"";  patchFile = "\""+patchFile+"\"";

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
