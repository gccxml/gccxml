/*  io.h

    Definitions for low level I/O functions.

*/

/*
 *      C/C++ Run Time Library - Version 10.0
 *
 *      Copyright (c) 1987, 2000 by Inprise Corporation
 *      All Rights Reserved.
 *
 */

/* $Revision$ */

#ifndef __IO_H
#define __IO_H

#ifndef ___STDDEF_H
#include <_stddef.h>
#endif

#if !defined(___NFILE_H)
#include <_nfile.h>
#endif

#if !defined(RC_INVOKED)

#if defined(__STDC__)
#pragma warn -nak
#endif

#pragma pack(push, 1)

#endif  /* !RC_INVOKED */

#ifdef __cplusplus
namespace std {
#endif

extern  unsigned    _RTLENTRY _EXPDATA _nfile;

#define HANDLE_MAX   (_NFILE_)
struct  ftime   {
    unsigned    ft_tsec  : 5;   /* Two second interval */
    unsigned    ft_min   : 6;   /* Minutes */
    unsigned    ft_hour  : 5;   /* Hours */
    unsigned    ft_day   : 5;   /* Days */
    unsigned    ft_month : 4;   /* Months */
    unsigned    ft_year  : 7;   /* Year */
};

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#ifndef  _TIME_T
typedef long time_t;
#ifdef __cplusplus
#  define _TIME_T std::time_t
#else
#  define _TIME_T time_t
#endif /* __cplusplus */
#endif

/* MSC names for file attributes */

#define _A_NORMAL   0x00        /* Normal file, no attributes */
#define _A_RDONLY   0x01        /* Read only attribute */
#define _A_HIDDEN   0x02        /* Hidden file */
#define _A_SYSTEM   0x04        /* System file */
#define _A_VOLID    0x08        /* Volume label */
#define _A_SUBDIR   0x10        /* Directory */
#define _A_ARCH     0x20        /* Archive */

#ifndef MAXPATH
#define MAXPATH   260
#define MAXDIR    256
#define MAXFILE   256
#define MAXEXT    256
#define MAXDRIVE    3
#endif

/* MSC names for functions */

#define _access(_a,_b)      access(_a,_b)
#define _eof(_a)            eof(_a)

/* Microsoft C compatible findfirst() data types */

#if defined (_INTEGRAL_MAX_BITS) && (_INTEGRAL_MAX_BITS >= 64) && !defined(__STDC__)
struct _wfinddatai64_t {
        unsigned attrib;
        _TIME_T  time_create;   /* -1 for FAT file systems */
        _TIME_T  time_access;   /* -1 for FAT file systems */
        _TIME_T  time_write;
        __int64  size;
        wchar_t  name[MAXPATH];
};

struct _finddatai64_t {
        unsigned attrib;
        _TIME_T  time_create;   /* -1 for FAT file systems */
        _TIME_T  time_access;   /* -1 for FAT file systems */
        _TIME_T  time_write;
        __int64  size;
        char     name[MAXPATH];
};
#endif /* _INTEGRAL_MAX_BITS */

struct _wfinddata_t {
        unsigned attrib;
        _TIME_T  time_create;   /* -1 for FAT file systems */
        _TIME_T  time_access;   /* -1 for FAT file systems */
        _TIME_T  time_write;
        long     size;
        wchar_t  name[MAXPATH];
};

struct _finddata_t {
        unsigned attrib;
        _TIME_T  time_create;   /* -1 for FAT file systems */
        _TIME_T  time_access;   /* -1 for FAT file systems */
        _TIME_T  time_write;
        long     size;
        char     name[MAXPATH];
};

#ifdef __cplusplus
extern "C" {
#endif
int  _RTLENTRY _EXPFUNC access  (const char _FAR *__path, int __amode);
#ifdef __IN_CHMOD
int  _RTLENTRY _EXPFUNC _rtl_chmod  ();
int  _RTLENTRY _EXPFUNC _chmod  ();
#else
int  _RTLENTRY _EXPFUNC _rtl_chmod (const char _FAR *__pathname, int __func, ... );
int  _RTLENTRY _EXPFUNC _chmod     (const char _FAR *__pathname, int __func, ... );
#endif
int  _RTLENTRY _EXPFUNC chmod   (const char _FAR *__path, int __amode);
int  _RTLENTRY _EXPFUNC chsize  (int __handle, long __size);
int  _RTLENTRY _EXPFUNC _rtl_close  (int __handle);
int  _RTLENTRY _EXPFUNC _close  (int __handle);
int  _RTLENTRY _EXPFUNC close   (int __handle);
int  _RTLENTRY _EXPFUNC _rtl_creat  (const char _FAR *__path, int attribute);
int  _RTLENTRY _EXPFUNC _creat  (const char _FAR *__path, int attribute);
int  _RTLENTRY _EXPFUNC creat   (const char _FAR *__path, int __amode);
int  _RTLENTRY _EXPFUNC creatnew(const char _FAR *__path, int __mode); /* DOS 3.0 or later */
int  _RTLENTRY _EXPFUNC creattemp(char _FAR *__path, int __amode); /* DOS 3.0 or later */
int  _RTLENTRY _EXPFUNC dup   (int __handle);
int  _RTLENTRY _EXPFUNC dup2  (int __oldhandle, int __newhandle);
int  _RTLENTRY _EXPFUNC eof     (int __handle);
long _RTLENTRY _EXPFUNC filelength(int __handle);
int  _RTLENTRY _EXPFUNC getftime(int __handle, struct ftime _FAR *__ftimep);

#if defined (_INTEGRAL_MAX_BITS) && (_INTEGRAL_MAX_BITS >= 64) && !defined(__STDC__)
long _RTLENTRY _EXPFUNC _wfindfirsti64(wchar_t *, struct _wfinddatai64_t *);
long _RTLENTRY _EXPFUNC _wfindnexti64(long, struct _wfinddatai64_t *);
long _RTLENTRY _EXPFUNC _findfirsti64(char *, struct _finddatai64_t *);
long _RTLENTRY _EXPFUNC _findnexti64(long, struct _finddatai64_t *);
#endif /* _INTEGRAL_MAX_BITS */

long _RTLENTRY _EXPFUNC __wfindfirst(wchar_t *, struct _wfinddata_t *);
long _RTLENTRY _EXPFUNC __wfindnext(long, struct _wfinddata_t *);
long _RTLENTRY _EXPFUNC _findfirst(char *, struct _finddata_t *);
long _RTLENTRY _EXPFUNC _findnext(long, struct _finddata_t *);

int _RTLENTRY _EXPFUNC _findclose(long);

int  _RTLENTRY _EXPFUNC _waccess   (const wchar_t *__path, int __amode);
int  _RTLENTRY _EXPFUNC _wchmod    (const wchar_t *__path, int __amode);
int  _RTLENTRY _EXPFUNC _wrtl_chmod(const wchar_t *__pathname, int __func, ... );
int  _RTLENTRY _EXPFUNC _wcreat    (const wchar_t *__path, int __amode);
int  _RTLENTRY _EXPFUNC _wrtl_creat(const wchar_t *__path, int attribute);
int  _RTLENTRY _EXPFUNC _wunlink   (const wchar_t *__path);
int  _RTLENTRY _EXPFUNC _wremove   (const wchar_t * __path);
int  _RTLENTRY _EXPFUNC _wrename   (const wchar_t *__oldname,const wchar_t *__newname);
int  _RTLENTRY _EXPFUNC _wrtl_open (const wchar_t *__path, int __oflags);
wchar_t   * _RTLENTRY  _EXPFUNC     _wmktemp(wchar_t *__template);

long _RTLENTRY  _EXPFUNC _get_osfhandle(int __handle);
int  _RTLENTRY  _EXPFUNC _open_osfhandle(long __osfhandle, int __oflag);

#ifdef __IN_IOCTL
int  _RTLENTRY           ioctl   ();
#else
int  _RTLENTRY _EXPFUNC ioctl   (int __handle, int __func, ...);
        /* optional 3rd and 4th args are: void _FAR * __argdx, int argcx */
#endif

int  _RTLENTRY _EXPFUNC isatty (int __handle);
int  _RTLENTRY _EXPFUNC lock   (int __handle, long __offset, long __length);
int  _RTLENTRY _EXPFUNC locking(int __handle, int __mode, long __length);
long _RTLENTRY _EXPFUNC lseek  (int __handle, long __offset, int __fromwhere);
char * _RTLENTRY _EXPFUNC   _mktemp(char *__template );

#ifdef __IN_OPEN
int  _RTLENTRY _EXPFUNC open  ();
int  _RTLENTRY _EXPFUNC _wopen  ();
#else
int  _RTLENTRY _EXPFUNC open  (const char _FAR *__path, int __access,... /*unsigned mode*/);
#endif
int  _RTLENTRY _EXPFUNC _wopen(const wchar_t *__path, int __access,... /*unsigned mode*/);
int  _RTLENTRY _EXPFUNC _rtl_open (const char _FAR *__path, int __oflags);
int  _RTLENTRY _EXPFUNC _open (const char _FAR *__path, int __oflags);
int  _RTLENTRY _EXPFUNC read  (int __handle, void _FAR *__buf, unsigned __len);
int  _RTLENTRY _EXPFUNC _rtl_read (int __handle, void _FAR *__buf, unsigned __len);
int  _RTLENTRY _EXPFUNC _read (int __handle, void _FAR *__buf, unsigned __len);
int  _RTLENTRY _EXPFUNC remove(const char _FAR *__path);
int  _RTLENTRY _EXPFUNC rename(const char _FAR *__oldname,const char _FAR *__newname);
int  _RTLENTRY  _EXPFUNC setftime(int __handle, struct ftime _FAR *__ftimep);
int  _RTLENTRY  _EXPFUNC setmode(int __handle, int __amode);

#ifdef __IN_SOPEN
int  _RTLENTRY  _EXPFUNC _sopen ();
int  _RTLENTRY  _EXPFUNC _wsopen ();
#else
int  _RTLENTRY  _EXPFUNC _sopen (const char *__path, int __access, int __shflag,
                      ... /* unsigned mode */);
int  _RTLENTRY  _EXPFUNC _wsopen (const wchar_t *__path, int __access, int __shflag,
                      ... /* unsigned mode */);
#endif
long _RTLENTRY _EXPFUNC tell  (int __handle);
unsigned _RTLENTRY      umask (unsigned __cmask);
int  _RTLENTRY _EXPFUNC unlink(const char _FAR *__path);
int  _RTLENTRY _EXPFUNC unlock(int __handle, long __offset, long __length);
int  _RTLENTRY _EXPFUNC _rtl_write(int __handle, const void _FAR *__buf, unsigned __len);
int  _RTLENTRY _EXPFUNC _commit(int __handle);
int  _RTLENTRY _EXPFUNC _write(int __handle, const void _FAR *__buf, unsigned __len);
int  _RTLENTRY _EXPFUNC write (int __handle, const void _FAR *__buf, unsigned __len);
#if !defined(__STDC__)
char  * _RTLENTRY _EXPFUNC mktemp( char *__template );
#ifdef __IN_SOPEN
int  _RTLENTRY _EXPFUNC sopen ();
#else
int  _RTLENTRY _EXPFUNC sopen (const char *__path, int __access, int __shflag,
                      ... /* unsigned mode */);
#endif /* __IN_SOPEN */
#endif /* __STDC__ */

#define _lseek(__handle, __offset, __fromwhere) lseek(__handle, __offset, __fromwhere)
#define _dup(__h) dup(__h)

#ifdef __cplusplus
}
#endif


#if !defined(RC_INVOKED)

/* Obsolete functions */
#pragma obsolete _chmod

/* restore default packing */
#pragma pack(pop)

#if defined(__STDC__)
#pragma warn .nak
#endif

#endif  /* !RC_INVOKED */

#ifdef __cplusplus
} // std
#endif

