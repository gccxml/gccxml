/*=========================================================================

  Program:   GCC-XML
  Module:    gxSystemTools.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _gxSystemTools_h
#define _gxSystemTools_h

#include "gxConfigure.h"

#if defined(_MSC_VER)
#pragma warning (disable: 4786)
#endif

#include <iostream>
#include <fstream>
#include <string>

/** Utility methods to hide platform differences.  */
class gxSystemTools
{
public:
  /** Read a registry value with the given key.  Return whether it was
      found.  */
  static bool ReadRegistryValue(const char *key, std::string& result);
  
  /** Copy a file named by "source" to the file named by
      "destination". */
  static bool FileCopy(const char* source, const char* destination);
  
  /** Get a lower case string.  */
  static std::string LowerCase(const std::string& s);
  
  /** Read an environment variable.  */
  static const char* GetEnv(const char* key);
  static bool GetEnv(const char* key, std::string& result);
  
  /** Check if a file exists.  */
  static bool FileExists(const char* filename);
  
  /** Check if a file exists and is a directory.  */
  static bool FileIsDirectory(const char* name);
  
  /** Given a path to a directory, convert it to a full path.  This
   * collapses away relative paths.  The full path is returned.  */
  static std::string CollapseDirectory(const char* in_dir);
  
  /** Get the current working directory.  */
  static std::string GetCWD();
  
  /** Convert the given path to one with forward slashes.  */
  static void ConvertToUnixSlashes(std::string& path);

  /** Run the given command and get back the output and the result value.  */
  static bool RunCommand(const char* command, std::string& output,
                         int &retVal);
  
  /** Delete the file with the given name.  */
  static bool RemoveFile(const char* source);
  
  /** Convert a file path to a suitable output format.  */
  static std::string ConvertToOutputPath(const char* path);
  static std::string ConvertToWindowsOutputPath(const char* path);
  static std::string ConvertToUnixOutputPath(const char* path);
};

#endif
