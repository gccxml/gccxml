/*=========================================================================

  Program:   GCC-XML
  Module:    gxFlagsParser.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _gxFlagsParser_h
#define _gxFlagsParser_h

#include "gxSystemTools.h"

#include <vector>

/** Class to parse gccxml flags into individual arguments.  */
class gxFlagsParser
{
public:
  /** Parse the given set of flags.  */
  void Parse(const char* flags);

  /** Add the set of arguments to the given vector.  */
  void AddParsedFlags(std::vector<std::string>& resultArgs);
  
private:
  void AddFlag(const std::string&);
  
  std::vector<std::string> m_Flags;
};

#endif
