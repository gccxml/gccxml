PROJECT(GCC C)

# Do not place internal build-related executables in a parent project
# executable path.  Backup the real executable output path so it can
# be restored in gcc/cp for building gccxml_cc1plus.
SET(GCC_EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(EXECUTABLE_OUTPUT_PATH "${GCC_BINARY_DIR}/gcc")

# Do not inherit library building rules from the parent project.
SET(BUILD_SHARED_LIBS 0)
SET(LIBRARY_OUTPUT_PATH "${GCC_BINARY_DIR}/gcc")

IF(APPLE)
  IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -no-cpp-precomp")
  ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(APPLE)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ANSI_CFLAGS}")

ADD_DEFINITIONS(-DALMOST_STDC)

SUBDIRS(config_cmake libiberty gcc)
