/*=========================================================================

  Program:   GCC-XML
  Module:    gxSystemTools.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _gxSystemTools_h
#define _gxSystemTools_h

#include "gxConfigure.h"

#include <gxsys/SystemTools.hxx>

#if defined(_MSC_VER)
#pragma warning (disable: 4786)
#endif

#include <iostream>
#include <fstream>
#include <string>

/** Utility methods to hide platform differences.  */
class gxSystemTools: public gxsys::SystemTools
{
public:
  /** Run the given command and get back the output and the result value.  */
  static bool RunCommand(const char* command, std::string& output,
                         int &retVal);

  /** Copy a file named by "source" to the file named by
      "destination". */
  static bool FileCopy(const char* source, const char* destination);

  /** Given a path to a directory, convert it to a full path.  This
   * collapses away relative paths.  The full path is returned.  */
  static std::string CollapseDirectory(const char* in_dir);
};

#endif
