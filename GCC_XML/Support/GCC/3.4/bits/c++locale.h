#ifndef GCCXML_BITS_CXXLOCALE
#define GCCXML_BITS_CXXLOCALE

#include <clocale>
#include <cstring>
#include <cstdio>

#ifndef __attribute__
# define __attribute__(x)
# define GCCXML_BITS_CXXLOCALE_DEFINED_ATTRIBUTE
#endif

#include_next <bits/c++locale.h>

#ifdef GCCXML_BITS_CXXLOCALE_DEFINED_ATTRIBUTE
# undef __attribute__
#endif

#endif
