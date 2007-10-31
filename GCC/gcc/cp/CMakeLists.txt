# Restore the real executable output path.  We want to put our executable
# there.
SET(EXECUTABLE_OUTPUT_PATH ${GCC_EXECUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

#-----------------------------------------------------------------------------
# C++ Compiler Executable

IF(CMAKE_SYSTEM MATCHES "HP-UX")
  IF(CMAKE_C_COMPILER MATCHES "(^cc$|/cc$)")
    SET_SOURCE_FILES_PROPERTIES(../c-opts.c PROPERTIES COMPILE_FLAGS "-Wp,-H,20000")
  ENDIF(CMAKE_C_COMPILER MATCHES "(^cc$|/cc$)")
ENDIF(CMAKE_SYSTEM MATCHES "HP-UX")

#-----------------------------------------------------------------------------
# Special flags for some source files.

IF(XCODE)
  SET_SOURCE_FILES_PROPERTIES(../prefix.c
    PROPERTIES COMPILE_FLAGS "-DPREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"")
ELSE(XCODE)
  SET_SOURCE_FILES_PROPERTIES(../prefix.c
    PROPERTIES COMPILE_FLAGS "-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ENDIF(XCODE)
IF(CYGWIN)
  SET_SOURCE_FILES_PROPERTIES(../cppdefault.c PROPERTIES
    COMPILE_FLAGS "-DTOOL_INCLUDE_DIR=/usr/include")
ENDIF(CYGWIN)

ADD_EXECUTABLE(gccxml_cc1plus
  cp-lang.c ../stub-objc.c call.c decl.c expr.c pt.c typeck2.c class.c
  decl2.c error.c lex.c parser.c ptree.c rtti.c typeck.c cvt.c except.c
  friend.c init.c method.c search.c semantics.c tree.c repo.c dump.c
  optimize.c mangle.c cp-objcp-common.c name-lookup.c cxx-pretty-print.c
  cp-gimplify.c ../tree-mudflap.c ../attribs.c ../c-common.c
  ../c-format.c ../c-pragma.c ../c-semantics.c ../c-lex.c ../c-dump.c
  ${cxx_target_srcs} ../c-pretty-print.c ../c-opts.c ../c-pch.c
  ../c-incpath.c ../cppdefault.c ../c-ppoutput.c ../c-cppbuiltin.c
  ../prefix.c ../c-gimplify.c ../c-omp.c ../tree-inline.c
  ../dummy-checksum.c ../main.c

  xml.c
  )

TARGET_LINK_LIBRARIES(gccxml_cc1plus backend cpp decnumber iberty)

# Use the "-lx" option to disable map file generation on Borland.
# This avoids the "Fatal: Access violation.  Link terminated." error.
IF(BORLAND)
  SET_TARGET_PROPERTIES(gccxml_cc1plus PROPERTIES LINK_FLAGS "-lx")
ENDIF(BORLAND)

# Install gccxml_cc1plus next to the gccxml executable.
INSTALL(TARGETS gccxml_cc1plus
  RUNTIME DESTINATION ${GCCXML_INSTALL_ROOT}bin
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_EXECUTABLE})
