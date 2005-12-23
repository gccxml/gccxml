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

bool InstallSupport(const char* patchCommand, const char* catCommand,
                    const char* patchFile, const char* sourcePath,
                    const char* destPath);
bool FindTool(const char* vcDir, const char* name, std::string& result);

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  if(argc < 2)
    {
    std::cout << "Usage:" << std::endl
              << "  " << argv[0]
              << " patch_dir [gccxml_root] [timestamp_file]" << std::endl;
    return 0;
    }

  std::string patchDir = argv[1];
  std::string gccxmlRoot = ".";
  std::string timestamp;
  if(argc >= 3)
    {
    gccxmlRoot = argv[2];

    // Make sure the output directory exists.
    gxSystemTools::MakeDirectory(gccxmlRoot.c_str());
    }
  if(argc >= 4)
    {
    timestamp = argv[3];
    }

  // Clean up the paths.
  patchDir = gxSystemTools::CollapseDirectory(patchDir.c_str());
  gccxmlRoot = gxSystemTools::CollapseDirectory(gccxmlRoot.c_str());
  gxSystemTools::ConvertToUnixSlashes(patchDir);
  gxSystemTools::ConvertToUnixSlashes(gccxmlRoot);

  // The registry keys for MSVC install detection.
  const char* vc6Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\"
    "DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";
  const char* vc7Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.0;InstallDir";
  const char* vc71Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.1;InstallDir";
  const char* vc8Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;InstallDir";

  // Check which versions of MSVC are installed.
  std::string msvc6;
  std::string msvc7;
  std::string msvc71;
  std::string msvc8;
  bool have6 = gxSystemTools::ReadRegistryValue(vc6Registry, msvc6);
  bool have7 = gxSystemTools::ReadRegistryValue(vc7Registry, msvc7);
  bool have71 = gxSystemTools::ReadRegistryValue(vc71Registry, msvc71);
  bool have8 = false;

  // Look for a VS8 that is not the beta release.
  if(gxSystemTools::ReadRegistryValue(vc8Registry, msvc8))
    {
    // The "CLR Version" registry entry in VS 8 has value "v2.0.40607"
    // for the beta and "v2.0.50727" for the release.
    const char* vc8RegistryVersion =
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;CLR Version";
    std::string version;
    if(gxSystemTools::ReadRegistryValue(vc8RegistryVersion, version))
      {
      int vnum;
      if((sscanf(version.c_str(), "v2.0.%d", &vnum) == 1) &&
         vnum >= 50727)
        {
        have8 = true;
        }
      }
    }

  // See if there is anything to do.
  if(!have6 && !have7 && !have71 && !have8)
    {
    std::cout << "None of MSVC 6, 7, 7.1, or 8 is installed.\n";
    }

  // Need to install at least one of the support directories.  We need
  // to find the cat and patch executables.
  std::string patchCommand;
  if(!FindTool(patchDir.c_str(), "patch", patchCommand) &&
     (have6||have7||have71||have8))
    {
    std::cerr << "Cannot find patch executable.\n";
    return 1;
    }
  std::string catCommand;
  if(!FindTool(patchDir.c_str(), "cat", catCommand) &&
     (have6||have7||have71||have8))
    {
    std::cerr << "Cannot find cat executable.\n";
    return 1;
    }

  int result = 0;
  if(have6)
    {
    msvc6 += "/Include";
    std::string patchFile = patchDir + "/vc6Include.patch";
    std::string destPath = gccxmlRoot+"/Vc6/Include";
    if(gxSystemTools::FileExists(patchFile.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchFile.c_str(), msvc6.c_str(), destPath.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 6, but cannot find vc6Include.patch.\n";
      result = 1;
      }
    }

  if(have7)
    {
    std::string msvc7i = msvc7 + "/../../Vc7/Include";
    std::string msvc7p = msvc7 + "/../../Vc7/PlatformSDK/Include";
    msvc7i = gxSystemTools::CollapseDirectory(msvc7i.c_str());
    msvc7p = gxSystemTools::CollapseDirectory(msvc7p.c_str());
    std::string patchI = patchDir + "/vc7Include.patch";
    std::string patchP = patchDir + "/vc7PlatformSDK.patch";
    std::string destPathI = gccxmlRoot+"/Vc7/Include";
    std::string destPathP = gccxmlRoot+"/Vc7/PlatformSDK";
    if(gxSystemTools::FileExists(patchI.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchI.c_str(), msvc7i.c_str(), destPathI.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 7, but cannot find vc7Include.patch.\n";
      result = 1;
      }
    if(gxSystemTools::FileExists(patchP.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchP.c_str(), msvc7p.c_str(), destPathP.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 7, but cannot find vc7PlatformSDK.patch.\n";
      result = 1;
      }
    }
  if(have71)
    {
    std::string msvc71i = msvc71 + "/../../Vc7/Include";
    std::string msvc71p = msvc71 + "/../../Vc7/PlatformSDK/Include";
    msvc71i = gxSystemTools::CollapseDirectory(msvc71i.c_str());
    msvc71p = gxSystemTools::CollapseDirectory(msvc71p.c_str());
    std::string patchI = patchDir + "/vc71Include.patch";
    std::string patchP = patchDir + "/vc71PlatformSDK.patch";
    std::string destPathI = gccxmlRoot+"/Vc71/Include";
    std::string destPathP = gccxmlRoot+"/Vc71/PlatformSDK";
    if(gxSystemTools::FileExists(patchI.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchI.c_str(), msvc71i.c_str(), destPathI.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 7.1, but cannot find vc71Include.patch.\n";
      result = 1;
      }
    if(gxSystemTools::FileExists(patchP.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchP.c_str(), msvc71p.c_str(), destPathP.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 7.1, but cannot find vc71PlatformSDK.patch.\n";
      result = 1;
      }
    }
  if(have8)
    {
    std::string msvc8i = msvc8 + "/../../Vc/Include";
    std::string msvc8p = msvc8 + "/../../Vc/PlatformSDK/Include";
    msvc8i = gxSystemTools::CollapseDirectory(msvc8i.c_str());
    msvc8p = gxSystemTools::CollapseDirectory(msvc8p.c_str());
    std::string patchI = patchDir + "/vc8Include.patch";
    std::string patchP = patchDir + "/vc8PlatformSDK.patch";
    std::string destPathI = gccxmlRoot+"/Vc8/Include";
    std::string destPathP = gccxmlRoot+"/Vc8/PlatformSDK";
    if(gxSystemTools::FileExists(patchI.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchI.c_str(), msvc8i.c_str(), destPathI.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 8, but cannot find vc8Include.patch.\n";
      result = 1;
      }
    if(gxSystemTools::FileExists(patchP.c_str()))
      {
      if(!InstallSupport(patchCommand.c_str(), catCommand.c_str(),
                         patchP.c_str(), msvc8p.c_str(), destPathP.c_str()))
        {
        result = 1;
        }
      }
    else
      {
      std::cerr << "Have MSVC 8, but cannot find vc8PlatformSDK.patch.\n";
      result = 1;
      }
    }

  // If we succeeded, write the timestamp file.
  if(result == 0 && (timestamp.length() > 0))
    {
    std::ofstream tfile(timestamp.c_str(), std::ios::out | std::ios::binary);
    tfile << "int main() { return 0; }\n";
    if(!tfile)
      {
      std::cerr << "Error writing timestamp file \""
                << timestamp.c_str() << "\".\n";
      result = 1;
      }
    }

  return result;
}

//----------------------------------------------------------------------------
bool InstallSupport(const char* patchCommand, const char* catCommand,
                    const char* patchFile, const char* sourcePath,
                    const char* destPath)
{
  // Look at the patch file to see what headers need to be copied.
  std::ifstream patch(patchFile);
  if(!patch)
    {
    std::cerr << "Error opening patch file " << patchFile << std::endl;
    return false;
    }

  // Make sure the destination path exists before trying to put files
  // there.
  gxSystemTools::MakeDirectory(destPath);

  // Copy the files over before patching.
  char buf[4096];
  for(patch.getline(buf, 4096); !patch.eof(); patch.getline(buf, 4096))
    {
    std::string line = buf;
    if(line.substr(0,6) == "Index:")
      {
      std::string source = sourcePath;
      source += "/"+line.substr(7);
      std::string dest = destPath;
      dest += "/"+line.substr(7);
      gxSystemTools::FileCopy(source.c_str(), dest.c_str());
      }
    }
  std::string cmd = catCommand;
  if(cmd.find(" ") != cmd.npos)
    {
    if(gxSystemTools::GetShortPath(catCommand, cmd))
      {
      catCommand = cmd.c_str();
      }
    }
  std::string patchCmd = gxSystemTools::ConvertToOutputPath(catCommand);
  patchCmd += " ";
  patchCmd += gxSystemTools::ConvertToOutputPath(patchFile);
  patchCmd += " | ";
  patchCmd += gxSystemTools::ConvertToOutputPath(patchCommand);
  patchCmd += " -p0 -t -d ";
  patchCmd += gxSystemTools::ConvertToOutputPath(destPath);

  // Patch the copies of the header files.
  std::cout << "Executing " << patchCmd.c_str() << std::endl;
  std::string output;
  int retVal;
  if(gxSystemTools::RunCommand(patchCmd.c_str(), output, retVal) &&
     (retVal == 0))
    {
    return true;
    }

  std::cerr << "Error running patch.  Output is ["
            << output.c_str() << "]\n";
  return false;
}

//----------------------------------------------------------------------------
bool FindTool(const char* vcDir, const char* name, std::string& result)
{
  // check for executable in the source directory
  std::string command = vcDir;
  command += "/vc";
  command += name;
  command += ".exe";
  if(gxSystemTools::FileExists(command.c_str()))
    {
    result = command;
    return true;
    }
  // Find the executable.
  command = name;
  command += ".exe";
  if(gxSystemTools::FileExists(command.c_str()))
    {
    result = command;
    return true;
    }
  else
    {
    // The registry key to use to find the executable from cygwin.
    const char* cygwinRegistry1 =
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/bin;native";
    const char* cygwinRegistry2 =
      "HKEY_CURRENT_USER\\Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/bin;native";
    if((gxSystemTools::ReadRegistryValue(cygwinRegistry1, command) ||
        gxSystemTools::ReadRegistryValue(cygwinRegistry2, command)) &&
       gxSystemTools::FileExists((command+"/"+name+".exe").c_str()))
      {
      // Found the binary location from cygwin's registry entry.
      command += "/";
      command += name;
      command += ".exe";
      result = command;
      return true;
      }
   else
      {
      // Try to find it in the path.
      command = gxSystemTools::FindProgram(name);
      if(command.length() > 0)
        {
        result = command;
        return true;
        }
      }
    }
  return false;
}
