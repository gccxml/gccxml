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
#include "gxDocumentation.h"

#include <string.h>
#include <errno.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <process.h>
inline int GXSpawn(const char* cmd, char** argv)
{
  return static_cast<int>(_spawnvp(_P_WAIT, cmd, argv));
}
#else
#include <unistd.h>
inline int GXSpawn(const char* cmd, char** argv)
{
  return execvp(cmd, argv);
}
#endif

#define GCCXML_VERSION_STRING "0.3 - development"

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
  if(configuration.GetManFlag())
    {
    gxDocumentation::PrintManPage(std::cout);
    return 0;
    }
  
  if(configuration.GetCopyrightFlag())
    {
    gxDocumentation::PrintCopyright(std::cout);
    return 0;
    }
  
  if(configuration.GetHelpHTMLFlag())
    {
    gxDocumentation::PrintHelpHTML(std::cout);
    return 0;
    }
  
  if(configuration.GetHelpFlag())
    {
    gxDocumentation::PrintHelp(std::cout);
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
  
  // Check if there is anything to do.
  if(configuration.GetArguments().empty())
    {
    std::cout << "GCC-XML version " GCCXML_VERSION_STRING "\n";
    std::cout
      << "No arguments given for GCC C++ parser.  Not running it.\n";
    gxDocumentation::PrintUsage(std::cout);
    return 0;
    }
  
  // We have something to do.  Make sure the GCCXML_FLAGS setting is
  // valid.
  if(!cfr)
    {
    return 1;
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
