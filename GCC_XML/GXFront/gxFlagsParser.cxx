/*=========================================================================

  Program:   GCC-XML
  Module:    gxFlagsParser.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gxFlagsParser.h"

#include <ctype.h>  /* isspace */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy */

/*--------------------------------------------------------------------------*/
static int gxFlagsCommandLineParseInternal(const char* in, char* out);

/*--------------------------------------------------------------------------*/
static int gxFlags_CommandLineParse(const char* in, char*** pargv)
{
  /* Allocate working space to store the parsed arguments.  The parsed
     command line with null terminators can be no longer than the size
     of the input buffer.  At least one character in the input is
     needed to delimit each argument, and only one is used in the
     output.  */
  char* buffer = (char*)malloc(strlen(in)+1);

  /* Parse the arguments into the working space.  */
  int argc = gxFlagsCommandLineParseInternal(in, buffer);

  /* Allocate the resulting arguments array.  */
  char** argv = (char**)malloc(sizeof(char*) * (argc+1));

  /* Loop through the arguments in the working space.  */
  char* begin = buffer;
  int i;
  for(i=0; i < argc; ++i)
    {
    /* Find the end of this argument.  */
    char* end = begin;
    while(*end) { ++end; }

    /* Allocate space for this argument.  */
    argv[i] = (char*)malloc(end-begin+1);

    /* Copy the argument into its space.  */
    memcpy(argv[i], begin, end-begin+1);

    /* Skip to the next argument.  */
    begin = end + 1;
    }

  /* Terminate the array of arguments.  */
  argv[argc] = 0;

  /* Store the argument array in the output.  */
  *pargv = argv;

  /* Free working space.  */
  free(buffer);

  /* Return the number of arguments parsed. */
  return argc;
}

/*--------------------------------------------------------------------------*/
static void gxFlags_CommandLineFree(char** argv)
{
  /* Free each argument.  */
  char** arg = argv;
  for(;*arg;++arg)
    {
    free(*arg);
    }

  /* Free the argument array.  */
  free(argv);
}

/*--------------------------------------------------------------------------*/
static int gxFlagsCommandLineParseInternal(const char* in, char* out)
{
  /* Iterate through the string once.  */
  const char* c = in;

  /* Count the number of arguments.  */
  int n = 0;

  /* Skip whitespace.  */
  for(;*c && isspace(*c); ++c);

  while(*c)
    {
    /* Keep track of quoting state.  */
    int quoted = 0;

    /* Parse this argument until whitespace is again encountered.  */
    for(;*c && (quoted || !isspace(*c)); ++c)
      {
      if(*c == '\\')
        {
        /* Skip the backslash.  */
        ++c;

        /* Store the next character verbatim.  */
        if(*c)
          {
          *out++ = *c;
          }
        }
      else if((quoted == 1 && *c == '\'') || (quoted == 2 && *c == '"'))
        {
        /* Close quote mode.  */
        quoted = 0;
        }
      else if(!quoted && *c == '\'')
        {
        /* Open single quote mode.  */
        quoted = 1;
        }
      else if(!quoted && *c == '"')
        {
        /* Open double quote mode.  */
        quoted = 2;
        }
      else
        {
        /* Store character in this argument.  */
        *out++ = *c;
        }
      }

    /* Terminate this argument.  */
    *out++ = '\0';

    /* Increment the argument count.  */
    ++n;

    /* Skip whitespace.  */
    for(;*c && isspace(*c); ++c);
    }

  /* Return the number of arguments parsed.  */
  return n;
}

//----------------------------------------------------------------------------
void gxFlagsParser::Parse(const char* in_flags)
{
  char** argv;
  int argc = gxFlags_CommandLineParse(in_flags, &argv);
  for(int i=0; i < argc; ++i)
    {
    this->AddFlag(argv[i]);
    }
  gxFlags_CommandLineFree(argv);
}

//----------------------------------------------------------------------------
void gxFlagsParser::AddParsedFlags(std::vector<std::string>& resultArgs)
{
  for(std::vector<std::string>::const_iterator flag = m_Flags.begin();
      flag != m_Flags.end(); ++flag)
    {
    resultArgs.push_back(*flag);
    }
}

//----------------------------------------------------------------------------
void gxFlagsParser::AddFlag(const std::string& flag)
{
  // Used by Parse() to insert a parsed flag.  Adjusts certain command
  // line options.
#if !defined(GCCXML_NATIVE_CC1PLUS)
    // If gccxml_cc1plus is provided separately, it will not have the
    // gccxml-specific -iwrapper option.  Change it to -I.
  if(flag.substr(0, 9) == "-iwrapper")
    {
    m_Flags.push_back("-I"+flag.substr(9, flag.length()-9));
    return;
    }
  else
#endif
    {
    m_Flags.push_back(flag);
    }
}
