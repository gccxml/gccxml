#ifdef TEST_HAVE_MMAP
# define gcc_default_includes
#endif
#ifdef TEST_HAVE_UINTPTR_T
# define gcc_default_includes
#endif
#ifdef TEST_HAVE_PID_T
# define gcc_default_includes
#endif
#ifdef TEST_HAVE_UINT64_T
# define gcc_default_includes
#endif

#ifdef gcc_default_includes
#include <stdio.h>
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#if HAVE_STRINGS_H
# include <strings.h>
#endif
#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* libiberty tests */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef TEST_gcc_ac_cv_c_const
int
main ()
{
/* FIXME: Include the comments suggested by Paul. */
#ifndef __cplusplus
  /* Ultrix mips cc rejects this.  */
  typedef int charset[2];
  const charset x;
  /* SunOS 4.1.1 cc rejects this.  */
  char const *const *ccp;
  char **p;
  /* NEC SVR4.0.2 mips cc rejects this.  */
  struct point {int x, y;};
  static struct point const zero = {0,0};
  /* AIX XL C 1.02.0.0 rejects this.
     It does not let you subtract one const X* pointer from another in
     an arm of an if-expression whose if-part is not a constant
     expression */
  const char *g = "string";
  ccp = &g + (g ? g-g : 0);
  /* HPUX 7.0 cc rejects these. */
  ++ccp;
  p = (char**) ccp;
  ccp = (char const *const *) p;
  { /* SCO 3.2v4 cc rejects this.  */
    char *t;
    char const *s = 0 ? (char *) 0 : (char const *) 0;

    *t++ = 0;
  }
  { /* Someone thinks the Sun supposedly-ANSI compiler will reject this.  */
    int x[] = {25, 17};
    const int *foo = &x[0];
    ++foo;
  }
  { /* Sun SC1.0 ANSI compiler rejects this -- but not the above. */
    typedef const int *iptr;
    iptr p = 0;
    ++p;
  }
  { /* AIX XL C 1.02.0.0 rejects this saying
       "k.c", line 2.27: 1506-025 (S) Operand must be a modifiable lvalue. */
    struct s { int j; const int *ap[3]; };
    struct s *b; b->j = 5;
  }
  { /* ULTRIX-32 V3.1 (Rev 9) vcc rejects this */
    const int foo = 10;
  }
#endif

  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_SYS_WAIT_H
#include <sys/types.h>
#include <sys/wait.h>
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

int
main ()
{
  int s;
  wait (&s);
  s = WIFEXITED (s) ? WEXITSTATUS (s) : 1;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_TIME_WITH_SYS_TIME
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <sys/time.h>
#include <time.h>

int
main ()
{
if ((struct tm *) 0)
return 0;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_NEED_DECLARATION_ERRNO
#include <errno.h>
int
main ()
{
int x = errno;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_gcc_ac_cv_func_fork
/* System header to define __stub macros and hopefully few prototypes,
    which can conflict with char fork (); below.
    Prefer <limits.h> to <assert.h> if __STDC__ is defined, since
    <limits.h> exists even on freestanding compilers.  Under hpux,
    including <limits.h> includes <sys/time.h> and causes problems
    checking for functions defined therein.  */
#if defined (__STDC__) && !defined (_HPUX_SOURCE)
# include <limits.h>
#else
# include <assert.h>
#endif
/* Override any gcc2 internal prototype to avoid an error.  */
#ifdef __cplusplus
extern "C"
{
#endif
/* We use char because int might match the return type of a gcc2
   builtin and then its argument prototype would still apply.  */
char fork ();
/* The GNU C library defines this for functions which it implements
    to always fail with ENOSYS.  Some functions are actually named
    something starting with __ and the normal name is an alias.  */
#if defined (__stub_fork) || defined (__stub___fork)
choke me
#else
char (*f) () = fork;
#endif
#ifdef __cplusplus
}
#endif

int
main ()
{
return f != fork;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_gcc_ac_cv_func_vfork
/* System header to define __stub macros and hopefully few prototypes,
    which can conflict with char vfork (); below.
    Prefer <limits.h> to <assert.h> if __STDC__ is defined, since
    <limits.h> exists even on freestanding compilers.  Under hpux,
    including <limits.h> includes <sys/time.h> and causes problems
    checking for functions defined therein.  */
#if defined (__STDC__) && !defined (_HPUX_SOURCE)
# include <limits.h>
#else
# include <assert.h>
#endif
/* Override any gcc2 internal prototype to avoid an error.  */
#ifdef __cplusplus
extern "C"
{
#endif
/* We use char because int might match the return type of a gcc2
   builtin and then its argument prototype would still apply.  */
char vfork ();
/* The GNU C library defines this for functions which it implements
    to always fail with ENOSYS.  Some functions are actually named
    something starting with __ and the normal name is an alias.  */
#if defined (__stub_vfork) || defined (__stub___vfork)
choke me
#else
char (*f) () = vfork;
#endif
#ifdef __cplusplus
}
#endif

int
main ()
{
return f != vfork;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_UINTPTR_T
int
main ()
{
if ((uintptr_t *) 0)
  return 0;
if (sizeof (uintptr_t))
  return 0;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_PID_T
int
main ()
{
if ((pid_t *) 0)
  return 0;
if (sizeof (pid_t))
  return 0;
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_STDC_HEADERS
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>

int
main ()
{

  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_NEED_DECLARATION_CANONICALIZE_FILE_NAME
#include <stdio.h>
#if HAVE_STRING_H
#include <string.h>
#else
#if HAVE_STRINGS_H
#include <strings.h>
#endif
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
int
main ()
{
char *(*pfn) = (char *(*)) canonicalize_file_name
  ;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_MMAP
/* malloc might have been renamed as rpl_malloc. */
#undef malloc

/* Thanks to Mike Haertel and Jim Avera for this test.
   Here is a matrix of mmap possibilities:
        mmap private not fixed
        mmap private fixed at somewhere currently unmapped
        mmap private fixed at somewhere already mapped
        mmap shared not fixed
        mmap shared fixed at somewhere currently unmapped
        mmap shared fixed at somewhere already mapped
   For private mappings, we should verify that changes cannot be read()
   back from the file, nor mmap's back from the file at a different
   address.  (There have been systems where private was not correctly
   implemented like the infamous i386 svr4.0, and systems where the
   VM page cache was not coherent with the file system buffer cache
   like early versions of FreeBSD and possibly contemporary NetBSD.)
   For shared mappings, we should conversely verify that changes get
   propagated back to all the places they're supposed to be.

   Grep wants private fixed already mapped.
   The main things grep needs to know about mmap are:
   * does it exist and is it safe to write into the mmap'd area
   * how to use it (BSD variants)  */

#include <fcntl.h>
#include <sys/mman.h>

#if !STDC_HEADERS && !HAVE_STDLIB_H
char *malloc ();
#endif

/* This mess was copied from the GNU getpagesize.h.  */
#if !HAVE_GETPAGESIZE
/* Assume that all systems that can run configure have sys/param.h.  */
# if !HAVE_SYS_PARAM_H
#  define HAVE_SYS_PARAM_H 1
# endif

# ifdef _SC_PAGESIZE
#  define getpagesize() sysconf(_SC_PAGESIZE)
# else /* no _SC_PAGESIZE */
#  if HAVE_SYS_PARAM_H
#   include <sys/param.h>
#   ifdef EXEC_PAGESIZE
#    define getpagesize() EXEC_PAGESIZE
#   else /* no EXEC_PAGESIZE */
#    ifdef NBPG
#     define getpagesize() NBPG * CLSIZE
#     ifndef CLSIZE
#      define CLSIZE 1
#     endif /* no CLSIZE */
#    else /* no NBPG */
#     ifdef NBPC
#      define getpagesize() NBPC
#     else /* no NBPC */
#      ifdef PAGESIZE
#       define getpagesize() PAGESIZE
#      endif /* PAGESIZE */
#     endif /* no NBPC */
#    endif /* no NBPG */
#   endif /* no EXEC_PAGESIZE */
#  else /* no HAVE_SYS_PARAM_H */
#   define getpagesize() 8192   /* punt totally */
#  endif /* no HAVE_SYS_PARAM_H */
# endif /* no _SC_PAGESIZE */

#endif /* no HAVE_GETPAGESIZE */

int
main ()
{
  char *data, *data2, *data3;
  int i, pagesize;
  int fd;

  pagesize = getpagesize ();

  /* First, make a file with some known garbage in it. */
  data = (char *) malloc (pagesize);
  if (!data)
    exit (1);
  for (i = 0; i < pagesize; ++i)
    *(data + i) = rand ();
  umask (0);
  fd = creat ("conftest.mmap", 0600);
  if (fd < 0)
    exit (1);
  if (write (fd, data, pagesize) != pagesize)
    exit (1);
  close (fd);

  /* Next, try to mmap the file at a fixed address which already has
     something else allocated at it.  If we can, also make sure that
     we see the same garbage.  */
  fd = open ("conftest.mmap", O_RDWR);
  if (fd < 0)
    exit (1);
  data2 = (char *) malloc (2 * pagesize);
  if (!data2)
    exit (1);
  data2 += (pagesize - ((int) data2 & (pagesize - 1))) & (pagesize - 1);
  if (data2 != mmap (data2, pagesize, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_FIXED, fd, 0L))
    exit (1);
  for (i = 0; i < pagesize; ++i)
    if (*(data + i) != *(data2 + i))
      exit (1);

  /* Finally, make sure that changes to the mapped area do not
     percolate back to the file as seen by read().  (This is a bug on
     some variants of i386 svr4.0.)  */
  for (i = 0; i < pagesize; ++i)
    *(data2 + i) = *(data2 + i) + 1;
  data3 = (char *) malloc (pagesize);
  if (!data3)
    exit (1);
  if (read (fd, data3, pagesize) != pagesize)
    exit (1);
  for (i = 0; i < pagesize; ++i)
    if (*(data + i) != *(data3 + i))
      exit (1);
  close (fd);
  exit (0);
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_STACK_DIRECTION
find_stack_direction ()
{
  static char *addr = 0;
  auto char dummy;
  if (addr == 0)
    {
      addr = &dummy;
      return find_stack_direction ();
    }
  else
    return (&dummy > addr) ? 1 : -1;
}
main ()
{
  exit (find_stack_direction() < 0);
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_CRAY_STACKSEG_END
int main()
{
#if defined(CRAY) && ! defined(CRAY2)
  return 0;
#else
  not_cray;
#endif
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_SYS_RESOURCE_H_WITH_SYS_TIME_H
#include <sys/time.h>
#include <sys/resource.h>
int main()
{
  struct rlimit rlim;
  (void)rlim;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* gcc tests not in libiberty */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef TEST_GETGROUPS_T
/* Thanks to Mike Rendell for this test.  */
#include <sys/types.h>
#define NGID 256
#undef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
main()
{
  gid_t gidset[NGID];
  int i, n;
  union { gid_t gval; long lval; }  val;

  val.lval = -1;
  for (i = 0; i < NGID; i++)
    gidset[i] = val.gval;
  n = getgroups (sizeof (gidset) / MAX (sizeof (int), sizeof (gid_t)) - 1,
                 gidset);
  /* Exit non-zero if getgroups seems to require an array of ints.  This
     happens when gid_t is short but getgroups modifies an array of ints.  */
  exit ((n > 0 && gidset[n] != val.gval) ? 1 : 0);
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_STRING_WITH_STRINGS
#include <string.h>
#include <strings.h>
int main() {

; return 0; }
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST__GNU_SOURCE
#include <features.h>
int main() {

#if ! (defined __GLIBC__ || defined __GNU_LIBRARY__)
#error Not a GNU C library system
#endif
; return 0; }
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_WORKING_MBSTOWCS
#include <stdlib.h>
int main()
{
  mbstowcs(0, "", 0);
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_PRINTF_PTR
#include <stdio.h>

int main()
{
  char buf[64];
  char *p = buf, *q = NULL;
  sprintf(buf, "%p", p);
  sscanf(buf, "%p", &q);
  return (p != q);
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_LC_MESSAGES
#include <locale.h>
int main() {
return LC_MESSAGES
; return 0; }
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_STRUCT_TMS
#include "gcc_test_auto_host.h"
#include "ansidecl.h"
#include "system.h"
#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif

int main() {
struct tms tms;
; return 0; }
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_CLOCK_T
#include "gcc_test_auto_host.h"
#include "ansidecl.h"
#include "system.h"

int main() {
clock_t x;
; return 0; }
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_INO_T
#include <sys/types.h>

int main()
{
  ino_t x;
  (void)x;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_DEV_T
#include <sys/types.h>

int main()
{
  dev_t x;
  (void)x;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_INITFINI_ARRAY
static int x = -1;
int main (void) { return x; }
int foo (void) { x = 0; }
int (*fp) (void) __attribute__ ((section (".init_array"))) = foo;
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_SYS_TYPES_HAS_SSIZE_T
#include <sys/types.h>

ssize_t f(ssize_t x) { return x; }

int main()
{
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_SYS_TYPES_HAS_OFF_T
#include <sys/types.h>

off_t f(off_t x) { return x; }

int main()
{
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_DIRENT_H
#include <sys/types.h>
#include <dirent.h>

int main()
{
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_SYS_ERRLIST_IN_STDLIB_H
#include <stdlib.h>

int main()
{
  return sys_errlist?0:1;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE__SYSTEM_CONFIGURATION
extern int _system_configuration;

int main()
{
  return _system_configuration;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_SYS_ERRLIST
extern int sys_errlist;

int main()
{
  return sys_errlist;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_SYS_NERR
extern int sys_nerr;

int main()
{
  return sys_nerr;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_SYS_SIGLIST
extern int sys_siglist;

int main()
{
  return sys_siglist;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE__DOPRNT
extern int _doprnt;

int main()
{
  return _doprnt;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_UINT64_T
int main()
{
  extern uint64_t foo;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_HAVE_LANGINFO_CODESET
#include <langinfo.h>
int main()
{
  char* cs = nl_langinfo(CODESET);
  (void)cs;
  return 0;
}
#endif

/*--------------------------------------------------------------------------*/
#ifdef TEST_TM_IN_TIME_H
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <time.h>
int main()
{
  struct tm *tp;
  (void)tp->tm_sec;
  return 0;
}
#endif
