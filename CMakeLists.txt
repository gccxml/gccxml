CMAKE_MINIMUM_REQUIRED(VERSION 2.6.3 FATAL_ERROR)
PROJECT(gccxml)

# We are not using shared libraries.  Skip any RPATH.
SET(CMAKE_SKIP_RPATH 1)

# Tell GCC_XML that we are providing gccxml_cc1plus for it.
SET(GCCXML_NATIVE_CC1PLUS 1)

# Set GCCXML plugin install name

MESSAGE("gccxml_plugin_path ${CMAKE_INSTALL_DIR}/libgccxml${CMAKE_SHARED_LIBRARY_SUFFIX}")
SET(GCCXML_PLUGIN_PATH "${CMAKE_INSTALL_DIR}/libgccxml${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Place both gccxml_cc1plus and the GCC_XML executables next to each
# other.
SET(EXECUTABLE_OUTPUT_PATH "${gccxml_BINARY_DIR}/bin" CACHE PATH
  "Single output directory for all executables.")
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

# Tell GCC and GCC-XML where to install relative to the CMAKE_INSTALL_PREFIX.
SET(GCCXML_INSTALL_ROOT)
SET(LC_ALL C)
SET(LANGUAGE C)

# Installation component names for stand-alone GCC-XML.
SET(GCCXML_INSTALL_COMPONENT_NAME_DOCUMENTATION Documentation)
SET(GCCXML_INSTALL_COMPONENT_NAME_RUNTIME_EXECUTABLE RuntimeExecutable)
SET(GCCXML_INSTALL_COMPONENT_NAME_RUNTIME_LIBRARY RuntimeLibrary)

# Extra debug flags
SET(CMAKE_CXX_FLAGS_DEBUG "${MAKE_CXX_FLAGS_DEBUG} -g3 -gdwarf-3 -O0 -Wall -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "${MAKE_CXX_FLAGS_DEBUG} -fomit-frame-pointer -mtune=generic -Os")

# Dashboard support
INCLUDE(CTest)

# Test GCC_XML.
ENABLE_TESTING()
SET(GCCXML_ADD_TESTS 1)

# Build GCC and GCC_XML.
SUBDIRS(GCC GCC_XML)

# Use CPack to build a redistributable installer
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/gccxmlCPack.cmake")
