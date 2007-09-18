/*=========================================================================

  Program:   GCC-XML
  Module:    gxSystemTools.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gxSystemTools.h"

#include <vector>

#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#if !defined(_WIN32) || defined(__CYGWIN__)
#include <limits.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/wait.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

//----------------------------------------------------------------------------
bool gxSystemTools::RunCommand(const char* command,  std::string& output,
                               int &retVal)
{
  const int BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];
#if defined(WIN32) && !defined(__CYGWIN__)
  std::string commandToFile = command;
  commandToFile += " > ";
  std::string tempFile;
  char* temp =  _tempnam(0, "gccxml");
  if(!temp)
    {
    temp = "gccxmltemp";
    }
  tempFile += temp;
  commandToFile += tempFile;
  commandToFile += " 2>&1";
  retVal = system(commandToFile.c_str());
  std::ifstream fin(tempFile.c_str());
  if(!fin)
    {
    fin.close();
    gxSystemTools::RemoveFile(tempFile.c_str());
    return false;
    }
  while(fin)
    {
    fin.getline(buffer, BUFFER_SIZE);
    output += buffer;
    output += "\n";
    }
  fin.close();
  gxSystemTools::RemoveFile(tempFile.c_str());
  return (retVal == 0);
#else
  fflush(stdout);
  fflush(stderr);
  FILE* cpipe = popen(command, "r");
  if(!cpipe)
    {
    return false;
    }
  fgets(buffer, BUFFER_SIZE, cpipe);
  while(!feof(cpipe))
    {
    output += buffer;
    fgets(buffer, BUFFER_SIZE, cpipe);
    }

  retVal = pclose(cpipe);
  retVal = WEXITSTATUS(retVal);
  return (retVal == 0);
#endif
}

//----------------------------------------------------------------------------
bool gxSystemTools::FileCopy(const char* source, const char* destination)
{
  return gxsys::SystemTools::CopyFileAlways(source, destination);
}

//----------------------------------------------------------------------------
std::string gxSystemTools::CollapseDirectory(const char* in_dir)
{
  return gxsys::SystemTools::CollapseFullPath(in_dir);
}
