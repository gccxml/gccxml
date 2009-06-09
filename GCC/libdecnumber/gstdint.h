/* BEGIN GCC-XML MODIFICATIONS (2009/06/09 18:18:01) */
/* This file was manually written for gccxml's GCC. */
#ifndef GCC_GSTDINT_H
#define GCC_GSTDINT_H 1

#include <sys/types.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef          __int8  int8_t;
typedef          __int16 int16_t;
typedef          __int32 int32_t;
typedef          __int64 int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#elif defined(HAVE_STDINT_H)
# include <stdint.h>
#elif defined(HAVE_INTTYPES_H)
# include <inttypes.h>
#endif

#endif /* GCC_GSTDINT_H */
/* END GCC-XML MODIFICATIONS (2009/06/09 18:18:01) */
