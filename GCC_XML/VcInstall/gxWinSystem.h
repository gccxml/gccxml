#ifndef _gccxmlWinSystem_h
#define _gccxmlWinSystem_h

#include <string>

/** Class with static utility methods.  */
class gxWinSystem
{
public:

  /** Read a registry value with the given key.  Return whether it was
      found.  */
  static bool ReadRegistryValue(const char *key, std::string& result);
  
  /** Copy a file named by "source" to the file named by
      "destination". */
  static bool gxCopyFile(const char* source, const char* destination);
  
  /** Get a lower case string.  */
  static std::string gxWinSystem::LowerCase(const std::string& s);
};

#endif
