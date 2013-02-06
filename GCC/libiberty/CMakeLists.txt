ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)
INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_BINARY_DIR}/libiberty
)

SET(iberty_SRCS
  # ADDED for 4.7
  asprintf.c atexit.c
	basename.c bcmp.c bcopy.c bsearch.c bzero.c
	calloc.c choose-temp.c clock.c concat.c cp-demangle.c
	crc32.c
	ffs.c filename_cmp.c
	getcwd.c getpagesize.c 
  # gettimeofday.c
	index.c insque.c
	memchr.c memcmp.c memcpy.c memmem.c
	memmove.c mempcpy.c memset.c mkstemps.c
  #	pex-djgpp.c 
  # pex-msdos.c
  # pex-win32.c
  putenv.c
	random.c rename.c rindex.c
  setenv.c setproctitle.c sha1.c sigsetmask.c
  simple-object.c simple-object-coff.c simple-object-elf.c
  simple-object-mach-o.c
  snprintf.c
  stack-limit.c stpcpy.c stpncpy.c
  strcasecmp.c strchr.c strdup.c strncasecmp.c
  strncmp.c strrchr.c strsignal.c strstr.c strtod.c strtol.c
  strtoul.c strndup.c strverscmp.c
  timeval-utils.c tmpnam.c
  vasprintf.c vfork.c 
  #  vfprintf.c 
  vprintf.c 
  vsnprintf.c
  vsprintf.c
  waitpid.c

  # FROM 4.2 compatibility branch. Removed files commented out.

  regex.c cplus-dem.c cp-demangle.c md5.c alloca.c argv.c
  choose-temp.c concat.c cp-demint.c dyn-string.c fdmatch.c
  fibheap.c floatformat.c fnmatch.c fopen_unlocked.c getopt.c
  getopt1.c getpwd.c getruntime.c hashtab.c hex.c
  lbasename.c lrealpath.c make-relative-prefix.c
  make-temp-file.c objalloc.c obstack.c partition.c pexecute.c
  physmem.c pex-common.c pex-one.c 
  # pex-unix.c
  safe-ctype.c
  sort.c spaces.c splay-tree.c strerror.c
  # ternary.c
  unlink-if-ordinary.c xatexit.c xexit.c xmalloc.c
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
