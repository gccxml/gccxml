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
inline int Mkdir(const char* dir)
{
  return _mkdir(dir);
}
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
inline int Mkdir(const char* dir)
{
  return mkdir(dir, 00777);
}
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
std::string gxSystemTools::GetFilenamePath(const char* filename)
{
  std::string fn = filename;
  gxSystemTools::ConvertToUnixSlashes(fn);
  
  std::string::size_type slash_pos = fn.rfind("/");
  if(slash_pos != std::string::npos)
    {
    return fn.substr(0, slash_pos);
    }
  else
    {
    return "";
    }
}

//----------------------------------------------------------------------------
std::string gxSystemTools::GetFilenameName(const char* filename)
{
  std::string fn = filename;
  gxSystemTools::ConvertToUnixSlashes(fn);
  
  std::string::size_type slash_pos = fn.rfind("/");
  if(slash_pos != std::string::npos)
    {
    return fn.substr(slash_pos+1);
    }
  else
    {
    return filename;
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
bool gxSystemTools::MakeDirectory(const char* name)
{
  std::string dir = name;
  if(dir.length() == 0)
    {
    return false;
    }
#ifdef _WIN32
  // The MSVC7 _mkdir function can only add one directory at a time.
  gxSystemTools::ConvertToUnixSlashes(dir);
  std::string::size_type pos = std::string::npos;
  std::vector<std::string> dirs;
  
  while(!gxSystemTools::FileIsDirectory(dir.substr(0, pos).c_str()))
    {
    dirs.push_back(dir.substr(0, pos).c_str());
    if(pos != std::string::npos)
      {
      pos = pos-1;
      }
    pos = dir.find_last_of("/", pos);
    if(pos == std::string::npos)
      {
      break;
      }
    }
  for(std::vector<std::string>::reverse_iterator i = dirs.rbegin();
      i != dirs.rend(); ++i)
    {
    if(Mkdir(i->c_str()) != 0)
      {
      return false;
      }
    }
  return true;
#else
  return (Mkdir(dir.c_str()) == 0);
#endif
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
void gxSystemTools::ConvertToUnixSlashes(std::string& path)
{
  std::string::size_type pos = 0;
  while((pos = path.find('\\', pos)) != std::string::npos)
    {
    path[pos] = '/';
    pos++;
    }
  // remove any trailing slash
  if(path.size() && path[path.size()-1] == '/')
    {
    path = path.substr(0, path.size()-1);
    }

  // if there is a tilda ~ then replace it with HOME
  if(path.find("~") == 0)
    {
    std::string home;
    if(gxSystemTools::GetEnv("HOME", home))
      {
      path = home + path.substr(1);
      }
    }
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
bool gxSystemTools::RemoveFile(const char* source)
{
  return (unlink(source) != 0) ? false : true;
}

//----------------------------------------------------------------------------
std::string gxSystemTools::ConvertToOutputPath(const char* path)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  return gxSystemTools::ConvertToWindowsOutputPath(path);
#else
  return gxSystemTools::ConvertToUnixOutputPath(path);
#endif
}

//----------------------------------------------------------------------------
std::string gxSystemTools::ConvertToWindowsOutputPath(const char* path)
{  
  // remove double slashes not at the start
  std::string ret = path;
  std::string::size_type pos = 0;
  // first convert all of the slashes
  while((pos = ret.find('/', pos)) != std::string::npos)
    {
    ret[pos] = '\\';
    pos++;
    }
  // check for really small paths
  if(ret.size() < 2)
    {
    return ret;
    }
  // now clean up a bit and remove double slashes
  // Only if it is not the first position in the path which is a network
  // path on windows
  pos = 1; // start at position 1
  while((pos = ret.find("\\\\", pos)) != std::string::npos)
    {
    ret.erase(pos, 1);
    }
  // now double quote the path if it has spaces in it
  // and is not already double quoted
  if(ret.find(" ") != std::string::npos
     && ret[0] != '\"')
    {
    std::string result;
    result = "\"" + ret + "\"";
    ret = result;
    }
  return ret;
}

//----------------------------------------------------------------------------
std::string gxSystemTools::ConvertToUnixOutputPath(const char* path)
{
  // change // to /, and escape any spaces in the path
  std::string ret = path;
  
  // remove // except at the beginning might be a cygwin drive
  std::string::size_type pos = 1;
  while((pos = ret.find("//", pos)) != std::string::npos)
    {
    ret.erase(pos, 1);
    }
  // now escape spaces if there is a space in the path
  if(ret.find(" ") != std::string::npos)
    {
    std::string result = "";
    char lastch = 1;
    for(const char* ch = ret.c_str(); *ch != '\0'; ++ch)
      {
      // if it is already escaped then don't try to escape it again
      if(*ch == ' ' && lastch != '\\')
        {
        result += '\\';
        }
      result += *ch;
      lastch = *ch;
      }
    ret = result;
    }
  return ret;
}

//----------------------------------------------------------------------------
void gxSystemToolsGetPath(std::vector<std::string>& path)
{
  // adds the elements of the env variable path to the arg passed in
#if defined(_WIN32) && !defined(__CYGWIN__)
  const char* pathSep = ";";
#else
  const char* pathSep = ":";
#endif
  std::string pathEnv;
  if(!gxSystemTools::GetEnv("PATH", pathEnv)) { return; }
  // A hack to make the below algorithm work.  
  if(pathEnv[pathEnv.length()-1] != pathSep[0])
    {
    pathEnv += pathSep;
    }
  std::string::size_type start =0;
  bool done = false;
  while(!done)
    {
    std::string::size_type endpos = pathEnv.find(pathSep, start);
    if(endpos != std::string::npos)
      {
      path.push_back(pathEnv.substr(start, endpos-start));
      start = endpos+1;
      }
    else
      {
      done = true;
      }
    }
  for(std::vector<std::string>::iterator i = path.begin();
      i != path.end(); ++i)
    {
    gxSystemTools::ConvertToUnixSlashes(*i);
    }
}

//----------------------------------------------------------------------------
std::string gxSystemTools::FindProgram(const char* name)
{
  // Find the executable with the given name.  Searches the system
  // path.  Returns the full path to the executable if it is found.
  // Otherwise, the empty string is returned.
  
  // See if the executable exists as written.
  if(gxSystemTools::FileExists(name) && !gxSystemTools::FileIsDirectory(name))
    {
    // Yes.  Convert it to a full path.
    std::string fullName = gxSystemTools::GetCWD()+"/"+name;
    std::string fileName = gxSystemTools::GetFilenameName(fullName.c_str());
    std::string fileDir = gxSystemTools::GetFilenamePath(fullName.c_str());
    fileDir = gxSystemTools::CollapseDirectory(fileDir.c_str());
    fullName = fileDir+"/"+fileName;
    return fullName;
    }
  
  std::string tryPath = name;
#ifdef _WIN32
  tryPath += ".exe";
  if(gxSystemTools::FileExists(tryPath.c_str()) &&
     !gxSystemTools::FileIsDirectory(tryPath.c_str()))
    {
    return tryPath.c_str();
    }
#endif

  // Get the system search path.
  std::vector<std::string> path;
  gxSystemToolsGetPath(path);
  
  for(std::vector<std::string>::const_iterator p = path.begin();
      p != path.end(); ++p)
    {
    tryPath = *p;
    tryPath += "/";
    tryPath += name;
    if(gxSystemTools::FileExists(tryPath.c_str()) &&
      !gxSystemTools::FileIsDirectory(tryPath.c_str()))
      {
      return tryPath.c_str();
      }
#ifdef _WIN32
    tryPath += ".exe";
    if(gxSystemTools::FileExists(tryPath.c_str()) &&
       !gxSystemTools::FileIsDirectory(tryPath.c_str()))
      {
      return tryPath.c_str();
      }
#endif
    }

  // Couldn't find the program.
  return "";
}

//----------------------------------------------------------------------------
std::string gxSystemTools::GetCurrentDateTime(const char* format)
{
  char buf[1024];
  time_t t;
  time(&t);
  strftime(buf, sizeof(buf), format, localtime(&t));
  return buf;
}
