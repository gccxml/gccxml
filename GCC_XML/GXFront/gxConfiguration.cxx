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

#include <gxsys/ios/sstream>

//----------------------------------------------------------------------------
const char* gxConfigurationVc6Registry =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\"
"DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";
const char* gxConfigurationVc7Registry =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.0;InstallDir";
const char* gxConfigurationVc71Registry =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\7.1;InstallDir";
const char* gxConfigurationVc8Registry =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;InstallDir";
const char* gxConfigurationVc8RegistryVersion =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0;CLR Version";
const char* gxConfigurationVc8exRegistry =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\8.0;InstallDir";
const char* gxConfigurationVc8sdkRegistry =
"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\MicrosoftSDK\\InstalledSDKs\\8F9E5EF3-A9A5-491B-A889-C58EFFECE8B3;Install Dir";

//----------------------------------------------------------------------------
gxConfiguration::gxConfiguration()
{
  m_HelpFlag = false;
  m_VersionFlag = false;
  m_PrintFlag = false;
  m_PreprocessFlag = false;
  m_DebugFlag = false;
  m_ManFlag = false;
  m_CopyrightFlag = false;
  m_HelpHTMLFlag = false;
  m_HaveGCCXML_CXXFLAGS = false;
  m_RunningInBuildTree = false;
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

  // If no executable has been set, see if there is one in the
  // executable root.  Try intdir (Debug, Release,...) first.
#ifdef CMAKE_INTDIR
  if(m_GCCXML_EXECUTABLE.length() == 0)
    {
    std::string loc = m_ExecutableRoot+"/"+CMAKE_INTDIR+"/gccxml_cc1plus";
# ifdef _WIN32
    loc += ".exe";
# endif
    if(gxSystemTools::FileExists(loc.c_str()) &&
       !gxSystemTools::FileIsDirectory(loc.c_str()))
      {
      m_GCCXML_EXECUTABLE = loc;
      }
    }
#endif

  if(m_GCCXML_EXECUTABLE.length() == 0)
    {
    std::string loc = m_ExecutableRoot+"/gccxml_cc1plus";
#ifdef _WIN32
    loc += ".exe";
#endif
    if(gxSystemTools::FileExists(loc.c_str()) &&
       !gxSystemTools::FileIsDirectory(loc.c_str()))
      {
      m_GCCXML_EXECUTABLE = loc;
      }
    }

  // If no executable has been set, try looking in the system path.
  if(m_GCCXML_EXECUTABLE.length() == 0)
    {
    m_GCCXML_EXECUTABLE = gxSystemTools::FindProgram("gccxml_cc1plus");
    }

  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_ROOT);
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_EXECUTABLE);
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_CONFIG);
  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_COMPILER);

  // If no preprocessor has been set, see if there is one next to the
  // gccxml_cc1plus executable.
  if(m_GCCXML_CPP.length() == 0 && m_GCCXML_EXECUTABLE.length() > 0)
    {
    std::string loc =
      gxSystemTools::GetFilenamePath(m_GCCXML_EXECUTABLE.c_str());
    loc += "/gccxml_cpp0";
#ifdef _WIN32
    loc += ".exe";
#endif
    if(gxSystemTools::FileExists(loc.c_str()) &&
       !gxSystemTools::FileIsDirectory(loc.c_str()))
      {
      m_GCCXML_CPP = loc;
      }
    }

  // If no preprocessor has been set, try looking in the system path.
  if(m_GCCXML_CPP.length() == 0)
    {
    m_GCCXML_CPP = gxSystemTools::FindProgram("gccxml_cpp0");
    }

  // As a last resort, assume the preprocessor is built into the
  // gccxml_cc1plus executable.  This is the case when gccxml_cc1plus
  // is gcc 3.3 or higher.
  if(m_GCCXML_CPP.length() == 0)
    {
    m_GCCXML_CPP = m_GCCXML_EXECUTABLE;
    }

  gxSystemTools::ConvertToUnixSlashes(m_GCCXML_CPP);

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
     << "  GCCXML_CPP=\"" << m_GCCXML_CPP.c_str() << "\"\n"
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
  // Decide whether the preprocessor is built into a cc1plus
  // executable or is given separately.
  bool ppIsCC = (m_GCCXML_CPP.find("cpp0") == m_GCCXML_CPP.npos);

  // Add standard arguments.
  if(!this->GetPreprocessFlag() || ppIsCC)
    {
    // These arguments should not be used for a separate preprocessor.
    arguments.push_back("-quiet");
    arguments.push_back("-fsyntax-only");
    arguments.push_back("-w");

#if !defined(GCCXML_NATIVE_CC1PLUS)
    // If gccxml_cc1plus is provided separately, it will not have the
    // gccxml-specific -iwrapper option.  We need the -I- option
    // instead.
    arguments.push_back("-I-");
#endif
    }
  if(this->GetPreprocessFlag() && ppIsCC)
    {
    arguments.push_back("-E");
    }
  if(!this->GetPreprocessFlag())
    {
    // Thse arguments should not be used while preprocessing.
    arguments.push_back("-o");
#if defined(_WIN32) && !defined(__CYGWIN__)
    arguments.push_back("NUL");
#else
    arguments.push_back("/dev/null");
#endif
    }
  arguments.push_back("-nostdinc");

  // Allow source code to be aware of GCC-XML.
  gxsys_ios::ostringstream version;
  version << "-D__GCCXML__=" << int(GCCXML_VERSION_MAJOR*10000 +
                                    GCCXML_VERSION_MINOR*100 +
                                    GCCXML_VERSION_PATCH);
  arguments.push_back(version.str().c_str());
  arguments.push_back("-D__GCCXML_GNUC__=3");
  arguments.push_back("-D__GCCXML_GNUC_MINOR__=3");
  arguments.push_back("-D__GCCXML_GNUC_PATCHLEVEL__=2");

  // Add user arguments.
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
bool gxConfiguration::GetPreprocessFlag() const
{
  return m_PreprocessFlag;
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
const std::string& gxConfiguration::GetGCCXML_CPP() const
{
  return m_GCCXML_CPP;
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

  // Construct the name of the executable.
  std::string exeName = argv0;
  if(pos != std::string::npos)
    {
    exeName = av0.substr(pos+1).c_str();
    }
#ifdef _WIN32
  exeName = gxSystemTools::LowerCase(exeName.c_str());
  if(exeName.length() < 4 || exeName.substr(exeName.length()-4) != ".exe")
    {
    exeName += ".exe";
    }
#endif

  // Construct the full path to this executable as if it were in the
  // build tree, if it exists there.
  std::string ePath="<GCCXML_EXECUTABLE_DIR-DOES-NOT-EXIST>";
  if(gxSystemTools::FileIsDirectory(GCCXML_EXECUTABLE_DIR))
    {
    ePath = gxSystemTools::CollapseDirectory(GCCXML_EXECUTABLE_DIR);
    gxSystemTools::ConvertToUnixSlashes(ePath);
#ifdef CMAKE_INTDIR
    ePath += "/" CMAKE_INTDIR;
#endif
    ePath += "/";
    ePath += exeName;
    }

  // Construct the full path to the executable from argv[0].
  std::string sPath = selfPath;
  sPath += "/";
  sPath += exeName;

  // Find the data files.
  std::string sharePath = selfPath+"/../share/gccxml-" GCCXML_VERSION;

  // If we are running from the build directory, use the source
  // directory as the data root.
  if(gxSystemTools::SameFile(sPath.c_str(), ePath.c_str()))
    {
    // The build location of the executable and the argv[0] are the
    // same file.  We are running from the build tree.
    m_ExecutableRoot = GCCXML_EXECUTABLE_DIR;
    m_DataRoot = GCCXML_SOURCE_DIR "/Support";
    m_RunningInBuildTree = true;
    }
  else
    {
    // Use our own location as the executable root.
    m_ExecutableRoot = selfPath;

    if(gxSystemTools::FileIsDirectory(sharePath.c_str()))
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
    else if(strcmp(argv[i], "--gccxml-cpp") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_CPP = argv[i];
        }
      else
        {
        std::cerr << "Option --gccxml-cpp requires an argument.\n";
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
    else if(strcmp(argv[i], "--gccxml-gcc-options") == 0)
      {
      if(++i < argc)
        {
        if(!this->ReadArgumentFile(argv[i]))
          {
          std::cerr << "Error reading options from file \""
                    << argv[i] << "\".\n";
          return false;
          }
        }
      else
        {
        std::cerr << "Option --gccxml-gcc-options requires an argument.\n";
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
    else if((strcmp(argv[i], "--preprocess") == 0) ||
            (strcmp(argv[i], "-E") == 0))
      {
      m_PreprocessFlag = true;
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
      m_Arguments.push_back(argv[i]);
      }
    }

  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::ReadArgumentFile(const char* fname)
{
  std::ifstream fin(fname, std::ios::in);
  if(!fin)
    {
    return false;
    }

  std::string option;
  while(std::getline(fin, option))
    {
    // Remove leading and trailing whitespace.
    std::string::size_type first = option.find_first_not_of(" \t");
    std::string::size_type last = option.find_last_not_of(" \t");
    if(first != std::string::npos && last != std::string::npos)
      {
      // There is at least one non-whitespace character.
      option = option.substr(first, last-first+1);

      // Look for comments.
      if(option[0] != '#')
        {
        m_Arguments.push_back(option);
        }
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
  if(m_GCCXML_CPP.length() == 0)
    {
    gxSystemTools::GetEnv("GCCXML_CPP", m_GCCXML_CPP);
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
  config = m_ExecutableRoot+"/gccxml_config";
  if(gxSystemTools::FileExists(config.c_str()))
    {
    m_GCCXML_CONFIG = config;
    return true;
    }
  config = m_ExecutableRoot+"/config";
  if(gxSystemTools::FileExists(config.c_str()))
    {
    m_GCCXML_CONFIG = config;
    return true;
    }
  // Check for a configuration file in the data root directory.
  config = m_DataRoot+"/gccxml_config";
  if(gxSystemTools::FileExists(config.c_str()))
    {
    m_GCCXML_CONFIG = config;
    return true;
    }
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
      else if(key == "GCCXML_CPP")
        {
        if(m_GCCXML_CPP.length() == 0) { m_GCCXML_CPP = value; }
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
  lpos = line.find_first_not_of(" \t\r");

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
  lpos = rawValue.find_first_not_of(" \t\r");
  rpos = rawValue.find_last_not_of(" \t\r");
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
  else if(compilerName == "msvc71")
    {
    return this->FindFlagsMSVC71();
    }
  else if(compilerName == "msvc8")
    {
    std::string loc;
    bool have8ex =
      gxSystemTools::ReadRegistryValue(gxConfigurationVc8exRegistry, loc);
    if(have8ex)
      {
      return this->FindFlagsMSVC8ex();
      }
    else
      {
      return this->FindFlagsMSVC8();
      }
    }
  else if(compilerName == "cl")
    {
    // We must decide if this is MSVC 6, 7, 7.1, or 8.
    std::string loc;
    bool have6 = gxSystemTools::ReadRegistryValue(gxConfigurationVc6Registry,
                                                  loc);
    bool have7 = gxSystemTools::ReadRegistryValue(gxConfigurationVc7Registry,
                                                  loc);
    bool have71 = gxSystemTools::ReadRegistryValue(gxConfigurationVc71Registry,
                                                   loc);
    bool have8ex =
      gxSystemTools::ReadRegistryValue(gxConfigurationVc8exRegistry, loc);
    bool have8sdk =
      gxSystemTools::ReadRegistryValue(gxConfigurationVc8sdkRegistry, loc);

    // Look for a VS8 that is not the beta release.
    bool have8 = false;
    if(gxSystemTools::ReadRegistryValue(gxConfigurationVc8Registry, loc))
      {
      // The "CLR Version" registry entry in VS 8 has value "v2.0.40607"
      // for the beta and "v2.0.50727" for the release.
      std::string version;
      if(gxSystemTools::ReadRegistryValue(gxConfigurationVc8RegistryVersion,
                                          version))
        {
        int vnum;
        if((sscanf(version.c_str(), "v2.0.%d", &vnum) == 1) &&
           vnum >= 50727)
          {
          have8 = true;
          }
        }
      }

    // Find available support directories.
    bool support6 =
      gxSystemTools::FileIsDirectory((m_GCCXML_ROOT+"/Vc6").c_str());
    bool support7 =
      gxSystemTools::FileIsDirectory((m_GCCXML_ROOT+"/Vc7").c_str());
    bool support71 =
      gxSystemTools::FileIsDirectory((m_GCCXML_ROOT+"/Vc71").c_str());
    bool support8 =
      gxSystemTools::FileIsDirectory((m_GCCXML_ROOT+"/Vc8").c_str());
    bool support8ex =
      gxSystemTools::FileIsDirectory((m_GCCXML_ROOT+"/Vc8ex").c_str());

    // See if only one is installed.
    if(have6 && !have7 && !have71 && !have8 && !have8ex)
      {
      return this->FindFlagsMSVC6();
      }
    else if(!have6 && have7 && !have71 && !have8 && !have8ex)
      {
      return this->FindFlagsMSVC7();
      }
    else if(!have6 && !have7 && have71 && !have8 && !have8ex)
      {
      return this->FindFlagsMSVC71();
      }
    else if(!have6 && !have7 && !have71 && have8 && !have8ex)
      {
      return this->FindFlagsMSVC8();
      }
    else if(!have6 && !have7 && !have71 && !have8 && have8ex)
      {
      return this->FindFlagsMSVC8ex();
      }
    else if(have6 || have7 || have71 || have8 || have8ex)
      {
      // Have more than one.  See if only one has the support
      // directory available.
      if(support6 && !support7 && !support71 && !support8 && !support8ex)
        {
        return this->FindFlagsMSVC6();
        }
      else if(!support6 && support7 && !support71 && !support8 && !support8ex)
        {
        return this->FindFlagsMSVC7();
        }
      else if(!support6 && !support7 && support71 && !support8 && !support8ex)
        {
        return this->FindFlagsMSVC71();
        }
      else if(!support6 && !support7 && !support71 && support8 && !support8ex)
        {
        return this->FindFlagsMSVC8();
        }
      else if(!support6 && !support7 && !support71 && !support8 && support8ex)
        {
        return this->FindFlagsMSVC8ex();
        }
      else if(!support6 && !support7 && !support71 && !support8 && !support8ex)
        {
        std::cerr << "Compiler \"" << m_GCCXML_COMPILER
                  << "\" is not supported by GCC_XML because none of \n"
                  << "the Vc6, Vc7, Vc71, Vc8, or Vc8ex "
                  << "support directories exists.\n";
        return false;
        }

      // Can support either.  See if one is found in the path.
      std::string cl = gxSystemTools::FindProgram("cl");
      if(cl.length() > 0)
        {
        // Found cl in the path.  Look at full location.
        gxSystemTools::ConvertToUnixSlashes(cl);
        if((cl.find("/VC98/") != std::string::npos) ||
           (cl.find("/vc98/") != std::string::npos) ||
           (cl.find("/Vc98/") != std::string::npos))
          {
          return this->FindFlagsMSVC6();
          }
        else if((cl.find("/VC7/") != std::string::npos) ||
                (cl.find("/vc7/") != std::string::npos) ||
                (cl.find("/Vc7/") != std::string::npos))
          {
          // This is a cl from 7 or 7.1.  Use one if we have it.
          if(have7 && !have71)
            {
            return this->FindFlagsMSVC7();
            }
          else if(!have7 && have71)
            {
            return this->FindFlagsMSVC71();
            }
          }

        // Couldn't tell from program location.  Try running it.
        std::string output;
        int retVal=0;
        if(gxSystemTools::RunCommand("cl", output, retVal) && (retVal == 0))
          {
          // The compiler ran.  Get version from output.
          if(output.find("Compiler Version 12.") != std::string::npos)
            {
            return this->FindFlagsMSVC6();
            }
          else if(output.find("Compiler Version 13.0") != std::string::npos)
            {
            return this->FindFlagsMSVC7();
            }
          else if(output.find("Compiler Version 13.1") != std::string::npos)
            {
            return this->FindFlagsMSVC71();
            }
          else if(output.find("Compiler Version 14.") != std::string::npos)
            {
            if(have8)
              {
              return this->FindFlagsMSVC8();
              }
            else if(have8ex)
              {
              return this->FindFlagsMSVC8ex();
              }
            }
          }
        // Couldn't tell by running the compiler.
        }
      // Couldn't tell by finding the compiler.  Guess based on which
      // was used to build this executable.
      const char* const clText =
        "Compiler \"cl\" specified, but more than one of "
        "MSVC 6, 7, 7.1, and 8 are installed.\n"
        "Please specify \"msvc6\", \"msvc7\", \"msvc71\", or \"msvc8\" for "
        "the GCCXML_COMPILER setting.\n";
#if defined(_MSC_VER) && ((_MSC_VER >= 1200) && (_MSC_VER < 1300))
      std::cerr << "Warning:\n" << clText
                << "Using MSVC 6 because it was used to build GCC-XML.\n"
                << "\n";
      return this->FindFlagsMSVC6();
#elif defined(_MSC_VER) && ((_MSC_VER >= 1300) && (_MSC_VER < 1310))
      std::cerr << "Warning:\n" << clText
                << "Using MSVC 7 because it was used to build GCC-XML.\n"
                << "\n";
      return this->FindFlagsMSVC7();
#elif defined(_MSC_VER) && ((_MSC_VER >= 1310) && (_MSC_VER < 1400))
      std::cerr << "Warning:\n" << clText
                << "Using MSVC 7.1 because it was used to build GCC-XML.\n"
                << "\n";
      return this->FindFlagsMSVC71();
#elif defined(_MSC_VER) && ((_MSC_VER >= 1400) && (_MSC_VER < 1500))
      std::cerr << "Warning:\n" << clText
                << "Using MSVC 8 because it was used to build GCC-XML.\n"
                << "\n";
      if(have8)
        {
        return this->FindFlagsMSVC8();
        }
      else if(have8ex)
        {
        return this->FindFlagsMSVC8ex();
        }
      else
        {
        std::cerr << "No installed version of MSVC 8 was found!\n";
        }
#else
      // Give up.  The user must specify one.
      std::cerr << clText;
      return false;
#endif
      }
    else
      {
      std::cerr << "Compiler \"" << m_GCCXML_COMPILER
                << "\" is not supported by GCC_XML because "
                << "none of MSVC 6, 7, 7.1, 8 is installed.\n";
      return false;
      }
    }
  else if(compilerName == "bcc32")
    {
    // Compiler must be in the path.  Find it.
    std::string bcc32 = gxSystemTools::FindProgram("bcc32");
    if(bcc32.length() > 0)
      {
      // Check if the compiler location gives its version.
      if((bcc32.find("BCC55/") != bcc32.npos) ||
         (bcc32.find("Bcc55/") != bcc32.npos) ||
         (bcc32.find("bcc55/") != bcc32.npos))
        {
        return this->FindFlagsBCC55(bcc32.c_str());
        }

      // Couldn't tell from program location.  Try running it.
      std::string output;
      int retVal=0;
      if(gxSystemTools::RunCommand("bcc32", output, retVal) && (retVal == 0))
        {
        // The compiler ran.  Get version from output.
        if(output.find("Borland C++ 5.5") != std::string::npos)
          {
          return this->FindFlagsBCC55(bcc32.c_str());
          }
        }
      }
    }

  // Didn't find supported compiler.
  std::cerr << "Compiler \"" << m_GCCXML_COMPILER
            << "\" is not supported by GCC_XML.\n";
  return false;
#else
  // This is a UNIX environment.  Use the gccxml_find_flags script.
  std::string gccxmlFindFlags;
  if(m_GCCXML_ROOT.find(" ") != m_GCCXML_ROOT.npos)
    {
    gccxmlFindFlags = "\"";
    gccxmlFindFlags += m_GCCXML_ROOT;
    gccxmlFindFlags += "/gccxml_find_flags\"";
    }
  else
    {
    gccxmlFindFlags = m_GCCXML_ROOT;
    gccxmlFindFlags += "/gccxml_find_flags";
    }
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
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(gxConfigurationVc6Registry, msvcPath))
    {
    std::cerr << "Error finding MSVC 6.0 from registry.\n";
    return false;
    }
  msvcPath += "/Include";
  gxSystemTools::ConvertToUnixSlashes(msvcPath);
  std::string vcIncludePath = m_GCCXML_ROOT+"/Vc6/Include";
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath);

  // Make sure the support directory exists.
  if(!gxSystemTools::FileIsDirectory(vcIncludePath.c_str()))
    {
    std::cerr << "Vc6/Include support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath.c_str() << "\".\n";
    return false;
    }

  m_GCCXML_FLAGS =
    "-U__STDC__ -U__MINGW32__"
    "-D__stdcall=__attribute__((__stdcall__)) "
    "-D__cdecl=__attribute__((__cdecl__)) "
    "-D__fastcall=__attribute__((__fastcall__)) "
    "-D_stdcall=__attribute__((__stdcall__)) "
    "-D_cdecl=__attribute__((__cdecl__)) "
    "-D_fastcall=__attribute__((__fastcall__)) "
    "-D__declspec(x)=__attribute__((x)) "
    "-D_inline=inline -D__uuidof(x)=IID() -D__int64=\"long long\" "
    "-D__cplusplus -D_MSC_VER=1200 -D_MSC_EXTENSIONS "
    "-D_WIN32 -D_M_IX86 -D_WCHAR_T_DEFINED -D_INTEGRAL_MAX_BITS=64 "
    "-DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT -DSHSTDAPI_(x)=x "
    "-iwrapper\""+vcIncludePath+"\" -I\""+msvcPath+"\" ";
 return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsMSVC7()
{
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(gxConfigurationVc7Registry, msvcPath))
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

  // Make sure the support directories exist.
  if(!gxSystemTools::FileIsDirectory(vcIncludePath1.c_str()))
    {
    std::cerr << "Vc7/Include support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath1.c_str() << "\".\n";
    return false;
    }
  if(!gxSystemTools::FileIsDirectory(vcIncludePath2.c_str()))
    {
    std::cerr << "Vc7/PlatformSDK support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath2.c_str() << "\".\n";
    return false;
    }

  m_GCCXML_FLAGS =
    "-U__STDC__ -U__MINGW32__"
    "-D__stdcall=__attribute__((__stdcall__)) "
    "-D__cdecl=__attribute__((__cdecl__)) "
    "-D__fastcall=__attribute__((__fastcall__)) "
    "-D_stdcall=__attribute__((__stdcall__)) "
    "-D_cdecl=__attribute__((__cdecl__)) "
    "-D_fastcall=__attribute__((__fastcall__)) "
    "-D__declspec(x)=__attribute__((x)) "
    "-D__cplusplus -D_inline=inline -D__forceinline=__inline "
    "-D_MSC_VER=1300 -D_MSC_EXTENSIONS -D_WIN32 -D_M_IX86 "
    "-D_WCHAR_T_DEFINED -DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT "
    "-D_INTEGRAL_MAX_BITS=64 "
    "-D__uuidof(x)=IID() -DSHSTDAPI_(x)=x -D__w64= -D__int64=\"long long\" "
    "-iwrapper\""+vcIncludePath1+"\" "
    "-iwrapper\""+vcIncludePath2+"\" "
    "-I\""+msvcPath1+"\" "
    "-I\""+msvcPath2+"\" ";
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsMSVC71()
{
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(gxConfigurationVc71Registry, msvcPath))
    {
    std::cerr << "Error finding MSVC 7.1 from registry.\n";
    return false;
    }
  std::string msvcPath1 = msvcPath+"/../../Vc7/Include";
  std::string msvcPath2 = msvcPath+"/../../Vc7/PlatformSDK/Include";
  msvcPath1 = gxSystemTools::CollapseDirectory(msvcPath1.c_str());
  msvcPath2 = gxSystemTools::CollapseDirectory(msvcPath2.c_str());
  gxSystemTools::ConvertToUnixSlashes(msvcPath1);
  gxSystemTools::ConvertToUnixSlashes(msvcPath2);
  std::string vcIncludePath1 = m_GCCXML_ROOT+"/Vc71/Include";
  std::string vcIncludePath2 = m_GCCXML_ROOT+"/Vc71/PlatformSDK";
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath1);
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath2);

  // Make sure the support directories exist.
  if(!gxSystemTools::FileIsDirectory(vcIncludePath1.c_str()))
    {
    std::cerr << "Vc71/Include support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath1.c_str() << "\".\n";
    return false;
    }
  if(!gxSystemTools::FileIsDirectory(vcIncludePath2.c_str()))
    {
    std::cerr << "Vc71/PlatformSDK support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath2.c_str() << "\".\n";
    return false;
    }

  m_GCCXML_FLAGS =
    "-U__STDC__ -U__MINGW32__"
    "-D__stdcall=__attribute__((__stdcall__)) "
    "-D__cdecl=__attribute__((__cdecl__)) "
    "-D__fastcall=__attribute__((__fastcall__)) "
    "-D_stdcall=__attribute__((__stdcall__)) "
    "-D_cdecl=__attribute__((__cdecl__)) "
    "-D_fastcall=__attribute__((__fastcall__)) "
    "-D__declspec(x)=__attribute__((x)) "
    "-D__cplusplus -D_inline=inline -D__forceinline=__inline "
    "-D_MSC_VER=1310 -D_MSC_EXTENSIONS -D_WIN32 -D_M_IX86 "
    "-D_WCHAR_T_DEFINED -DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT "
    "-D_INTEGRAL_MAX_BITS=64 "
    "-D__uuidof(x)=IID() -DSHSTDAPI_(x)=x -D__w64= -D__int64=\"long long\" "
    "-iwrapper\""+vcIncludePath1+"\" "
    "-iwrapper\""+vcIncludePath2+"\" "
    "-I\""+msvcPath1+"\" "
    "-I\""+msvcPath2+"\" ";
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsMSVC8()
{
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(gxConfigurationVc8Registry, msvcPath))
    {
    std::cerr << "Error finding MSVC 8 from registry.\n";
    return false;
    }
  std::string msvcPath1 = msvcPath+"/../../Vc/Include";
  std::string msvcPath2 = msvcPath+"/../../Vc/PlatformSDK/Include";
  msvcPath1 = gxSystemTools::CollapseDirectory(msvcPath1.c_str());
  msvcPath2 = gxSystemTools::CollapseDirectory(msvcPath2.c_str());
  gxSystemTools::ConvertToUnixSlashes(msvcPath1);
  gxSystemTools::ConvertToUnixSlashes(msvcPath2);
  std::string vcIncludePath1 = m_GCCXML_ROOT+"/Vc8/Include";
  std::string vcIncludePath2 = m_GCCXML_ROOT+"/Vc8/PlatformSDK";
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath1);
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath2);

  // Make sure the support directories exist.
  if(!gxSystemTools::FileIsDirectory(vcIncludePath1.c_str()))
    {
    std::cerr << "Vc8/Include support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath1.c_str() << "\".\n";
    return false;
    }
  if(!gxSystemTools::FileIsDirectory(vcIncludePath2.c_str()))
    {
    std::cerr << "Vc8/PlatformSDK support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath2.c_str() << "\".\n";
    return false;
    }

  m_GCCXML_FLAGS =
    "-U__STDC__ -U__MINGW32__"
    "-D__stdcall=__attribute__((__stdcall__)) "
    "-D__cdecl=__attribute__((__cdecl__)) "
    "-D__fastcall=__attribute__((__fastcall__)) "
    "-D__thiscall=__attribute__((__thiscall__)) "
    "-D_stdcall=__attribute__((__stdcall__)) "
    "-D_cdecl=__attribute__((__cdecl__)) "
    "-D_fastcall=__attribute__((__fastcall__)) "
    "-D_thiscall=__attribute__((__thiscall__)) "
    "-D__declspec(x)=__attribute__((x)) -D__pragma(x)= "
    "-D__cplusplus -D_inline=inline -D__forceinline=__inline "
    "-D_MSC_VER=1400 -D_MSC_EXTENSIONS -D_WIN32 "
    "-D_M_IX86 "
    "-D_WCHAR_T_DEFINED -DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT "
    "-D_INTEGRAL_MAX_BITS=64 "
    "-D__uuidof(x)=IID() -DSHSTDAPI_(x)=x "
    "-D__w64= "
    "-D__int8=char "
    "-D__int16=short "
    "-D__int32=int "
    "-D__int64=\"long long\" "
    "-D__ptr64= "
    "-DSTRSAFE_NO_DEPRECATE "
    "-D_CRT_FAR_MAPPINGS_NO_DEPRECATE "
    "-D_CRT_MANAGED_FP_NO_DEPRECATE "
    "-D_CRT_MANAGED_HEAP_NO_DEPRECATE "
    "-D_CRT_NONSTDC_NO_DEPRECATE "
    "-D_CRT_OBSOLETE_NO_DEPRECATE "
    "-D_CRT_SECURE_NO_DEPRECATE "
    "-D_CRT_SECURE_NO_DEPRECATE_GLOBALS "
    "-D_CRT_VCCLRIT_NO_DEPRECATE "
    "-D_SCL_SECURE_NO_DEPRECATE "
    "-D_WINDOWS_SECURE_NO_DEPRECATE "
    "-iwrapper\""+vcIncludePath1+"\" "
    "-iwrapper\""+vcIncludePath2+"\" "
    "-I\""+msvcPath1+"\" "
    "-I\""+msvcPath2+"\" ";
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsMSVC8ex()
{
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  std::string msvcPath;
  if(!gxSystemTools::ReadRegistryValue(gxConfigurationVc8exRegistry, msvcPath))
    {
    std::cerr << "Error finding MSVC 8 Express from registry.\n";
    return false;
    }
  std::string msvcPath1 = msvcPath+"/../../Vc/Include";
  msvcPath1 = gxSystemTools::CollapseDirectory(msvcPath1.c_str());
  gxSystemTools::ConvertToUnixSlashes(msvcPath1);
  std::string vcIncludePath1 = m_GCCXML_ROOT+"/Vc8ex/Include";
  gxSystemTools::ConvertToUnixSlashes(vcIncludePath1);

  std::string psdkPath;
  bool havePSDK =
    gxSystemTools::ReadRegistryValue(gxConfigurationVc8sdkRegistry, psdkPath);
  std::string msvcPath2;
  std::string vcIncludePath2;
  if(havePSDK)
    {
    msvcPath2 = psdkPath+"/Include";
    msvcPath2 = gxSystemTools::CollapseDirectory(msvcPath2.c_str());
    gxSystemTools::ConvertToUnixSlashes(msvcPath2);
    vcIncludePath2 = m_GCCXML_ROOT+"/Vc8ex/PlatformSDK";
    gxSystemTools::ConvertToUnixSlashes(vcIncludePath2);
    }

  // Make sure the support directories exist.
  if(!gxSystemTools::FileIsDirectory(vcIncludePath1.c_str()))
    {
    std::cerr << "Vc8ex/Include support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath1.c_str() << "\".\n";
    return false;
    }
  if(havePSDK && !gxSystemTools::FileIsDirectory(vcIncludePath2.c_str()))
    {
    std::cerr << "Vc8ex/PlatformSDK support directory is not available.\n";
    std::cerr << "Checked \"" << vcIncludePath2.c_str() << "\".\n";
    return false;
    }

  m_GCCXML_FLAGS =
    "-U__STDC__ -U__MINGW32__"
    "-D__stdcall=__attribute__((__stdcall__)) "
    "-D__cdecl=__attribute__((__cdecl__)) "
    "-D__fastcall=__attribute__((__fastcall__)) "
    "-D__thiscall=__attribute__((__thiscall__)) "
    "-D_stdcall=__attribute__((__stdcall__)) "
    "-D_cdecl=__attribute__((__cdecl__)) "
    "-D_fastcall=__attribute__((__fastcall__)) "
    "-D_thiscall=__attribute__((__thiscall__)) "
    "-D__declspec(x)=__attribute__((x)) -D__pragma(x)= "
    "-D__cplusplus -D_inline=inline -D__forceinline=__inline "
    "-D_MSC_VER=1400 -D_MSC_EXTENSIONS -D_WIN32 "
    "-D_M_IX86 "
    "-D_WCHAR_T_DEFINED -DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT "
    "-D_INTEGRAL_MAX_BITS=64 "
    "-D__uuidof(x)=IID() -DSHSTDAPI_(x)=x "
    "-D__w64= "
    "-D__int8=char "
    "-D__int16=short "
    "-D__int32=int "
    "-D__int64=\"long long\" "
    "-D__ptr64= "
    "-DSTRSAFE_NO_DEPRECATE "
    "-D_CRT_FAR_MAPPINGS_NO_DEPRECATE "
    "-D_CRT_MANAGED_FP_NO_DEPRECATE "
    "-D_CRT_MANAGED_HEAP_NO_DEPRECATE "
    "-D_CRT_NONSTDC_NO_DEPRECATE "
    "-D_CRT_OBSOLETE_NO_DEPRECATE "
    "-D_CRT_SECURE_NO_DEPRECATE "
    "-D_CRT_SECURE_NO_DEPRECATE_GLOBALS "
    "-D_CRT_VCCLRIT_NO_DEPRECATE "
    "-D_SCL_SECURE_NO_DEPRECATE "
    "-D_WINDOWS_SECURE_NO_DEPRECATE ";
  m_GCCXML_FLAGS += "-iwrapper\""+vcIncludePath1+"\" ";
  if(havePSDK)
    {
    m_GCCXML_FLAGS += "-iwrapper\""+vcIncludePath2+"\" ";
    }
  m_GCCXML_FLAGS += "-I\""+msvcPath1+"\" ";
  if(havePSDK)
    {
    m_GCCXML_FLAGS += "-I\""+msvcPath2+"\" ";
    }
  return true;
}

//----------------------------------------------------------------------------
bool gxConfiguration::FindFlagsBCC55(const char* inBcc32)
{
  // Find the support include directory.
  std::string include2 = m_GCCXML_ROOT+"/Borland/5.5";
  if(!gxSystemTools::FileIsDirectory(include2.c_str()))
    {
    // If we are running in the build tree, we can use the support
    // directory from the source tree.
    if(m_RunningInBuildTree)
      {
      include2 = GCCXML_SOURCE_DIR "/Support/Borland/5.5";
      }
    else
      {
      std::cerr << "Borland/5.5 support directory is not available.\n";
      std::cerr << "Checked \"" << include2.c_str() << "\".\n";
      return false;
      }
    }
  std::string include1 = include2;
  include1 += "/Wrappers";

  // Find the compiler's include directory.
  std::string include3 = inBcc32;
  include3 = include3.substr(0, include3.rfind('/'));
  include3 = include3.substr(0, include3.rfind('/'));
  include3 += "/Include";

  if(!gxSystemTools::FileIsDirectory(include3.c_str()))
    {
    std::cerr << "Borland C++ 5.5 include directory cannot be found.\n";
    std::cerr << "Checked \"" << include3.c_str()
              << "\" because it is the standard location relative to the "
              << "executable, which is located at \""
              << inBcc32 << "\".\n";
    return false;
    }

  m_GCCXML_FLAGS =
    "-D__stdcall=__attribute__((__stdcall__)) "
    "-D__cdecl=__attribute__((__cdecl__)) "
    "-D__fastcall=__attribute__((__fastcall__)) "
    "-D_stdcall=__attribute__((__stdcall__)) "
    "-D_cdecl=__attribute__((__cdecl__)) "
    "-D_fastcall=__attribute__((__fastcall__)) "
    "-D__declspec(x)=__attribute__((x)) "
    "-D__cplusplus=1 -D_inline=inline -D__forceinline=__inline "
    "-D__rtti= -D_WIN32=1 -D__WIN32__=1 -D_M_IX86 "
    "-D_WCHAR_T_DEFINED -DPASCAL= -DRPC_ENTRY= -DSHSTDAPI=HRESULT "
    "-D__uuidof(x)=IID() -DSHSTDAPI_(x)=x -D__w64= -D__int64=\"long long\" "
    "-D__TURBOC__=0x0551 -D__BORLANDC__=0x0551 "
    "-U__STDC__ -U__PTRDIFF_TYPE__ -U__SIZE_TYPE__ "
    "-iwrapper\""+include1+"\" "
    "-iwrapper\""+include2+"\" "
    "-I\""+include3+"\" ";
  return true;

}
