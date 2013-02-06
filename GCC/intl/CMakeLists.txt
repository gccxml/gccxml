INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/intl/include
  ${GCC_BINARY_DIR}/intl
  )

# DEFINE SOURCE FILES #
# ------------------- #

set(INTL_HEADERS 
    gmo.h gettextP.h hash-string.h loadinfo.h plural-exp.h eval-plural.h
    localcharset.h  relocatable.h libgnuintl.h
    )

set(INTL_SOURCES 
    bindtextdom.c dcgettext.c dgettext.c gettext.c finddomain.c loadmsgcat.c
    localealias.c textdomain.c l10nflist.c explodename.c dcigettext.c
    dcngettext.c dngettext.c ngettext.c plural.y plural-exp.c localcharset.c
    relocatable.c localename.c log.c osdep.c intl-compat.c
    )

set(INTL_OBJECTS
    bindtextdom.o 
    dcgettext.o dgettext.o gettext.o finddomain.o loadmsgcat.o localealias.o
    textdomain.o l10nflist.o explodename.o dcigettext.o dcngettext.o
    dngettext.o ngettext.o plural.o plural-exp.o localcharset.o relocatable.o
    localename.o log.o osdep.o intl-compat.o
    )

MESSAGE("Running command ${CMAKE_COMMAND} -E COPY ${CMAKE_CURRENT_SOURCE_DIR}/libgnuintl.h ${CMAKE_CURRENT_BINARY_DIR}/libintl.h")

ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libintl.h
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/libgnuintl.h" "${CMAKE_CURRENT_BINARY_DIR}/libintl.h"
  )

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/libintl.h PROPERTIES GENERATED TRUE)
SET_SOURCE_FILES_PROPERTIES(gettext.c OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libintl.h)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckTypeSize)

# Function Probes #
# =============== #

# Need to test for each of the following symbols and add define statement to 
#  config.h

SET(TEST_FUNCTIONS
  feof_unlocked fgets_unlocked getc_unlocked getcwd getegid
  geteuid getgid getuid mempcpy munmap putenv setenv setlocale stpcpy
  strcasecmp strdup strtoul tsearch __argz_count __argz_stringify __argz_next
  __fsetlocking)

foreach(_function ${TEST_FUNCTIONS})
  string(TOUPPER ${_function} _define)
  CHECK_FUNCTION_EXISTS(${_function} HAVE_${_define})
endforeach()

# Include file tests #
# ================== #

CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
if (HAVE_SYS_TYPES_H)
  ADD_DEFINITIONS(-DHAVE_SYS_TYPES_H)
ENDIF(HAVE_SYS_TYPES_H)

CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
if (HAVE_INTTYPES_H)
  ADD_DEFINITIONS(-DHAVE_INTTYPES_H)
ENDIF(HAVE_INTTYPES_H)

CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
if (HAVE_STDINT_H)
  ADD_DEFINITIONS(-DHAVE_STDINT_H)
ENDIF(HAVE_STDINT_H)

CHECK_INCLUDE_FILES(stddef.h HAVE_STDDEF_H)
if (HAVE_STDDEF_H)
  ADD_DEFINITIONS(-DHAVE_STDDEF_H)
ENDIF(HAVE_STDDEF_H)


# Symbol Tests #
# ============ #

CHECK_SYMBOL_EXISTS(alloca      "alloca.h" HAVE_ALLOCA)
CHECK_SYMBOL_EXISTS(snprintf    "stdio.h"  HAVE_SNPRINTF)
CHECK_SYMBOL_EXISTS(asprintf    "wchar.h"  HAVE_ASPRINTF)
CHECK_SYMBOL_EXISTS(wprintf     "wchar.h"  HAVE_WPRINTF)
CHECK_SYMBOL_EXISTS(printf      "stdio.h"  HAVE_POSIX_PRINTF)
CHECK_SYMBOL_EXISTS(LC_MESSAGES "locale.h" HAVE_LC_MESSAGES)


# UINTMAX_T and INTMAX_T tests
##############################

## Check for uintmax_t ##

SET(CMAKE_EXTRA_INCLUDE_FILES inttypes.h)
CHECK_TYPE_SIZE(uintmax_t   UINTMAX_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)
IF(HAVE_UINTMAX_T)
  SET(HAVE_INTTYPES_H_WITH_UINTMAX "1")
  UNSET(HAVE_UINTMAX_T)
