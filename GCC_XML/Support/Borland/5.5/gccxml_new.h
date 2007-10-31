#ifndef __NEW_H
#define __NEW_H
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig
// -*- C++ -*-
#if !defined(_HPACC_)
#ifndef __STD_NEW
#define __STD_NEW

/***************************************************************************
 *
 * new - declarations for the Standard Library new header
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 **************************************************************************/

#include <stdcomp.h>
#include <rw/stddefs.h> 
#include <exception>
#include <alloc.h>  // __BORLANDC__ needed for malloc

#ifndef _RWSTD_NO_NAMESPACE 
namespace __rwstd {
#endif

extern const char _RWSTDExportFunc(*) __rw_stdexcept_BadAllocException;

#ifndef _RWSTD_NO_NAMESPACE
}
namespace std {
#endif

#if !defined(_RWSTD_BAD_ALLOC_DEFINED) || defined(__TURBOC__)
class _RWSTDExport bad_alloc : public exception     
{ 
  public:
    bad_alloc () _RWSTD_THROW_SPEC_NULL : exception( )
    { ; }
    bad_alloc(const bad_alloc&) _RWSTD_THROW_SPEC_NULL
    { ; }
    bad_alloc& operator=(const bad_alloc&) _RWSTD_THROW_SPEC_NULL
    { return *this; }
    virtual ~bad_alloc ()  _RWSTD_THROW_SPEC_NULL;

    virtual const char * what () const  _RWSTD_THROW_SPEC_NULL
    { 
      return __RWSTD::__rw_stdexcept_BadAllocException;
    }
};
#else
#ifndef _RWSTD_NO_NAMESPACE 
}
#endif
#if defined(_MSC_VER) && !defined(__BORLANDC__)
#include _RWSTD_MS42_HEADER(new)
#else
#include <new.h>
#endif
#ifndef _RWSTD_NO_NAMESPACE 
namespace std {
#endif
#endif /*_RWSTD_BAD_ALLOC_DEFINED */

#ifndef _RWSTD_NOTHROW_IN_STD
struct nothrow_t {};
extern nothrow_t nothrow;
#endif // _RWSTD_NOTHROW_IN_STD

#if !defined(_RWSTD_EXCEPTION_HANDLER_IN_STD) || defined(__BORLANDC__)
typedef void ( _RTLENTRY *new_handler) ();

#if !defined(_MSC_VER) || defined(__BORLANDC__)
extern new_handler _RTLENTRY _EXPFUNC set_new_handler( new_handler new_p );
#endif
#endif // ! _RWSTD_EXCEPTION_HANDLER_IN_STD || __BORLANDC__
#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#ifndef _RWSTD_NO_NEW_BRACKETS
#ifndef _RWSTD_NO_THROW_SPEC_ON_NEW
inline void * _RTLENTRY operator new(size_t, void* ptr)  _RWSTD_THROW_SPEC_NULL
 { return ptr; }
#if !defined(__sgi) || (defined(__sgi) && defined(_COMPILER_VERSION) && (_COMPILER_VERSION < 0x2D1))
inline void*  _RTLENTRY operator new[] ( size_t, void* ptr)  _RWSTD_THROW_SPEC_NULL
 { return ptr; }
#endif
#else
inline void * _RTLENTRY _EXPFUNC operator new(size_t, void*)
 { return ptr; }
#if !defined(__sgi) || (defined(__sgi) && defined(_COMPILER_VERSION) && (_COMPILER_VERSION < 0x2D1))
inline void*  _RTLENTRY operator new[] ( size_t, void* ptr)
 { return ptr; }
#endif
#endif // _RWSTD_NO_THROW_SPEC_ON_NEW
#else
#if defined(_MSC_VER) && !defined(__BORLANDC__)
#include <new.h>
extern void * _RTLENTRY _EXPFUNC operator new(size_t, void* ptr);
#else
#ifndef _RWSTD_NO_THROW_SPEC_ON_NEW
inline  void * _RTLENTRY operator new(size_t, void* ptr) _RWSTD_THROW_SPEC_NULL;
// { return ptr; }
#else
inline  void * _RTLENTRY operator new(size_t, void* ptr);
// { return ptr; }
#endif // _RWSTD_NO_THROW_SPEC_ON_NEW
#endif // _MSC_VER
#endif // _RWSTD_NO_NEW_BRACKETS

#ifdef __BORLANDC__
// Prototypes for the standard global new & delete operators
void * _RTLENTRY _EXPFUNC operator new (size_t);
void   _RTLENTRY _EXPFUNC operator delete (void *);

// inline versions of the nothrow_t versions of new & delete operators
inline void * _RTLENTRY operator new (size_t size, const std::nothrow_t &)
{
    size = size ? size : 1;
    return malloc(size);
}
inline void * _RTLENTRY operator new[] (size_t size, const std::nothrow_t &nt)
{
    return ::operator new (size, nt);
}
inline void _RTLENTRY operator delete (void *v, const std::nothrow_t &nt)
{
    ::operator delete (v);  // The standard operator doesn't throw any exceptions
}
inline void _RTLENTRY operator delete[] (void *v, const std::nothrow_t &nt)
{
    ::operator delete[] (v);  // The standard operator doesn't throw any exceptions
}
#endif // __BORLANDC__
#endif /*__STD_NEW */

#else
#include <new.h>
#endif /* !defined (_HPACC_) */

#pragma option pop
#endif /* __NEW_H */

#ifndef __USING_STD_NAMES__
  using namespace std;
#endif
