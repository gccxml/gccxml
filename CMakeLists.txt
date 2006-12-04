PROJECT(GCCXMLComplete)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# We are not using shared libraries.  Skip any RPATH.
SET(CMAKE_SKIP_RPATH 1)

# Tell GCC_XML that we are providing gccxml_cc1plus for it.
SET(GCCXML_NATIVE_CC1PLUS 1)

# Place both gccxml_cc1plus and the GCC_XML executables next to each
# other.
SET(EXECUTABLE_OUTPUT_PATH "${GCCXMLComplete_BINARY_DIR}/bin" CACHE PATH
  "Single output directory for all executables.")
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

# Tell GCC and GCC-XML where to install relative to the CMAKE_INSTALL_PREFIX.
SET(GCCXML_INSTALL_ROOT)

# Build GCC and GCC_XML.
SUBDIRS(GCC GCC_XML)
