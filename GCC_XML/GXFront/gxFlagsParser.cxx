/*=========================================================================

  Program:   GCC-XML
  Module:    gxFlagsParser.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

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
  if(flag.substr(0, 9) == "-include ")
    {
    m_Flags.push_back("-include");
    m_Flags.push_back(flag.substr(9, flag.find_last_not_of(" \t")-8));
    }
#if !defined(GCCXML_NATIVE_CC1PLUS)
    // If gccxml_cc1plus is provided separately, it will not have the
    // gccxml-specific -iwrapper option.  Change it to -I.
  else if(flag.substr(0, 9) == "-iwrapper")
    {
    m_Flags.push_back("-I"+flag.substr(9, flag.find_last_not_of(" \t")-8));
    }
#endif
  else
    {
    m_Flags.push_back(flag.substr(0, flag.find_last_not_of(" \t")+1));
    }
}
