/**
 * Program to copy and patch MSVC header files to work with GCC-XML.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>
#include <process.h>
#include <windows.h>
#include <direct.h>

bool readRegistryValue(const char *key, std::string& result);
bool copyTextFile(const char* source, const char* destination);

int main(int argc, char* argv[])
{
  std::string patchFile = argv[1];
  std::string destPath = argv[2];
  
  // Use an extra enclosing scope to make sure all destructors are called
  // before the exec occurs to run the patch program.
  {
  // The registry key to use when attempting to automatically find the
  // MSVC include files.
  const char* vcRegistry =
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++;ProductDir";

  if(argc < 3)
    {
    std::cout << "Usage:" << std::endl
              << "  " << argv[0] << " patch_file destination_path [source_path]" << std::endl
	      << "The source_path will be guessed from the registry if it is not given." << std::endl
	      << "All arguments should be given as full paths." << std::endl;
    return 0;
    }
  
  std::string sourcePath;

  // If the source_path argument was not given, look it up in the registry.
  if(argc < 4)
    {
    if(!readRegistryValue(vcRegistry, sourcePath))
      {
      std::cerr << "source_path not specified and VC98 could not be found in the registry!" << std::endl;
      return 1;
      }
    sourcePath += "/Include";
    }
  else
    {
    sourcePath = argv[3];
    }

  // Make sure the destination path exists before trying to put files
  // there.
  _mkdir(destPath.c_str());
  
  // Look at the patch file to see what headers need to be copied.
  std::ifstream patch(patchFile.c_str());
  if(!patch)
    {
    std::cerr << "Error opening patch file." << std::endl;
    return 1;
    }
  
  char buf[4096];
  for(patch.getline(buf, 4096); !patch.eof(); patch.getline(buf, 4096))
    {
    std::string line = buf;
    if(line.substr(0,6) == "Index:")
      {
      std::string source = sourcePath;
      source += "/"+line.substr(7);
      std::string dest = destPath.c_str();
      dest += "/"+line.substr(7);
      copyTextFile(source.c_str(), dest.c_str());
      }
    }
  }
  
  destPath = "\""+destPath+"\"";
  patchFile = "\""+patchFile+"\"";
  
  // The arguments for executing the patch program.
  const char* patchOptions[] =
  {
    "-p0",
    "-t",
    "-d ", destPath.c_str(),
    "-i ", patchFile.c_str(),
    0
  };
  
  // Patch the copies of the header files.
  if(_execvp("patch", patchOptions) < 0)
    {
    exit(errno);
    }

  return 0;
}


/**
 * Adapted from CMake's cmSystemTools.h: ReadAValue
 * Get the data of key value.
 * Example : 
 *      HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\2.1\InstallPath
 *      =>  will return the data of the "default" value of the key
 *      HKEY_LOCAL_MACHINE\SOFTWARE\Scriptics\Tcl\8.4;Root
 *      =>  will return the data of the "Root" value of the key
 */
bool readRegistryValue(const char *key, std::string& result)
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


/**
 * Copy a text file.  This is a hack implementation.
 */
bool copyTextFile(const char* source,
                  const char* destination)
{
  std::ifstream fin(source);
  if(!fin)
    {
    return false;
    }
  char buf[4096];
  std::ofstream fout(destination);
  if(!fout )
    {
    return false;
    }  
  for(fin.getline(buf, 4096); !fin.eof(); fin.getline(buf, 4096))
    {
    fout << buf << std::endl;
    }
  return true;
}
