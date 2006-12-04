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

ADD_EXECUTABLE(gccxml_cc1plus
  call.c decl.c expr.c pt.c typeck2.c class.c decl2.c error.c lex.c
  parse.c ptree.c rtti.c spew.c typeck.c cvt.c except.c friend.c init.c
  method.c search.c semantics.c tree.c repo.c dump.c optimize.c mangle.c
  cp-lang.c
  ../attribs.c ../c-common.c ../c-format.c ../c-pragma.c
  ../c-semantics.c ../c-lex.c ../c-dump.c ../c-pretty-print.c
  ../c-opts.c
  ${cxx_target_srcs}
  xml.c
  )

TARGET_LINK_LIBRARIES(gccxml_cc1plus backend cpp iberty)

# Install gccxml_cc1plus next to the gccxml executable.
INSTALL(TARGETS gccxml_cc1plus
  RUNTIME DESTINATION ${GCCXML_INSTALL_ROOT}bin
  COMPONENT Development)
