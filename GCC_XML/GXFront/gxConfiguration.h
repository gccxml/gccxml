/*=========================================================================

  Program:   GCC-XML
  Module:    gxConfiguration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _gxConfiguration_h
#define _gxConfiguration_h

#include "gxSystemTools.h"

#include <vector>

/** Class to find and store the configuration settings.  */
class gxConfiguration
{
public:
  gxConfiguration();
  
  /** Setup the configuration based on the given arguments from
      the program's command line.  */
  bool Configure(int argc, const char*const* argv);
  
  /** Finish the configuration by finding the GCCXML_FLAGS setting.  */
  bool ConfigureFlags();
  
  /** Print the configuration settings.  */
  void PrintConfiguration(std::ostream& os) const;
  
  /** Get the set of arguments to pass through to the real executable.  */
  const std::vector<std::string>& GetArguments() const;
  
  /** Add the arguments to pass through to the real executable to the
      given vector.  */
  void AddArguments(std::vector<std::string>& arguments) const;
  
  /** Ask whether the --help argument was given.  */
  bool GetHelpFlag() const;
  
  /** Ask whether the --version argument was given.  */
  bool GetVersionFlag() const;
  
  /** Ask whether the --print argument was given.  */
  bool GetPrintFlag() const;
  
  /** Ask whether the --debug argument was given.  */
  bool GetDebugFlag() const;
  
  /** Ask whether the --man argument was given.  */
  bool GetManFlag() const;
  
  /** Ask whether the --copyright argument was given.  */
  bool GetCopyrightFlag() const;
  
  /** Ask whether the --help-html argument was given.  */
  bool GetHelpHTMLFlag() const;
  
  /** Get the GCCXML_EXECUTABLE setting.  */
  const std::string& GetGCCXML_EXECUTABLE() const;
  
  /** Get the GCCXML_FLAGS setting.  */
  const std::string& GetGCCXML_FLAGS() const;
  
  /** Get the GCCXML_USER_FLAGS setting.  */
  const std::string& GetGCCXML_USER_FLAGS() const;
  
protected:
  // The configuration settings.
  std::string m_GCCXML_CONFIG;
  std::string m_GCCXML_COMPILER;
  std::string m_GCCXML_CXXFLAGS;
  std::string m_GCCXML_EXECUTABLE;
  std::string m_GCCXML_FLAGS;
  std::string m_GCCXML_USER_FLAGS;
  std::string m_GCCXML_ROOT;
  bool m_HaveGCCXML_CXXFLAGS;
  
  // Program and data locations.
  std::string m_ExecutableRoot;
  std::string m_DataRoot;
  
  // The set of command line arguments to pass through to the real
  // GCC-XML executable.
  std::vector<std::string> m_Arguments;  
  
  // Whether certain arguments appeard on the command line.
  bool m_HelpFlag;
  bool m_VersionFlag;
  bool m_PrintFlag;
  bool m_DebugFlag;
  bool m_ManFlag;
  bool m_CopyrightFlag;
  bool m_HelpHTMLFlag;
  
  // Find executable and data locations of GCC-XML.
  void FindRoots(const char* argv0);
  
  // Parse settings off the command line.
  bool ProcessCommandLine(int argc, const char*const* argv);
  
  // Check the environment for any settings that we do not yet have.
  void CheckEnvironment();
  
  // Check the environment for CXX and CXXFLAGS settings.
  void CheckCxxEnvironment();
  
  // Check if there is a configuration file.  If so, read it.
  bool CheckConfigFile();
  bool FindConfigFile();
  bool ReadConfigFile();
  bool ParseConfigLine(const char* in_line, std::string& key,
                       std::string& value);
  
  // Check if we have a compiler setting.
  bool CheckCompiler();
  
  // Check if we have a flags setting.  If not, find it.
  bool CheckFlags();
  
  // Find flags based on compiler setting.
  bool FindFlags();
  bool FindFlagsMSVC6();
  bool FindFlagsMSVC7();
  bool FindFlagsMSVC71();
};

#endif
