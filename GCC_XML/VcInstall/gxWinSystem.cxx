#include "gxWinSystem.h"

#include <fstream>
#include <windows.h>
#include <ctype.h>

/*
 * Adapted from CMake's cmSystemTools.h: ReadAValue
 * Get the data of key value.
 * Example : 
 *      HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\2.1\InstallPath
 *      =>  will return the data of the "default" value of the key
 *      HKEY_LOCAL_MACHINE\SOFTWARE\Scriptics\Tcl\8.4;Root
 *      =>  will return the data of the "Root" value of the key
 */
bool gxWinSystem::ReadRegistryValue(const char* key, std::string& result)
{
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

bool gxWinSystem::gxCopyFile(const char* source, const char* destination)
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

// Return a lower case string 
std::string gxWinSystem::LowerCase(const std::string& s)
{
  std::string n;
  n.resize(s.size());
  for (size_t i = 0; i < s.size(); i++)
    {
    n[i] = tolower(s[i]);
    }
  return n;
}
