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

#include <gxsys/Process.h>
#include <gxsys/ios/sstream>

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
    std::cout << "GCC-XML version " GCCXML_VERSION_FULL "\n";
    return 0;
    }

  // Find the GCCXML_FLAGS setting.
  bool cfr = configuration.ConfigureFlags();

  // Print the configuration if it was requested.
  if(configuration.GetPrintFlag())
    {
    std::cout << "GCC-XML version " GCCXML_VERSION_FULL "\n";
    configuration.PrintConfiguration(std::cout);
    return cfr? 0:1;
    }

  // Check if there is anything to do.
  if(configuration.GetArguments().empty())
    {
    std::cout << "GCC-XML version " GCCXML_VERSION_FULL "\n";
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
  std::string cGCCXML_CPP = configuration.GetGCCXML_CPP();
  std::string cGCCXML_FLAGS = configuration.GetGCCXML_FLAGS();
  std::string cGCCXML_USER_FLAGS = configuration.GetGCCXML_USER_FLAGS();

  // Parse the flags.
  gxFlagsParser parser;
  parser.Parse(cGCCXML_FLAGS.c_str());
  parser.Parse(cGCCXML_USER_FLAGS.c_str());

  // Create the set of flags.
  std::vector<std::string> flags;
  configuration.AddArguments(flags);
  parser.AddParsedFlags(flags);

  // List set of flags if debugging.
  if(configuration.GetDebugFlag())
    {
    if(configuration.GetPreprocessFlag())
      {
      std::cout << "Using \"" << cGCCXML_CPP.c_str()
                << "\" as GCC-XML preprocessor.\n";
      std::cout << "Using the following arguments to GCC-XML preprocessor:\n";
      }
    else
      {
      std::cout << "Using \"" << cGCCXML_EXECUTABLE.c_str()
                << "\" as GCC-XML executable.\n";
      std::cout << "Using the following arguments to GCC-XML executable:\n";
      }
    for(std::vector<std::string>::const_iterator i = flags.begin();
        i != flags.end(); ++i)
      {
      std::cout << "  \"" << i->c_str() << "\"\n";
      }
    }

  // Select a program to run and make sure it is available.
  std::string cge;
  if(configuration.GetPreprocessFlag())
    {
    if(cGCCXML_CPP.length() == 0)
      {
      std::cerr << "Could not determine GCCXML_CPP setting.\n";
      return 1;
      }
    cge = cGCCXML_CPP;
    }
  else
    {
    if(cGCCXML_EXECUTABLE.length() == 0)
      {
      std::cerr << "Could not determine GCCXML_EXECUTABLE setting.\n";
      return 1;
      }
    cge = cGCCXML_EXECUTABLE;
    }

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(GCCXML_NATIVE_CC1PLUS)
  // Make sure a cygwin1.dll is available.
  std::string cyg = gxSystemTools::GetFilenamePath(cGCCXML_EXECUTABLE.c_str());
  cyg += "/cygwin1.dll";
  if((!gxSystemTools::FileExists(cyg.c_str())) &&
     (gxSystemTools::FindProgram("cygwin1.dll").length() == 0))
    {
    // There is no cygwin1.dll in the directory with gccxml_cc1plus or
    // in the system search path.  Look for a cygwin installation.

    // The registry key to use to find cygwin.
    const char* cygwinRegistry =
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/bin;native";
    if(gxSystemTools::ReadRegistryValue(cygwinRegistry, cyg) &&
       gxSystemTools::FileExists((cyg+"/cygwin1.dll").c_str()))
      {
      // Found the cygwin /usr/bin directory with cygwin1.dll.  Add it
      // to the end of the system search path.
      std::string path = "PATH=";
      path += gxSystemTools::GetEnv("PATH");
      if(path[path.length()-1] != ';')
        {
        path += ";";
        }
      path += cyg;
      _putenv(path.c_str());
      }
    else
      {
      std::cerr << "GCC-XML cannot find cygwin1.dll, so " << cge.c_str()
                << " will fail to run.  Aborting.\n";
      return 1;
      }
    }
#endif

  // Allow source code to be aware of GCC-XML.
  gxsys_ios::ostringstream version;
  version << "-D__GCCXML__=" << int(GCCXML_VERSION_MAJOR*10000 +
                                    GCCXML_VERSION_MINOR*100 +
                                    GCCXML_VERSION_PATCH);

  // Prepare list of arguments for exec call.
  std::vector<const char*> args;
  args.push_back(cge.c_str());
  args.push_back(version.str().c_str());
  for(unsigned int i=0; i < flags.size(); ++i)
    {
    args.push_back(flags[i].c_str());
    }
  args.push_back(0);

  // Run the patched GCC C++ parser.
  gxsysProcess* gp = gxsysProcess_New();
  gxsysProcess_SetPipeShared(gp, gxsysProcess_Pipe_STDOUT, 1);
  gxsysProcess_SetPipeShared(gp, gxsysProcess_Pipe_STDERR, 1);
  gxsysProcess_SetCommand(gp, &*args.begin());
  gxsysProcess_Execute(gp);
  gxsysProcess_WaitForExit(gp, 0);

  int result = 1;
  switch(gxsysProcess_GetState(gp))
    {
    case gxsysProcess_State_Exited:
      {
      result = gxsysProcess_GetExitValue(gp);
      } break;
    case gxsysProcess_State_Error:
      {
      std::cerr << "Error: Could not run " << cge.c_str() << ":\n";
      std::cerr << gxsysProcess_GetErrorString(gp) << "\n";
      } break;
    case gxsysProcess_State_Exception:
      {
      std::cerr << "Error: " << cge.c_str()
                << " terminated with an exception: "
                << gxsysProcess_GetExceptionString(gp) << "\n";
      } break;
    case gxsysProcess_State_Starting:
    case gxsysProcess_State_Executing:
    case gxsysProcess_State_Expired:
    case gxsysProcess_State_Killed:
      {
      // Should not get here.
      std::cerr << "Unexpected ending state after running " << cge.c_str()
                << std::endl;
      } break;
    }
  gxsysProcess_Delete(gp);

  return result;
}
