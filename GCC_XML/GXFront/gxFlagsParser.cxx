/*=========================================================================

  Program:   GCC-XML
  Module:    gxFlagsParser.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gxFlagsParser.h"

//----------------------------------------------------------------------------
void gxFlagsParser::Parse(const char* in_flags)
{
  // Prepare a work string for searching.
  std::string flags = in_flags;
  
  // Look for " -" separating arguments.
  
  // The first argument starts at the first "-" character.
  std::string::size_type leftPos = flags.find_first_of("-");
  if(leftPos == std::string::npos) { return; }
  std::string::size_type rightPos = flags.find(" -", leftPos);
  while(rightPos != std::string::npos)
    {
    // Pull out and store this argument.
    this->AddFlag(flags.substr(leftPos, rightPos-leftPos));
    
    // The next argument starts at the '-' from the previously found " -".
    leftPos = rightPos+1;
    rightPos = flags.find(" -", leftPos);
    }
  // Pull out and store the last argument.
  this->AddFlag(flags.substr(leftPos, std::string::npos));
}

//----------------------------------------------------------------------------
void gxFlagsParser::AddParsedFlags(std::vector<std::string>& resultArgs)
{
  for(std::vector<std::string>::const_iterator flag = m_Flags.begin();
      flag != m_Flags.end(); ++flag)
    {
    resultArgs.push_back(*flag);
    }
}

//----------------------------------------------------------------------------
void gxFlagsParser::AddFlag(const std::string& flag)
{
  // Used by Parse() to insert a parsed flag.  Strips trailing
  // whitespace from the argument.
  //
  // Includes a hack to split "-o /dev/null" into two arguments since
  // the parser only splits arguments with " -" occurrences.
  std::string tmp = flag.substr(0, flag.find_last_not_of(" \t")+1);
  if(tmp == "-o /dev/null")
    {
    m_Flags.push_back("-o");
    m_Flags.push_back("/dev/null");
    }
  else
    {
    m_Flags.push_back(tmp);
    }
}
