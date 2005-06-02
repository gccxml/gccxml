ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)
INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_BINARY_DIR}/libiberty
)

SET(iberty_SRCS
  regex.c cplus-dem.c cp-demangle.c md5.c alloca.c argv.c choose-temp.c
  concat.c dyn-string.c fdmatch.c fibheap.c floatformat.c fnmatch.c
  getopt.c getopt1.c getpwd.c getruntime.c hashtab.c hex.c lbasename.c
  make-relative-prefix.c make-temp-file.c objalloc.c obstack.c
  partition.c physmem.c pexecute.c safe-ctype.c sort.c spaces.c
  splay-tree.c strerror.c strsignal.c ternary.c xatexit.c xexit.c
  xmalloc.c xmemdup.c xstrdup.c xstrerror.c
  )
FOREACH(f
    ASPRINTF ATEXIT BASENAME BCMP BCOPY BSEARCH BZERO CALLOC CLOCK
    COPYSIGN _DOPRNT FFS GETCWD GETPAGESIZE INDEX INSQUE MEMCHR MEMCMP
    MEMCPY MEMMOVE MEMSET MKSTEMPS PUTENV RANDOM RENAME RINDEX SETENV
    SIGSETMASK STRCASECMP STRCHR STRDUP STRNCASECMP STRNCMP STRRCHR STRSTR
    STRTOD STRTOL STRTOUL TMPNAM VASPRINTF VFORK VFPRINTF VPRINTF
    VSPRINTF
    )
  # Removed from above list: WAITPID
  IF(NOT HAVE_${f})
    STRING(TOLOWER "${f}" SRC)
    SET(iberty_SRCS ${iberty_SRCS} ${SRC}.c)
  ENDIF(NOT HAVE_${f})
ENDFOREACH(f)

ADD_LIBRARY(iberty ${iberty_SRCS})