#endif  /* __IO_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__IO_H_USING_LIST)
#define __IO_H_USING_LIST
    using std::_nfile;
    using std::access;
    using std::_rtl_chmod;
    using std::_chmod;
    using std::_rtl_chmod;
    using std::_chmod;
    using std::chmod;
    using std::chsize;
    using std::_rtl_close;
    using std::_close;
    using std::close;
    using std::_rtl_creat;
    using std::_creat;
    using std::creat;
    using std::creatnew;
    using std::creattemp;
    using std::dup;
    using std::dup2;
    using std::eof;
    using std::filelength;
    using std::getftime;
    using std::_waccess;
    using std::_wchmod;
    using std::_wrtl_chmod;
    using std::_wcreat;
    using std::_wrtl_creat;
    using std::_wunlink;
    using std::_wremove;
    using std::_wrename;
    using std::_wrtl_open;
    using std::_wmktemp;
    using std::_get_osfhandle;
    using std::_open_osfhandle;
    using std::ioctl;
    using std::isatty;
    using std::lock;
    using std::locking;
    using std::lseek;
    using std::_mktemp;
    using std::open;
    using std::_wopen;
    using std::open;
    using std::_wopen;
    using std::_rtl_open;
    using std::_open;
    using std::read;
    using std::_rtl_read;
    using std::_read;
    using std::setftime;
    using std::setmode;
    using std::_sopen;
    using std::_wsopen;
    using std::tell;
    using std::umask;
    using std::unlink;
    using std::unlock;
    using std::_commit;
    using std::_rtl_write;
    using std::_write;
    using std::write;
#if !defined(__STDC__)
    using std::mktemp;
    using std::sopen;
#endif
#if defined (_INTEGRAL_MAX_BITS) && (_INTEGRAL_MAX_BITS >= 64) && !defined(__STDC__)
    using std::_wfindfirsti64;
    using std::_wfindnexti64;
    using std::_findfirsti64;
    using std::_findnexti64;
    using std::_wfinddatai64_t;
    using std::_finddatai64_t;
#endif /* _INTEGRAL_MAX_BITS */
    using std::__wfindfirst;
    using std::__wfindnext;
    using std::_findfirst;
    using std::_findnext;
    using std::_findclose;
    using std::ftime;
    using std::_wfinddata_t;
    using std::_finddata_t;
#endif /* __USING_CNAME__ */
