/*=========================================================================

  Program:   GCC-XML
  Module:    gxSystemTools.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gxSystemTools.h"

#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#ifndef _WIN32
#include <limits.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/wait.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <direct.h>
inline const char* Getcwd(char* buf, unsigned int len)
{
  return _getcwd(buf, len);
}
inline int Chdir(const char* dir)
{
  #if defined(__BORLANDC__)
  return chdir(dir);
  #else
  return _chdir(dir);
  #endif
}
#else
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
inline const char* Getcwd(char* buf, unsigned int len)
{
  return getcwd(buf, len);
}
inline int Chdir(const char* dir)
{
  return chdir(dir);
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
//----------------------------------------------------------------------------
bool gxSystemTools::ReadRegistryValue(const char* key, std::string& result)
{
  // Adapted from CMake's cmSystemTools.h: ReadAValue
  // Get the data of key value.
  // Example : 
  //      HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\2.1\InstallPath
  //      =>  will return the data of the "default" value of the key
  //      HKEY_LOCAL_MACHINE\SOFTWARE\Scriptics\Tcl\8.4;Root
  //      =>  will return the data of the "Root" value of the key
  
  // find the primary key
  std::string primary = key;
  std::string second;
  std::string valuename;
 
  size_t start = primary.find("\\");
  if(start == std::string::npos)
    {
    return false;
    }
  size_t valuenamepos = primary.find(";");
  if (valuenamepos != std::string::npos)
    {
    valuename = primary.substr(valuenamepos+1);
    }
  second = primary.substr(start+1, valuenamepos-start-1);
  primary = primary.substr(0, start);
  
  HKEY primaryKey;
  if(primary == "HKEY_CURRENT_USER")
    {
    primaryKey = HKEY_CURRENT_USER;
    }
  else if(primary == "HKEY_CURRENT_CONFIG")
    {
    primaryKey = HKEY_CURRENT_CONFIG;
    }
  else if(primary == "HKEY_CLASSES_ROOT")
    {
    primaryKey = HKEY_CLASSES_ROOT;
    }
  else if(primary == "HKEY_LOCAL_MACHINE")
    {
    primaryKey = HKEY_LOCAL_MACHINE;
    }
  else if(primary == "HKEY_USERS")
    {
    primaryKey = HKEY_USERS;
    }
  
  HKEY hKey;
  if(RegOpenKeyEx(primaryKey, second.c_str(),
                  0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
    return false;
    }
  else
    {
    DWORD dwType, dwSize;
    dwSize = 1023;
    char data[1024];
    if(RegQueryValueEx(hKey, (LPTSTR)valuename.c_str(), NULL, &dwType, 
                       (BYTE *)data, &dwSize) == ERROR_SUCCESS)
      {
      if (dwType == REG_SZ)
        {
        result = data;
        return true;
        }
      }
    }
  return false;
}
#else
//----------------------------------------------------------------------------
bool gxSystemTools::ReadRegistryValue(const char*, std::string&)
{
  return false;
}  
#endif

//----------------------------------------------------------------------------
bool gxSystemTools::FileCopy(const char* source, const char* destination)
{
  const int bufferSize = 4096;
  char buffer[bufferSize];
  std::ifstream fin(source,
#ifdef _WIN32
                    std::ios::binary |
#endif
                    std::ios::in);
  if(!fin) { return false; }
  std::ofstream fout(destination,
#ifdef _WIN32
                     std::ios::binary |
#endif
                     std::ios::out | std::ios::trunc);
  if(!fout) { return false; }
  
  // This copy loop is very sensitive on certain platforms with
  // slightly broken stream libraries (like HPUX).  Normally, it is
  // incorrect to not check the error condition on the fin.read()
  // before using the data, but the fin.gcount() will be zero if an
  // error occurred.  Therefore, the loop should be safe everywhere.
  while(fin)
    {
    fin.read(buffer, bufferSize);
    if(fin.gcount())
      {
      fout.write(buffer, fin.gcount());
      }
    }
  
  return true;
}

//----------------------------------------------------------------------------
std::string gxSystemTools::LowerCase(const std::string& s)
{
  std::string n;
  n.resize(s.size());
  for (size_t i = 0; i < s.size(); i++)
    {
    n[i] = tolower(s[i]);
    }
  return n;
}

//----------------------------------------------------------------------------
const char* gxSystemTools::GetEnv(const char* key)
{
  return getenv(key);
}

//----------------------------------------------------------------------------
bool gxSystemTools::GetEnv(const char* key, std::string& result)
{
  const char* v = getenv(key);
  if(v)
    {
    result = v;
    return true;
    }
  else
    {
    return false;
    }
}

//----------------------------------------------------------------------------
bool gxSystemTools::FileExists(const char* filename)
{
  struct stat fs;
  if(stat(filename, &fs) != 0) 
    {
    return false;
    }
  else
    {
    return true;
    }
}

//----------------------------------------------------------------------------
bool gxSystemTools::FileIsDirectory(const char* name)
{  
  struct stat fs;
  if(stat(name, &fs) == 0)
    {
#if _WIN32
    return ((fs.st_mode & _S_IFDIR) != 0);
#else
    return S_ISDIR(fs.st_mode);
#endif
    }
  else
    {
    return false;
    }
}

//----------------------------------------------------------------------------
std::string gxSystemTools::CollapseDirectory(const char* in_dir)
{
  std::string dir = in_dir;
#ifdef _WIN32
  // Ultra-hack warning:
  // This changes to the target directory, saves the working directory,
  // and then changes back to the original working directory.
  std::string cwd = gxSystemTools::GetCWD();
  if(dir != "") { Chdir(dir.c_str()); }
  std::string newDir = gxSystemTools::GetCWD();
  Chdir(cwd.c_str());
  return newDir;
#else
# ifdef MAXPATHLEN
  char resolved_name[MAXPATHLEN];
# else
#  ifdef PATH_MAX
  char resolved_name[PATH_MAX];
#  else
  char resolved_name[5024];
#  endif
# endif
  if(dir != "")
    {
    realpath(dir.c_str(), resolved_name);
    dir = resolved_name;
    }
  else
    {
    dir = gxSystemTools::GetCWD();
    }
  return dir;
#endif
}

//----------------------------------------------------------------------------
std::string gxSystemTools::GetCWD()
{
  char buf[2048];
  std::string path = Getcwd(buf, 2048);
  return path;
}

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
  tempFile += _tempnam(0, "gccxml");
  
  commandToFile += tempFile;
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
    }
  fin.close();
  cmSystemTools::RemoveFile(tempFile.c_str());
  return true;
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
  return true;
#endif
}

//----------------------------------------------------------------------------
bool gxSystemTools::RemoveFile(const char* source)
{
  return (unlink(source) != 0) ? false : true;
}
