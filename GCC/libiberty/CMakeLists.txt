ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)
INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_BINARY_DIR}/libiberty
)

SET(iberty_SRCS
  regex.c cplus-dem.c cp-demangle.c md5.c alloca.c argv.c
  choose-temp.c concat.c cp-demint.c dyn-string.c fdmatch.c
  fibheap.c floatformat.c fnmatch.c fopen_unlocked.c getopt.c
  getopt1.c getpwd.c getruntime.c hashtab.c hex.c
  lbasename.c lrealpath.c make-relative-prefix.c
  make-temp-file.c objalloc.c obstack.c partition.c pexecute.c
  physmem.c pex-common.c pex-one.c # pex-unix.c
  safe-ctype.c
  sort.c spaces.c splay-tree.c strerror.c
  ternary.c unlink-if-ordinary.c xatexit.c xexit.c xmalloc.c
  xmemdup.c xstrdup.c xstrerror.c xstrndup.c
  )

FOREACH(f
    ASPRINTF ATEXIT BASENAME BCMP BCOPY BSEARCH BZERO CALLOC CLOCK GETCWD
    GETPAGESIZE INDEX INSQUE MKSTEMPS MEMCHR MEMCMP MEMCPY MEMMOVE MEMPCPY
    MEMSET PUTENV RANDOM RENAME RINDEX SIGSETMASK STRCASECMP SETENV STPCPY
    STPNCPY STRCHR STRDUP STRNCASECMP STRNDUP STRRCHR STRSTR STRTOD STRTOL
    STRTOUL STRVERSCMP TMPNAM VASPRINTF VFPRINTF VPRINTF VSPRINTF STRERROR
    STRSIGNAL FFS SNPRINTF VSNPRINTF
    )
  # Removed from above list:
  #   WAITPID WAIT3 WAIT4 SYSMP __FSETLOCKING GETRUSAGE ON_EXIT TIMES
  #   PSTAT_GETSTATIC PSTAT_GETDYNAMIC GETSYSINFO TABLE PSIGNAL SYSCONF
  #   REALPATH SYSCTL SBRK CANONICALIZE_FILE_NAME GETTIMEOFDAY
  IF(NOT HAVE_${f})
    STRING(TOLOWER "${f}" SRC)
    LIST(APPEND iberty_SRCS ${SRC}.c)
  ENDIF(NOT HAVE_${f})
ENDFOREACH(f)

ADD_LIBRARY(iberty ${iberty_SRCS})
