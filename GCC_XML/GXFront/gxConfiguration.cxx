/*=========================================================================

  Program:   GCC-XML
  Module:    gxConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gxConfiguration.h"

//----------------------------------------------------------------------------
gxConfiguration::gxConfiguration()
{
  m_HelpFlag = false;
  m_VersionFlag = false;
  m_PrintFlag = false;
  m_DebugFlag = false;
  m_ManFlag = false;
  m_CopyrightFlag = false;
  m_HelpHTMLFlag = false;
  m_HaveGCCXML_CXXFLAGS = false;
}

//----------------------------------------------------------------------------
bool gxConfiguration::Configure(int argc, const char*const * argv)
{
  // Find our working paths.
  this->FindRoots(argv[0]);
  
  // Process the command line.
  if(!this->ProcessCommandLine(argc, argv)) { return false; }
  
  // If a config file was not specified on the command line, check the
  // environment now.
  bool checkedEnvironment;
  if(m_GCCXML_CONFIG.length() == 0)
    {
    this->CheckEnvironment();
    checkedEnvironment = true;
    }
  
  // Read the configuration file if it exists.
  if(!this->CheckConfigFile()) { return false; }
  
  // If we didn't earlier check the environment, do it now.
  if(!checkedEnvironment)
    {
    this->CheckEnvironment();
    }
  
  // Check the alternative environment settings for "CXX" and
  // "CXXFLAGS" as a last resort to get the compiler setting.
  this->CheckCxxEnvironment();
  
  // If no root has been set, use the data root.
  if(m_GCCXML_ROOT.length() == 0)
    {
    m_GCCXML_ROOT = m_DataRoot;
    }
  
  // If no executable has been set, use the one in the executable root.
  if(m_GCCXML_EXECUTABLE.length() == 0)
    {
    m_GCCXML_EXECUTABLE = m_ExecutableRoot+"/gccxml_cc1plus";
    }
  
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_ROOT);
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_EXECUTABLE);
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_CONFIG);
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_COMPILER);

  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::ConfigureFlags()
{
  if(!this->CheckFlags())
    {
    return false;
    }
  return true;
}
  
//----------------------------------------------------------------------------
void gxConfiguration::PrintConfiguration(std::ostream& os) const
{
  os << "Configuration settings:\n"
     << "  GCCXML_CONFIG=\"" << m_GCCXML_CONFIG.c_str() << "\"\n"
     << "  GCCXML_COMPILER=\"" << m_GCCXML_COMPILER.c_str() << "\"\n"
     << "  GCCXML_CXXFLAGS=\"" << m_GCCXML_CXXFLAGS.c_str() << "\"\n"
     << "  GCCXML_EXECUTABLE=\"" << m_GCCXML_EXECUTABLE.c_str() << "\"\n"
     << "  GCCXML_FLAGS=\"" << m_GCCXML_FLAGS.c_str() << "\"\n"
     << "  GCCXML_USER_FLAGS=\"" << m_GCCXML_USER_FLAGS.c_str() << "\"\n"
     << "  GCCXML_ROOT=\"" << m_GCCXML_ROOT.c_str() << "\"\n";
}

//----------------------------------------------------------------------------
const std::vector<std::string>& gxConfiguration::GetArguments() const
{
  return m_Arguments;
}

//----------------------------------------------------------------------------
void gxConfiguration::AddArguments(std::vector<std::string>& arguments) const
{
  for(std::vector<std::string>::const_iterator i=m_Arguments.begin();
      i != m_Arguments.end(); ++i)
    {
    arguments.push_back(*i);
    }
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetHelpFlag() const
{
  return m_HelpFlag;
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetVersionFlag() const
{
  return m_VersionFlag;
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetPrintFlag() const
{
  return m_PrintFlag;
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetDebugFlag() const
{
  return m_DebugFlag;
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetManFlag() const
{
  return m_ManFlag;
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetCopyrightFlag() const
{
  return m_CopyrightFlag;
}

//----------------------------------------------------------------------------
bool gxConfiguration::GetHelpHTMLFlag() const
{
  return m_HelpHTMLFlag;
}

//----------------------------------------------------------------------------
const std::string& gxConfiguration::GetGCCXML_EXECUTABLE() const
{
  return m_GCCXML_EXECUTABLE;
}
  
//----------------------------------------------------------------------------
const std::string& gxConfiguration::GetGCCXML_FLAGS() const
{
  return m_GCCXML_FLAGS;
}
  
//----------------------------------------------------------------------------
const std::string& gxConfiguration::GetGCCXML_USER_FLAGS() const
{
  return m_GCCXML_USER_FLAGS;
} 

//----------------------------------------------------------------------------
void gxConfiguration::FindRoots(const char* argv0)
{
  // Find our own executable's location.
  std::string av0 = argv0;
  gxSystemTools::ConvertToUnixSlashes(av0);
  std::string::size_type pos = av0.find_last_of("/");
  if(pos == std::string::npos)
    {
    av0 = gxSystemTools::FindProgram(argv0);
    pos = av0.find_last_of("/");
    }
  std::string selfPath;
  if(pos != std::string::npos)
    {
    selfPath = gxSystemTools::CollapseDirectory(av0.substr(0, pos).c_str());
    }
  else
    {
    selfPath = gxSystemTools::CollapseDirectory(".");
    }
  gxSystemTools::ConvertToUnixSlashes(selfPath);  

  // Make sure executable and self paths are represented the same way.
  std::string ePath = gxSystemTools::CollapseDirectory(GCCXML_EXECUTABLE_DIR);
  gxSystemTools::ConvertToUnixSlashes(ePath);
  std::string sPath = selfPath;
#if defined(_WIN32)
  ePath = gxSystemTools::LowerCase(ePath.c_str());
  sPath = gxSystemTools::LowerCase(sPath.c_str());
#endif
#if defined(_MSC_VER)
  if((sPath == (ePath+"/debug")) ||
     (sPath == (ePath+"/release")) ||
     (sPath == (ePath+"/relwithdebinfo")) ||
     (sPath == (ePath+"/minsizerel")))
    {
    // Strip off the build configuration subdirectory name.
    std::string::size_type pos = selfPath.rfind("/");
    selfPath = selfPath.substr(0, pos);
    sPath = sPath.substr(0, pos);
    }
#endif

  // Use our own location as the executable root.
  m_ExecutableRoot = selfPath;
  
  
  // Find the data files.
  std::string sharePath = selfPath+"/../share/GCC_XML";
   
  // If we are running from the build directory, use the source
  // directory as the data root.
  if(sPath == ePath)
    {
    m_DataRoot = GCCXML_SOURCE_DIR;
    }
  else if(gxSystemTools::FileIsDirectory(sharePath.c_str()))
    {
    // The data files are in the share path next to the bin path.
    m_DataRoot = gxSystemTools::CollapseDirectory(sharePath.c_str());
    }
  else
    {
    // Just assume that the data are next to the executable in the
    // intallation.
    m_DataRoot = m_ExecutableRoot;
    }
  gxSystemTools::ConvertToUnixSlashes(m_DataRoot);
}

//----------------------------------------------------------------------------
bool gxConfiguration::ProcessCommandLine(int argc, const char*const* argv)
{
  for(int i=1; i < argc;++i)
    {
    if(strcmp(argv[i], "--gccxml-compiler") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_COMPILER = argv[i];
        }
      else
        {
        std::cerr << "Option --gccxml-compiler requires an argument.\n";
        return false;
        }
      }
    else if(strcmp(argv[i], "--gccxml-cxxflags") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_CXXFLAGS = argv[i];
        m_HaveGCCXML_CXXFLAGS = true;
        }
      else
        {
        std::cerr << "Option --gccxml-cxxflags requires an argument.\n";
        return false;
        }
      }
    else if(strcmp(argv[i], "--gccxml-executable") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_EXECUTABLE = argv[i];
        }
      else
        {
        std::cerr << "Option --gccxml-executable requires an argument.\n";
        return false;
        }
      }
    else if(strcmp(argv[i], "--gccxml-config") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_CONFIG = argv[i];
        }
      else
        {
        std::cerr << "Option --gccxml-config requires an argument.\n";
        return false;
        }      
      }
    else if(strcmp(argv[i], "--gccxml-root") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_ROOT = argv[i];
        }
      else
        {
        std::cerr << "Option --gccxml-root requires an argument.\n";
        return false;
        }      
      }
    else if(strcmp(argv[i], "--help") == 0)
      {
      m_HelpFlag = true;
      }
    else if(strcmp(argv[i], "--version") == 0)
      {
      m_VersionFlag = true;
      }
    else if(strcmp(argv[i], "--print") == 0)
      {
      m_PrintFlag = true;
      }
    else if(strcmp(argv[i], "--debug") == 0)
      {
      m_DebugFlag = true;
      }
    else if(strcmp(argv[i], "--man") == 0)
      {
      m_ManFlag = true;
      }
    else if(strcmp(argv[i], "--copyright") == 0)
      {
      m_CopyrightFlag = true;
      }
    else if(strcmp(argv[i], "--help-html") == 0)
      {
      m_HelpHTMLFlag = true;
      }
    else
      {
#if defined(_WIN32)
      std::string arg = "\"";
      arg += argv[i];
      arg += "\"";
#else
      std::string arg = argv[i];
#endif
      m_Arguments.push_back(arg);
      }
    }
  
  return true;
}

//----------------------------------------------------------------------------
void gxConfiguration::CheckEnvironment()
{
  // Check for any settings we don't yet have in the environment.
  if(m_GCCXML_CONFIG.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_CONFIG", m_GCCXML_CONFIG);
    }
  if(m_GCCXML_COMPILER.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_COMPILER", m_GCCXML_COMPILER);
    }
  if(!m_HaveGCCXML_CXXFLAGS)
    {
    m_HaveGCCXML_CXXFLAGS =
      gxSystemTools::GetEnv("GCCXML_CXXFLAGS", m_GCCXML_CXXFLAGS);
    }
  if(m_GCCXML_EXECUTABLE.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_EXECUTABLE", m_GCCXML_EXECUTABLE);
    }
  if(m_GCCXML_ROOT.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_ROOT", m_GCCXML_ROOT);
    }
  if(m_GCCXML_FLAGS.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_FLAGS", m_GCCXML_FLAGS);
    }
  if(m_GCCXML_USER_FLAGS.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_USER_FLAGS", m_GCCXML_USER_FLAGS);
    }
}

//----------------------------------------------------------------------------
void gxConfiguration::CheckCxxEnvironment()
{
  if(m_GCCXML_COMPILER.length() == 0)
    {
    gxSystemTools::GetEnv("CXX", m_GCCXML_COMPILER);
    }
  if(!m_HaveGCCXML_CXXFLAGS)
    {
    m_HaveGCCXML_CXXFLAGS =
      gxSystemTools::GetEnv("CXXFLAGS", m_GCCXML_CXXFLAGS);
    }
}

//----------------------------------------------------------------------------
bool gxConfiguration::CheckConfigFile()
{
  // If we don't have a config file, guess its location.
  if(m_GCCXML_CONFIG.length() == 0)
    {
    if(!this->FindConfigFile())
      {
      return true;
      }
    }
  
  return this->ReadConfigFile();
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindConfigFile()
{
  std::string home;
  std::string config;
  // Check for a configuration file in the home directory.
  if(gxSystemTools::GetEnv("HOME", home) && (home.length() > 0))
    {
    char last = *(home.end()-1);
    if((last == '/') || (last == '\\'))
      {
      home = home.substr(0, home.length()-1);
      }
    config = home+"/.gccxml/config";
    if(gxSystemTools::FileExists(config.c_str()))
      {
      m_GCCXML_CONFIG = config;
      return true;
      }
    }
  // Check for a configuration file in the executable root directory.
  config = m_ExecutableRoot+"/config";
  if(gxSystemTools::FileExists(config.c_str()))
    {
    m_GCCXML_CONFIG = config;
    return true;
    }
  // Check for a configuration file in the data root directory.
  config = m_DataRoot+"/config";
  if(gxSystemTools::FileExists(config.c_str()))
    {
    m_GCCXML_CONFIG = config;
    return true;
    }
  
  return false;
}

//----------------------------------------------------------------------------
bool gxConfiguration::ReadConfigFile()
{
  if(!gxSystemTools::FileExists(m_GCCXML_CONFIG.c_str()))
    {
    std::cerr << "Configuration file \"" << m_GCCXML_CONFIG.c_str()
              << "\" does not exist.\n";
    return false;
    }
  
  std::ifstream config(m_GCCXML_CONFIG.c_str());
  if(!config)
    {
    std::cerr << "Error opening configuration file \""
              << m_GCCXML_CONFIG.c_str() << "\".\n";
    return false;
    }
  
  char buf[4096];
  // Parse config values and set them if not already set.
  while(config.getline(buf, 4096))
    {
    std::string key;
    std::string value;
    if(this->ParseConfigLine(buf, key, value))
      {
      if(key == "GCCXML_COMPILER")
        {
        if(m_GCCXML_COMPILER.length() == 0) { m_GCCXML_COMPILER = value; }
        }
      else if(key == "GCCXML_CXXFLAGS")
        {
        if(!m_HaveGCCXML_CXXFLAGS)
          {
          m_GCCXML_CXXFLAGS = value;
          m_HaveGCCXML_CXXFLAGS = true;
          }
        }
      else if(key == "GCCXML_EXECUTABLE")
        {
        if(m_GCCXML_EXECUTABLE.length() == 0) { m_GCCXML_EXECUTABLE = value; }
        }
      else if(key == "GCCXML_ROOT")
        {
        if(m_GCCXML_ROOT.length() == 0) { m_GCCXML_ROOT = value; }
        }
      else if(key == "GCCXML_FLAGS")
        {
        if(m_GCCXML_FLAGS.length() == 0) { m_GCCXML_FLAGS = value; }
        }
      else if(key == "GCCXML_USER_FLAGS")
        {
        if(m_GCCXML_USER_FLAGS.length() == 0) { m_GCCXML_USER_FLAGS = value; }
        }
      else
        {
        std::cerr << "Warning: ignoring setting for unknown key \""
                  << key.c_str() << "\"\n";
        }
      }
    }
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::ParseConfigLine(const char* in_line, std::string& key,
                                      std::string& value)
{
  std::string line = in_line;
  std::string::size_type lpos;
  std::string::size_type rpos;
  lpos = line.find_first_not_of(" \t");
  
  // Ignore comments and blank lines.
  if(lpos == std::string::npos) { return false; }
  if(line[lpos] == '#') { return false; }
  
  rpos = line.find("=", lpos);
  if(rpos == std::string::npos)
    {
    std::cerr << "Warning: ignoring invalid config file line:\n"
              << line.c_str() << "\n";
    return false;
    }
  
  // Have the key.
  key = line.substr(lpos, rpos-lpos);
  std::string rawValue = line.substr(rpos+1);
  
  // Pull off the value with no leading or trailing whitespace.
  lpos = rawValue.find_first_not_of(" \t");
  rpos = rawValue.find_last_not_of(" \t");
  if((lpos == std::string::npos) || (rpos == std::string::npos))
    {
    value = "";
    return true;
    }
  
  // If the value is double quoted, remove the end quotes.
  std::string strippedValue = rawValue.substr(lpos, rpos-lpos+1);
  if((rawValue.length() >= 2) && (*strippedValue.begin() == '"')
     && (*(strippedValue.end()-1) == '"'))
    {
    strippedValue = strippedValue.substr(1, strippedValue.length()-2);
    }
  
  value = strippedValue;
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::CheckCompiler()
{
  // Make sure a compiler has been selected.
  if(m_GCCXML_COMPILER.length() > 0) { return true; }
  std::cerr << "Could not determine compiler setting.\n";
  return false;
}

//----------------------------------------------------------------------------
bool gxConfiguration::CheckFlags()
{
  // See if there are already flags set.
  if(m_GCCXML_FLAGS.length() > 0) { return true; }
  
  // No flags, need compiler setting to guess flags.
  if(!this->CheckCompiler() || !this->FindFlags())
    {
    std::cerr << "Could not determine GCCXML_FLAGS setting.\n";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlags()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  // This is a Windows environment.  We must check the compiler name.
  std::string compilerName = m_GCCXML_COMPILER;
  compilerName = gxSystemTools::LowerCase(compilerName.c_str());
  std::string::size_type pos = compilerName.find_last_of("/");
  if(pos != std::string::npos)
    {
    compilerName = compilerName.substr(pos+1);
    }
  pos = compilerName.rfind(".exe");
  if(pos != std::string::npos)
    {
    compilerName = compilerName.substr(0, pos);
    }

  // Dispatch to find compiler.
  if(compilerName == "msvc6")
    {
    return this->FindFlagsMSVC6();
    }
  else if(compilerName == "msvc7")
    {
    return this->FindFlagsMSVC7();
    }
  else
    {
    std::cerr << "Compiler \"" << m_GCCXML_COMPILER 
              << "\" is not supported by GCC_XML.\n";
    return false;
    }
#else
  // This is a UNIX environment.  Use the gccxml_find_flags script.  
  std::string gccxmlFindFlags = m_GCCXML_ROOT+"/gccxml_find_flags";
  gccxmlFindFlags += " ";
  gccxmlFindFlags += m_GCCXML_COMPILER;
  gccxmlFindFlags += " ";
  gccxmlFindFlags += m_GCCXML_CXXFLAGS;
  int err;
  std::string flags;
  if(!gxSystemTools::RunCommand(gccxmlFindFlags.c_str(), flags, err) || err)
    {
    std::cerr << "Error executing \"" << gccxmlFindFlags << "\"\n";
    return false;
    }
  
  // Remove newlines from the flags. 
  std::string::size_type pos = flags.find_first_of("\r\n");
  while(pos != std::string::npos)
    {
    flags[pos] = ' ';
    pos = flags.find_first_of("\r\n", pos+1);
    }
  
  m_GCCXML_FLAGS = flags;
  return true;
#endif
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsMSVC6()
{
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  const char* vc6Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(vc6Registry, msvcPath))
    {
    std::cerr << "Error finding MSVC 6.0 from registry.\n";
    return false;
    }
  msvcPath += "/Include";
  gxSystemTools::ConvertToUnixSlashes(msvcPath);
  std::string vcIncludePath = m_GCCXML_ROOT+"/Vc6/Include";
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath);
  
  m_GCCXML_FLAGS =
    "-quiet -o /dev/null -nostdinc -I- -w -fsyntax-only "
    "-D__stdcall= -D__cdecl= -D_stdcall= -D_cdecl= -D__declspec(x)= "
    "-D_inline=inline -D__uuidof(x)=IID() -D__int64='long long' "
    "-D__cplusplus "
    "-D_MSC_VER=1200 -D_MSC_EXTENSIONS "
    "-D_WIN32 -D_M_IX86 -D_WCHAR_T_DEFINED "
    "-DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT -DSHSTDAPI_(x)=x "
    "-I\""+vcIncludePath+"\" -I\""+msvcPath+"\" ";
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsMSVC7()
{
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  const char* vc7Registry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.0;InstallDir";
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(vc7Registry, msvcPath))
    {
    std::cerr << "Error finding MSVC 7.0 from registry.\n";
    return false;
    }
  std::string msvcPath1 = msvcPath+"/../../Vc7/Include";
  std::string msvcPath2 = msvcPath+"/../../Vc7/PlatformSDK/Include";
  msvcPath1 = gxSystemTools::CollapseDirectory(msvcPath1.c_str());
  msvcPath2 = gxSystemTools::CollapseDirectory(msvcPath2.c_str());
  gxSystemTools::ConvertToUnixSlashes(msvcPath1);
  gxSystemTools::ConvertToUnixSlashes(msvcPath2);
  std::string vcIncludePath1 = m_GCCXML_ROOT+"/Vc7/Include";
  std::string vcIncludePath2 = m_GCCXML_ROOT+"/Vc7/PlatformSDK";
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath1);
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath2);
  
  m_GCCXML_FLAGS =
    "-quiet -o /dev/null -nostdinc -I- -w -fsyntax-only "
    "-D__stdcall= -D__cdecl= -D_stdcall= -D_cdecl= -D__cplusplus "
    "-D_inline=inline -D__forceinline=__inline "
    "-D_MSC_VER=1300 -D_MSC_EXTENSIONS -D_WIN32 -D_M_IX86 "
    "-D_WCHAR_T_DEFINED -DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT "
    "-D__declspec(x)= -D__uuidof(x)=IID() -DSHSTDAPI_(x)=x "
    "-D__w64= -D__int64='long long' "
    "-I\""+vcIncludePath1+"\" "
    "-I\""+vcIncludePath2+"\" "
    "-I\""+msvcPath1+"\" ";
    "-I\""+msvcPath2+"\" ";
  return true;
}

