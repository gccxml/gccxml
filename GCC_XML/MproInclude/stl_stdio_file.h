/*
 * Copyright (c) 1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */ 

// WARNING: This is an internal header file, included by other C++
// standard library headers.  You should not attempt to use this header
// file directly.


#ifndef __SGI_STL_STDIO_FILE_H
#define __SGI_STL_STDIO_FILE_H

// This file provides a low-level interface between the internal 
// representation of struct FILE, from the C stdio library, and 
// the C++ I/O library.  The C++ I/O library views a FILE object as
// a collection of three pointers: the beginning of the buffer, the
// current read/write position, and the end of the buffer.

// The interface:
// - char* _FILE_begin(const FILE& __f);
//       Returns a pointer to the beginning of the buffer.
// - char* _FILE_next(const FILE& __f);
//       Returns the current read/write position within the buffer.
// - char* _FILE_end(const FILE& __f);
//       Returns a pointer immediately past the end of the buffer.
// - char* _FILE_avail(const FILE& __f);
//       Returns the number of characters remaining in the buffer, i.e.
//       _FILE_end(__f) - _FILE_next(__f).
// - char& _FILE_preincr(FILE& __f)
//       Increments the current read/write position by 1, returning the 
//       character at the old position.
// - char& _FILE_postincr(FILE& __f)
//       Increments the current read/write position by 1, returning the 
//       character at the old position.
// - char& _FILE_predecr(FILE& __f)
//       Decrements the current read/write position by 1, returning the 
//       character at the old position.
// - char& _FILE_postdecr(FILE& __f)
//       Decrements the current read/write position by 1, returning the 
//       character at the old position.
// - void _FILE_bump(FILE& __f, int __n)
//       Increments the current read/write position by __n.
// - void _FILE_set(FILE& __f, char* __begin, char* __next, char* __end);
//       Sets the beginning of the bufer to __begin, the current read/write
//       position to __next, and the buffer's past-the-end pointer to __end.
//       If any of those pointers is null, then all of them must be null.

//----------------------------------------------------------------------
// Implementation for the IRIX C library.
#if defined(__sgi)

#include <stdio.h>

inline char* _FILE_begin(const FILE& __f) { return (char*) __f._base; }
inline char* _FILE_next(const FILE& __f) { return (char*) __f._ptr; }  
inline char* _FILE_end(const FILE& __f)
  { return (char*) __f._ptr + __f._cnt; }

inline ptrdiff_t _FILE_avail(const FILE& __f) { return __f._cnt; }

inline char& _FILE_preincr(FILE& __f)
  { --__f._cnt; return *(char*) (++__f._ptr); }
inline char& _FILE_postincr(FILE& __f)
  { --__f._cnt; return *(char*) (__f._ptr++); }
inline char& _FILE_predecr(FILE& __f)
  { ++__f._cnt; return *(char*) (--__f._ptr); }
inline char& _FILE_postdecr(FILE& __f)
  { ++__f._cnt; return *(char*) (__f._ptr--); }
inline void  _FILE_bump(FILE& __f, int __n)
  { __f._ptr += __n; __f._cnt -= __n; }

inline void _FILE_set(FILE& __f, char* __begin, char* __next, char* __end) {
  __f._base = (unsigned char*) __begin;
  __f._ptr  = (unsigned char*) __next;
  __f._cnt  = __end - __next;
}

#else // A C library that we don't have an implementation for.
#error The C++ I/O library is not configured for this operating system 
#endif

#endif /* __SGI_STL_STDIO_FILE_H */

// Local Variables:
// mode:C++
// End:
