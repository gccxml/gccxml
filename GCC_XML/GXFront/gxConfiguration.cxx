/*=========================================================================

  Program:   GCC-XML
  Module:    gxConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

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
     << "  GCCXML_EXECUTABLE=\"" << m_GCCXML_EXECUTABLE.c_str() << "\"\n"
     << "  GCCXML_FLAGS=\"" << m_GCCXML_FLAGS.c_str() << "\"\n"
     << "  GCCXML_USER_FLAGS=\"" << m_GCCXML_USER_FLAGS.c_str() << "\"\n"
     << "  GCCXML_ROOT=\"" << m_GCCXML_ROOT.c_str() << "\"\n"
     << "  ExecutableRoot=\"" << m_ExecutableRoot.c_str() << "\"\n"
     << "  DataRoot=\"" << m_DataRoot.c_str() << "\"\n";
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
  std::string::size_type pos = av0.find_last_of("/\\");
  std::string selfPath;
  if(pos != std::string::npos)
    {
    selfPath = gxSystemTools::CollapseDirectory(av0.substr(0, pos).c_str());
    }
  else
    {
    selfPath = gxSystemTools::CollapseDirectory(".");
    }
  
  // Use our own location as the executable root.
  m_ExecutableRoot = selfPath;
  
  // Find the data files.
  std::string sharePath = selfPath+"/../share/GCC_XML";
  
  // If we are running from the build directory, use the source
  // directory as the data root.
  if(selfPath == GCCXML_BINARY_DIR)
    {
    m_DataRoot = GCCXML_SOURCE_DIR;
    }
  else if(gxSystemTools::FileIsDirectory(sharePath.c_str()))
    {
    // The data files are in the share path next to the bin path.
    m_DataRoot = sharePath;
    }
  else
    {
    // Just assume that the data are next to the executable in the
    // intallation.
    m_DataRoot = m_ExecutableRoot;
    }
}

//----------------------------------------------------------------------------
bool gxConfiguration::ProcessCommandLine(int argc, const char*const* argv)
{
  for(int i=1; i < argc;++i)
    {
    if(strcmp(argv[i], "-gccxml-compiler") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_COMPILER = argv[i];
        }
      else
        {
        std::cerr << "Option -gccxml-compiler requires an argument.\n";
        return false;
        }
      }
    else if(strcmp(argv[i], "-gccxml-executable") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_EXECUTABLE = argv[i];
        }
      else
        {
        std::cerr << "Option -gccxml-executable requires an argument.\n";
        return false;
        }
      }
    else if(strcmp(argv[i], "-gccxml-config") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_CONFIG = argv[i];
        }
      else
        {
        std::cerr << "Option -gccxml-config requires an argument.\n";
        return false;
        }      
      }
    else if(strcmp(argv[i], "-gccxml-root") == 0)
      {
      if(++i < argc)
        {
        m_GCCXML_ROOT = argv[i];
        }
      else
        {
        std::cerr << "Option -gccxml-root requires an argument.\n";
        return false;
        }      
      }
    else if((strcmp(argv[i], "--help") == 0) ||
            (strcmp(argv[i], "-help") == 0))
      {
      m_HelpFlag = true;
      }
    else if((strcmp(argv[i], "--version") == 0) ||
            (strcmp(argv[i], "-version") == 0))
      {
      m_VersionFlag = true;
      }
    else if((strcmp(argv[i], "--print") == 0) ||
            (strcmp(argv[i], "-print") == 0))
      {
      m_PrintFlag = true;
      }
    else if((strcmp(argv[i], "--debug") == 0) ||
            (strcmp(argv[i], "-debug") == 0))
      {
      m_DebugFlag = true;
      }
    else
      {
      m_Arguments.push_back(argv[i]);
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
  // If no compiler has yet been configured, use the CXX environment
  // variable.
  if(m_GCCXML_COMPILER.length() > 0) { return true; }
  if(gxSystemTools::GetEnv("CXX", m_GCCXML_COMPILER))
    {
    return true;
    }
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
#if !defined(_WIN32) || defined(__CYGWIN__)
  // If in a UNIX environment, use the gccxml_find_flags script.  
  std::string gccxmlFindFlags = m_GCCXML_ROOT+"/gccxml_find_flags";
  gccxmlFindFlags += " ";
  gccxmlFindFlags += m_GCCXML_COMPILER;
  int err;
  std::string flags;
  if(!gxSystemTools::RunCommand(gccxmlFindFlags.c_str(), flags, err) || err)
    {
    std::cerr << "Error executing \"" << gccxmlFindFlags << "\"\n";
    return false;
    }
  
  std::string::size_type pos = flags.find_first_of("\r\n");
  while(pos != std::string::npos)
    {
    flags[pos] = ' ';
    pos = flags.find_first_of("\r\n", pos+1);
    }
  
  m_GCCXML_FLAGS = flags;
  return true;
#else
  return false;
#endif
}
