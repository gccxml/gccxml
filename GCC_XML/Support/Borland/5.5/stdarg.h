/*  stdarg.h

    Definitions for accessing parameters in functions that accept
    a variable number of arguments.

*/

/*
 *      C/C++ Run Time Library - Version 10.0
 *
 *      Copyright (c) 1987, 2000 by Inprise Corporation
 *      All Rights Reserved.
 *
 */

/* $Revision$ */

#ifndef __STDARG_H
#define __STDARG_H
#define _INC_STDARG  /* MSC Guard name */

#ifdef __VARARGS_H
#error Cannot include both STDARG.H and VARARGS.H
#endif

#ifndef ___STDDEF_H
#include <_stddef.h>
#endif

#ifdef __cplusplus
namespace std {
#endif /* __cplusplus */

typedef void _FAR *va_list;

#define __size(x) ((sizeof(x)+sizeof(int)-1) & ~(sizeof(int)-1))

#define va_start(ap, parmN) ((void)((ap) = (va_list)((char _FAR *)(&parmN)+__size(parmN))))
#define va_arg(ap, type) (*(type _FAR *)(((*(char _FAR *_FAR *)&(ap))+=__size(type))-(__size(type))))
#define va_end(ap)          ((void)0)

#if !defined(__STDC__)
#define _va_ptr             (...)
#endif

#ifdef __cplusplus
} // std
#endif /* __cplusplus */

#endif  /* __STDARG_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STDARG_H_USING_LIST)
#define __STDARG_H_USING_LIST
     using std::va_list;
#endif /* __USING_CNAME__ */