ENDIF(HAVE_UINTMAX_T)


SET(CMAKE_EXTRA_INCLUDE_FILES stdint.h)
CHECK_TYPE_SIZE(uintmax_t   UINTMAX_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)
IF(HAVE_UINTMAX_T)
  SET(HAVE_STDINT_H_WITH_UINTMAX "1")
ENDIF(HAVE_UINTMAX_T)

IF(NOT (HAVE_STDINT_H_WITH_UINTMAX_T OR HAVE_INTTYPES_H_WITH_UINTMAX) )
  SET(uintmax_t "uint64_t")
ELSE()
  SET(HAVE_UINTMAX_T "1")
ENDIF(NOT (HAVE_STDINT_H_WITH_UINTMAX_T OR HAVE_INTTYPES_H_WITH_UINTMAX) )

## Check for intmax_t ##
########################

SET(CMAKE_EXTRA_INCLUDE_FILES inttypes.h)
CHECK_TYPE_SIZE(intmax_t   INTMAX_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)
IF(HAVE_INTMAX_T)
  SET(HAVE_INTTYPES_H_WITH_INTMAX "1")
  UNSET(HAVE_INTMAX_T)
ENDIF(HAVE_INTMAX_T)

SET(CMAKE_EXTRA_INCLUDE_FILES stdint.h)
CHECK_TYPE_SIZE(intmax_t   INTMAX_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)
IF(HAVE_INTMAX_T)
  SET(HAVE_STDINT_H_WITH_INTMAX "1")
ENDIF(HAVE_INTMAX_T)

IF(NOT (HAVE_STDINT_H_WITH_INTMAX_T OR HAVE_INTTYPES_H_WITH_INTMAX) )
  SET(intmax_t "int64_t")
ELSE()
  SET(HAVE_INTMAX_T "1")
ENDIF(NOT (HAVE_STDINT_H_WITH_INTMAX_T OR HAVE_INTTYPES_H_WITH_INTMAX) )

#########################

ADD_DEFINITIONS(
  -DHAVE_CONFIG_H 
  -DHAVE_INTMAX_T
  -DHAVE_UINTMAX_T
  -DLOCALEDIR="${LOCALEDIR}" 
  -DLOCALE_ALIAS_PATH="${LOCALEDIR}"
  -DLIBDIR="${CMAKE_INSTALL_LIBDIR}"
  -DINSTALLDIR="${CMAKE_INSTALL_LIBDIR}"
  )

## Extra stuff in config.h...

# Define as const if the declaration of iconv() needs const.
SET(ICONV_CONST)

# Define if integer division by zero raises signal SIGFPE. */
#  INTDIV0_RAISES_SIGFPE

# Define because this is libintl.
set(IN_LIBINTL "1")

# Define because this is a library.
set(IN_LIBRARY "1")

SET(PACKAGE_BUGREPORT "\"\"")
SET(PACKAGE_NAME    "\"gccxml\"")
SET(PACKAGE_STRING  "\"GCC-XML\"")
SET(PACKAGE_TARNAME "\"gccxml-${GCC_BASEVER}.tar.gz\"")
SET(PACKAGE_URL     "\"\"")
SET(PACKAGE_VERSION ${GCC_BASEVER})

# Define to 1 if you have the ANSI C header files. */
SET(STDC_HEADERS "1")

# Define this entry point correctly. */
SET(relocate libintl_relocate)

# Define this entry point correctly. */
SET(set_relocation_prefix libintl_set_relocation_prefix)

IF(NOT HAVE_UINTMAX_T)
  SET(CMAKE_CONFIG_uintmax_t "unsigned long")
ENDIF(NOT HAVE_UINTMAX_T)

CONFIGURE_FILE(${GCC_SOURCE_DIR}/intl/config.h.in ${GCC_BINARY_DIR}/intl/config.h)
CONFIGURE_FILE(${GCC_SOURCE_DIR}/intl/config.intl.in ${GCC_BINARY_DIR}/intl/config.intl)

ADD_LIBRARY(intl ${INTL_SOURCES})
ADD_DEPENDENCIES(intl libintl.h)

