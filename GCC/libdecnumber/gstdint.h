/* BEGIN GCC-XML MODIFICATIONS (2007/10/31 15:08:49) */
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
#else
# include <stdint.h>
#endif

#endif /* GCC_GSTDINT_H */
/* END GCC-XML MODIFICATIONS (2007/10/31 15:08:49) */
