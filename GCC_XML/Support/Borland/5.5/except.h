/*  except.h

    Definitions for exception handling

*/

/*
 *      C/C++ Run Time Library - Version 10.0
 *
 *      Copyright (c) 1992, 2000 by Inprise Corporation
 *      All Rights Reserved.
 *
 */

/* $Revision$ */


#ifndef __EXCEPT_H
#define __EXCEPT_H
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig

#ifndef __cplusplus
#  error Must use C++ for except.h
#endif

#if !defined(___STDLIB_H)
#  include <stdlib.h>
#endif

#if !defined(__STD_EXCEPTION)
#  include <exception>
#endif

// forward declare string
namespace std {

template<class charT> struct _RWSTDExportTemplate char_traits;
template<class T> class _RWSTDExportTemplate allocator;

template<class charT, class traits, class Allocator> class _RWSTDExportTemplate
basic_string;

typedef basic_string<char, char_traits<char>, allocator<char> > string;

}

#if !defined(RC_INVOKED)

#pragma pack(push, 1)
#pragma option push -Vo-     // set standard C++ options

#if defined(__STDC__)
#pragma warn -nak
#endif

#endif  /* !RC_INVOKED */


namespace std {
typedef void (_RTLENTRY *terminate_handler)();
typedef void (_RTLENTRY *unexpected_handler)();


terminate_handler  _RTLENTRY set_terminate(terminate_handler);
unexpected_handler _RTLENTRY set_unexpected(unexpected_handler);

void  _RTLENTRY terminate();
void  _RTLENTRY unexpected();
bool  _RTLENTRY uncaught_exception();
} // std

#ifndef __STDC__
// For backwards compatibility ...
typedef std::unexpected_handler unexpected_function;
typedef std::terminate_handler terminate_function;
#pragma obsolete terminate_function
#pragma obsolete unexpected_function
#endif // !__STDC__


extern  char *      _RTLENTRY __ThrowFileName();
extern  unsigned    _RTLENTRY __ThrowLineNumber();
extern  char *      _RTLENTRY __ThrowExceptionName();

#define  __throwFileName      __ThrowFileName()
#define  __throwLineNumber    __ThrowLineNumber()
#define  __throwExceptionName __ThrowExceptionName()

class _EXPCLASS xmsg : public std::exception
{
public:
    xmsg(const std::string &msg);
    xmsg(const xmsg &);
    virtual ~xmsg() throw();
    xmsg & operator=(const xmsg &);

    virtual const char * what() const throw();
    const std::string & why() const;
  void                raise();// throw(xmsg);

private:
    std::string *str;
};

inline const std::string & xmsg::why() const
{
    return *str;
};

/* The xalloc class is here for backwards compatibility ONLY!  Operator new
   will not throw one of these anymore.  Operator new now throws a bad_alloc
   instead.
*/

class _EXPCLASS xalloc : public xmsg
{
public:
    xalloc(const std::string &msg, _SIZE_T size);

    _SIZE_T requested() const;
  void    raise();// throw(xalloc);

private:
    _SIZE_T siz;
};


inline xalloc::xalloc(const std::string &msg, _SIZE_T size)
        : xmsg(msg), siz(size)
{
}

inline _SIZE_T xalloc::requested() const
{
    return siz;
}

#pragma obsolete xalloc
#pragma obsolete xmsg

#if !defined(RC_INVOKED)

#if defined(__STDC__)
#pragma warn .nak
#endif

#pragma option pop      // restore user C++ options
/* restore default packing */
#pragma pack(pop)

#endif  /* !RC_INVOKED */

#pragma option pop
#endif  // __EXCEPT_H
