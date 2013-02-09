# Restore the real executable output path.  We want to put our executable
# there.
SET(EXECUTABLE_OUTPUT_PATH ${GCC_EXECUTABLE_OUTPUT_PATH})
SET(GCCXML_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib/libgccxml_plugin${CMAKE_SHARED_LIBRARY_SUFFIX}")

IF(UNIX)
  SET(GCCXML_PLUGIN_PATH "${GCCXML_PLUGIN_PATH}.${GCCXML_VERSION}")
ENDIF(UNIX)

INCLUDE_DIRECTORIES(BEFORE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libcpp/include
  ${GCC_SOURCE_DIR}/gcc/c-family
  ${GCC_BINARY_DIR}/gcc/c-family
  ${GCC_SOURCE_DIR}/gcc/libdecnumber
  ${GCC_SOURCE_DIR}/gcc/libdecnumber/bid
  ${GCC_BINARY_DIR}/gcc/libdecnumber
  )

#-----------------------------------------------------------------------------
# C++ Compiler Executable

IF(CMAKE_SYSTEM MATCHES "HP-UX")
  IF(CMAKE_C_COMPILER MATCHES "(^cc$|/cc$)")
    SET_SOURCE_FILES_PROPERTIES(../c-family/c-opts.c PROPERTIES COMPILE_FLAGS "-Wp,-H,20000")
  ENDIF(CMAKE_C_COMPILER MATCHES "(^cc$|/cc$)")
ENDIF(CMAKE_SYSTEM MATCHES "HP-UX")

#-----------------------------------------------------------------------------
# Special flags for some source files.

IF(CYGWIN)
  SET_SOURCE_FILES_PROPERTIES(../cppdefault.c PROPERTIES
      COMPILE_FLAGS "-DTOOL_INCLUDE_DIR=/usr/include")
ENDIF(CYGWIN)

#ADD_DEFINITIONS(
#  -DSTANDARD_STARTFILE_PREFIX=\"${unlibsubdir}\"
#  -DSTANDARD_EXEC_PREFIX=\"${GCC_LIBDIR}/gcc/\"
#  -DSTANDARD_LIBEXEC_PREFIX=\"${GCC_LIBDIR}/gcc/\"
#  -DDEFAULT_TARGET_VERSION=\"${GCC_VERSION}\"
#  -DDEFAULT_TARGET_MACHINE=\"${target}\"
#  -DSTANDARD_BINDIR_PREFIX=\"${GCC_PREFIX}/bin\"
#    -DTOOLDIR_BASE_PREFIX=\"${unlibsubdir}/${LIBSUBDIR_TO_PREFIX}\"
#    -DPREFIX=\"${GCC_PREFIX}\"
#  )

SET_SOURCE_FILES_PROPERTIES(../c-family/c-opts.c PROPERTIES 
  COMPILE_DEFINITIONS "${TARGET_SYSTEM_ROOT_DEFINE}")

SET_SOURCE_FILES_PROPERTIES(../c-family/c-cppbuiltin.c PROPERTIES 
  COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINES}")

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(${GCC_SOURCE_DIR}/gcc/cp/gccxml_prefix.h.in
               ${GCC_BINARY_DIR}/gcc/cp/gccxml_prefix.h @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# C Language-dependent file lists

SET(C_COMMON_SOURCES
  ../c-family/c-common.c    ../c-family/c-cppbuiltin.c ../c-family/c-dump.c
  ../c-family/c-format.c    ../c-family/c-gimplify.c    ../c-family/c-lex.c
  ../c-family/c-omp.c       ../c-family/c-opts.c        ../c-family/c-pch.c
  ../c-family/c-ppoutput.c  ../c-family/c-pragma.c      ../c-family/c-pretty-print.c
  ../c-family/c-semantics.c ../c-family/c-ada-spec.c
  )

SET(C_AND_OBJC_SOURCES
  ../attribs.c ../c-errors.c ../c-decl.c ../c-typeck.c
  ../c-convert.c ../c-aux-info.c ../c-objc-common.c
  ../c-parser.c ../tree-mudflap.c
  ${C_COMMON_SOURCES} ${c_target_srcs}
  )

#SET(C_SOURCES
#    ../c-lang.c ../c-family/stub-objc.c
#    ${C_AND_OBJC_SOURCES}
#    )
#ADD_LIBRARY(C_OBJS
#  OBJECT ${C_SOURCES}
#  )
#SET_TARGET_PROPERTIES(C_OBJS PROPERTIES
#  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET(CXX_C_SOURCES
    ../attribs.c ../incpath.c 
    ${C_COMMON_SOURCES} ${cxx_target_srcs}
    )

SET(CXX_AND_OBJCXX_SOURCES
    ./call.c   ./decl.c      ./expr.c   ./pt.c     ./typeck2.c
    ./class.c  ./decl2.c     ./error.c  ./lex.c    ./parser.c  
    ./ptree.c  ./rtti.c      ./typeck.c ./cvt.c    ./except.c
    ./friend.c ./init.c      ./method.c ./search.c ./semantics.c 
    ./tree.c   ./repo.c      ./dump.c   ./optimize.c ./mangle.c 
    ./cp-objcp-common.c    ./name-lookup.c   
    ./cxx-pretty-print.c   ./cp-gimplify.c 
    ../tree-mudflap.c 
    xml.c
    ${CXX_C_SOURCES}
    )

SET(CXX_SOURCES
    ./cp-lang.c ../c-family/stub-objc.c
    ${CXX_AND_OBJCXX_SOURCES})

ADD_LIBRARY(CXX_OBJS
  OBJECT ${CXX_SOURCES} 
  )

ADD_DEPENDENCIES(CXX_OBJS 
  backend
  ${GCC_BINARY_DIR}/gcc/gtype-desc.h
  ${GCC_BINARY_DIR}/gcc/insn-flags.h
  )

SET_SOURCE_FILES_PROPERTIES(xml.c PROPERTIES COMPILE_DEFINITIONS GCCXML_PLUGIN_PATH=\"${GCCXML_PLUGIN_PATH}\")

SET_TARGET_PROPERTIES(CXX_OBJS PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------------
# Rules to generate cc1-checksum.c and cc1plus-checksum.c

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/checksum-options
  "${CMAKE_LINKER} ${CMAKE_EXE_LINKERFLAGS} ${CMAKE_MODULE_LINKERFLAGS} ${LDFLAGS}")

SET(LIBDEPS ${CPPLIB} ${LIBIBERTY} ${LIBINTL_DEP} ${LIBICONV_DEP} ${LIBDECNUMBER})

ADD_EXECUTABLE(genchecksum ../genchecksum.c)
SET_TARGET_PROPERTIES(genchecksum PROPERTIES 
    LINK_FLAGS "${CMAKE_MODULE_LINKERFLAGS} ${LDFLAGS} ../libcommon.a")
TARGET_LINK_LIBRARIES(genchecksum ${BACKEND_LIBS} ${PLUGIN_LIBRARIES} ${LIBDEPS})
ADD_DEPENDENCIES(genchecksum checksum-options)

#ADD_CUSTOM_COMMAND(
#  OUTPUT cc1-checksum.c
#  COMMAND genchecksum ${C_SOURCES}
#          ${CMAKE_CURRENT_BINARY_DIR}/checksum-options > ${CMAKE_CURRENT_BINARY_DIR}/cc1-checksum.c
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#  DEPENDS genchecksum C_OBJS ${BACKEND_LIBS}
#)

ADD_CUSTOM_COMMAND(
  OUTPUT cc1plus-checksum.c
  COMMAND genchecksum ${CXX_SOURCES}
          ${CMAKE_CURRENT_BINARY_DIR}/checksum-options > ${CMAKE_CURRENT_BINARY_DIR}/cc1plus-checksum.c
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS genchecksum CXX_OBJS ${BACKEND_LIBS}
  )

#-----------------------------------------------------------------------------
# Rules to generate gccxml_cc1plus

ADD_EXECUTABLE(gccxml_cc1plus
  #../cppdefault.c
  #../prefix.c
  #../tree-inline.c
  ../main.c
  cc1plus-checksum.c
  $<TARGET_OBJECTS:CXX_OBJS>
  #./g++spec.c
  )

SET_TARGET_PROPERTIES(gccxml_cc1plus PROPERTIES COMPILE_DEFINITIONS "${DRIVER_DEFINES}")

MESSAGE("Linking gccxml_cc1plus with ${LIBS} ${BACKEND_LIBS} ${PLUGIN_LIBRARIES}")
TARGET_LINK_LIBRARIES(gccxml_cc1plus
  ${LIBS} ${BACKEND_LIBS} ${PLUGIN_LIBRARIES}
  common  common-target
  backend cpp decnumber iberty
  )

# Use the "-lx" option to disable map file generation on Borland.
# This avoids the "Fatal: Access violation.  Link terminated." error.
IF(BORLAND)
  SET_TARGET_PROPERTIES(gccxml_cc1plus PROPERTIES LINK_FLAGS "-lx")
ENDIF(BORLAND)
SET_TARGET_PROPERTIES(gccxml_cc1plus PROPERTIES LINK_FLAGS "${PLUGIN_FLAGS}")

# Install gccxml_cc1plus next to the gccxml executable.
INSTALL(TARGETS gccxml_cc1plus
  RUNTIME DESTINATION ${GCCXML_INSTALL_ROOT}bin
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_EXECUTABLE}
  )
