PROJECT(GCCXMLComplete)

# Tell GCC_XML that we are providing gccxml_cc1plus for it.
SET(GCCXML_NATIVE_CC1PLUS 1)

# Place both gccxml_cc1plus and the GCC_XML executables next to each
# other.
SET(EXECUTABLE_OUTPUT_PATH "${GCCXMLComplete_BINARY_DIR}/bin" CACHE PATH
  "Single output directory for all executables.")

# Build GCC and GCC_XML.
SUBDIRS(GCC GCC_XML)
