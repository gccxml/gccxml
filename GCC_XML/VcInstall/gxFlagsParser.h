#ifndef _gxFlagsParser_h
#define _gxFlagsParser_h

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif

#include <string>
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
