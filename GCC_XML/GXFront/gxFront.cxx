/*=========================================================================

  Program:   GCC-XML
  Module:    gxFront.cxx
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
#include "gxFlagsParser.h"

#include <string.h>
#include <errno.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <process.h>
inline int GXSpawn(const char* cmd, char** argv)
{
  return _spawnvp(_P_WAIT, cmd, argv);
}
#else
#include <unistd.h>
inline int GXSpawn(const char* cmd, char** argv)
{
  return execvp(cmd, argv);
}
#endif

#define GCCXML_VERSION_STRING "0.3 - development"

/** Print the program's usage.  */
void printUsage(std::ostream& os)
{
  os <<
    "Usage:\n"
    "\n"
    "  gccxml [options] input.cxx -fxml=output.xml [-fxml-start=foo[,...]]\n"
    "\n"
    "This program is a wrapper around a real GCC-XML executable.  It will\n"
    "help the user to find the proper GCCXML_FLAGS setting and then run\n"
    "the real executable.  Configuration settings are described below.\n"
    "\n"
    "Options include:\n"
    "  --help                  = Print this usage information and exit.\n"
    "  --version               = Print the version line and exit.\n"
    "  --print                 = Print the configuration settings and exit.\n"
    "  --gccxml-executable xxx = Use \"xxx\" as the real gccxml executable.\n"
    "  --gccxml-compiler xxx   = Use \"xxx\" compiler to find GCCXML_FLAGS.\n"
    "  --gccxml-cxxflags xxx   = Use \"xxx\" C++ flags to find GCCXML_FLAGS.\n"
    "  --gccxml-config xxx     = Read file \"xxx\" for configuration.\n"
    "  --gccxml-root xxx       = Use directory \"xxx\" to find supprot library.\n"
    "\n"
    "  Additional -I and -D compiler flags can also be given.\n";
}

int main(int argc, char** argv)
{
  gxConfiguration configuration;
  
  // Do initial configuration.
  if(!configuration.Configure(argc, argv))
    {
    std::cerr << "Error during configuration.  Aborting.\n";
    return 1;
    }
  
  // Check for any simple flags.
  if(configuration.GetHelpFlag())
    {
    printUsage(std::cout);
    return 0;
    }
  
  if(configuration.GetVersionFlag())
    {
    std::cout << "GCC-XML version " GCCXML_VERSION_STRING "\n";
    return 0;
    }
  
  // Find the GCCXML_FLAGS setting.
  bool cfr = configuration.ConfigureFlags();
  
  // Print the configuration if it was requested.
  if(configuration.GetPrintFlag())
    {
    std::cout << "GCC-XML version " GCCXML_VERSION_STRING "\n";
    configuration.PrintConfiguration(std::cout);
    return cfr? 0:1;
    }
  if(!cfr) { return 1; }
  
  // Check if there is anything to do.
  if(configuration.GetArguments().empty())
    {
    std::cout
      << "No arguments given for real GCC-XML executable.  Not running it.\n";
    printUsage(std::cout);
    return 0;
    }
  
  // Get the configuration settings.
  std::string cGCCXML_EXECUTABLE = configuration.GetGCCXML_EXECUTABLE();
  std::string cGCCXML_FLAGS = configuration.GetGCCXML_FLAGS();
  std::string cGCCXML_USER_FLAGS = configuration.GetGCCXML_USER_FLAGS();
  
  // Parse the flags.
  gxFlagsParser parser;
  parser.Parse(cGCCXML_FLAGS.c_str());
  parser.Parse(cGCCXML_USER_FLAGS.c_str());
  
  // Create the set of flags.
  std::vector<std::string> flags;
  parser.AddParsedFlags(flags);
  configuration.AddArguments(flags);
  
  // List set of flags if debugging.
  if(configuration.GetDebugFlag())
    {
    std::cout << "Using \"" << cGCCXML_EXECUTABLE.c_str()
              << "\" as GCC-XML executable.\n";
    std::cout << "Using the following arguments to GCC-XML executable:\n";
    for(std::vector<std::string>::const_iterator i = flags.begin();
        i != flags.end(); ++i)
      {
      std::cout << "  \"" << i->c_str() << "\"\n";
      }
    }
  
  // Convert the program path to a platform-dependent format.
  std::string cge =
    gxSystemTools::ConvertToOutputPath(cGCCXML_EXECUTABLE.c_str());
  
  // Prepare list of arguments for exec call.
  char** args = new char*[flags.size()+2];
  args[0] = strdup(cge.c_str());
  for(unsigned int i=0; i < flags.size(); ++i)
    {
    args[i+1] = strdup(flags[i].c_str());
    }
  args[flags.size()+1] = 0;
  
  if(GXSpawn(cGCCXML_EXECUTABLE.c_str(), args) < 0)
    {
    std::cerr << "Error executing " << cGCCXML_EXECUTABLE.c_str() << "\n";
    exit(errno);
    }
  
  return 0;
}
