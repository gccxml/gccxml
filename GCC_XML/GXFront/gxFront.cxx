/*=========================================================================

  Program:   GCC-XML
  Module:    gxFront.cxx
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

#define GCCXML_VERSION_STRING "0.2"

/** Print the program's usage.  */
void printUsage(std::ostream& os)
{
  os <<
    "Usage:\n"
    "\n"
    "  gccxml [options] input.cxx -fxml=output.xml [-fxml-start=foo]\n"
    "\n"
    "This program is a wrapper around a real GCC-XML executable.  It will\n"
    "help the user to find the proper GCCXML_FLAGS setting and then run\n"
    "the real executable.  Configuration settings are described below.\n"
    "\n"
    "Options include:\n"
    "  --help                 = Print this usage information and exit.\n"
    "  --version              = Print the version line and exit.\n"
    "  --print                = Print the configuration settings and exit.\n"
    "  -gccxml-executable xxx = Use \"xxx\" as the real gccxml executable.\n"
    "  -gccxml-compiler xxx   = Use \"xxx\" to find GCCXML_FLAGS setting.\n"
    "  -gccxml-config xxx     = Read file \"xxx\" for configuration.\n"
    "  -gccxml-root xxx       = Use directory \"xxx\" to find supprot library.\n"
    "\n"
    "  Additional -I and -D compiler flags can also be given.\n"
    "(incomplete documentation...)\n";
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
  
  if(configuration.GetArguments().empty())
    {
    std::cout
      << "No arguments given for real GCC-XML executable.  Not running it.\n";
    return 0;
    }
  
  // Run the real GCC-XML program.
  std::string command = configuration.GetCommand();
  std::string output;
  int result;
  if(!gxSystemTools::RunCommand(command.c_str(), output, result))
    {
    std::cerr << "Error executing:\n"
              << command.c_str() << "\n";
    return 1;
    }
  
  // Display the program's output.
  std::cout << output.c_str() << std::endl;  
  
  // Use the real program's result code.
  return result;
}
