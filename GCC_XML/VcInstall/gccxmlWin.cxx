/**
 * GCC-XML wrapper program for Windows.
 *
 * This is a really ugly hack implementation.  It should be replaced
 * with a more robust version supporting config file and environment
 * variable reading.
 */

#include "gxWinSystem.h"
#include "gxFlagsParser.h"

#include <iostream>
#include <fstream>
#include <errno.h>
#include <process.h>
#include <windows.h>
#include <direct.h>

#include <vector>

std::string findGccXmlCompiler();
std::string findGccXmlExecutable();
bool findGccXmlFlags(const std::string& gccxmlCompiler,
                     std::vector<std::string>& gccxmlFlags);
bool find_MSVC6_flags(std::vector<std::string>& gccxmlFlags);

int main(int argc, const char* argv[])
{
  const char* gccxmlCompilerC = 0;
  const char* gccxmlExecutableC = 0;
  bool dumpArguments = false;
  std::vector<std::string> gccxmlArguments;
  
  // Pull any options to this program off the command line.  Pass the
  // rest through to the list of gccxml arguments.
  {
  for(int i=1; i < argc;++i)
    {
    if(strcmp(argv[i], "-gccxml-compiler") == 0)
      {
      if(++i < argc)
        {
        gccxmlCompilerC = argv[i];
        }
      else
        {
        std::cerr << "Must give argument to -gccxml-compiler option.\n";
        return 1;
        }
      }
    else if(strcmp(argv[i], "-gccxml-executable") == 0)
      {
      if(++i < argc)
        {
        gccxmlExecutableC = argv[i];
        }
      else
        {
        std::cerr << "Must give argument to -gccxml-executable option.\n";
        return 1;
        }
      }
    else if(strcmp(argv[i], "-gccxml-dump-arguments") == 0)
      {
      dumpArguments = true;
      }
    else
      {
      gccxmlArguments.push_back(argv[i]);
      }
    }
  }

  // If the compiler was not given on the command line, find it.
  std::string gccxmlCompiler;
  if(gccxmlCompilerC) { gccxmlCompiler = gccxmlCompilerC; }
  else { gccxmlCompiler = findGccXmlCompiler(); }
  
  // If the gccxml executable was not given on the command line, find it.
  std::string gccxmlExecutable;
  if(gccxmlExecutableC) { gccxmlExecutable = gccxmlExecutableC; }
  else { gccxmlExecutable = findGccXmlExecutable(); }
  
  // Get the gccxml flags.
  std::vector<std::string> gccxmlFlags;
  if(!findGccXmlFlags(gccxmlCompiler, gccxmlFlags))
    {
    std::cerr << "Error getting GCCXML_FLAGS for compiler \""
              << gccxmlCompiler.c_str() << "\".\n";
    return 1;
    }
  
  // Combine the flags and arguments.
  {
  for(std::vector<std::string>::const_iterator a = gccxmlArguments.begin();
      a != gccxmlArguments.end(); ++a)
    {
    gccxmlFlags.push_back(*a);
    }
  }
  
  // Prepare the set of arguments for the real gccxml executable.
  const char** gccxmlArgs = new const char* [gccxmlFlags.size()+2];
  gccxmlArgs[0] = gccxmlExecutable.c_str();
  {
  for(unsigned int i=0; i < gccxmlFlags.size(); ++i)
    {
    gccxmlArgs[i+1] = gccxmlFlags[i].c_str();
    }
  gccxmlArgs[gccxmlFlags.size()+1] = 0;
  }

  if(dumpArguments)
    {
    for(const char** arg = gccxmlArgs; *arg ; ++arg)
      {
      std::cout << "arg = \"" << *arg << "\"\n";
      }
    }
  
  // Run the real executable.
  if(_execvp(gccxmlExecutable.c_str(), gccxmlArgs) < 0)
    {
    exit(errno);
    }
  
  return 0;
}

/** Find the compiler we are attempting to simulate.  */
std::string findGccXmlCompiler()
{
  // Look for MSVC 6.
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  const char* vcRegistry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";
  
  std::string vcPath;
  
  if(gxWinSystem::ReadRegistryValue(vcRegistry, vcPath))
    {
    vcPath += "/Bin/cl.exe";
    return vcPath;
    }
  
  return "<unknown>";
}

/** Find the gccxml_cc1plus executable.  */
std::string findGccXmlExecutable()
{
  // Just assume it will be found.  It is next to this executable.
  return "gccxml_cc1plus";
}

/** Find the GCCXML flags to simulate the given compiler.  */
bool findGccXmlFlags(const std::string& gccxmlCompiler,
                     std::vector<std::string>& gccxmlFlags)
{
  // Get the name of the compiler executable without a path or extension.
  std::string compiler = gccxmlCompiler;
  std::string::size_type pos = gccxmlCompiler.find_last_of("/\\");
  if(pos != std::string::npos)
    {
    compiler = gccxmlCompiler.substr(pos+1);
    }
  compiler = gxWinSystem::LowerCase(compiler);
  pos = compiler.rfind(".exe");
  if(pos != std::string::npos)
    {
    compiler = compiler.substr(0, pos);
    }

  // Check for MSVC 6.
  if(compiler == "cl")
    {
    return find_MSVC6_flags(gccxmlFlags);    
    }

  return false;
}

// Find gccxml flags for MSVC 6.
bool find_MSVC6_flags(std::vector<std::string>& gccxmlFlags)
{
  std::string gccxmlPath;
  // Must get our location.
  char fname[1024];
  ::GetModuleFileName(NULL, fname, 1023);
  std::string pname = fname;
  std::string::size_type pos = pname.find_last_of("/\\");
  if(pos != std::string::npos)
    {
    gccxmlPath = pname.substr(0, pos);
    }
  else
    {
    return false;
    }
  
  // Read the flags file.
  gccxmlPath += "/VcInclude/FLAGS.txt";
  
  std::ifstream flagsFile(gccxmlPath.c_str());
  if(!flagsFile) { return false; }
  
  char flagLine[4096];
  flagsFile.getline(flagLine, 4096);
  if(!flagsFile) { return false; }
  
  // Parse the list of flags.
  gxFlagsParser parser;
  parser.Parse(flagLine);
  parser.AddParsedFlags(gccxmlFlags);
  
  return true;
}

