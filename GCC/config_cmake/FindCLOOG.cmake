# Try to find the GMP librairies
#  CLOOG_FOUND - system has GMP lib
#  CLOOG_INCLUDE_DIR - the GMP include directory
#  CLOOG_LIBRARIES - Libraries needed to use GMP

# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (CLOOG_INCLUDE_DIR AND CLOOG_LIBRARIES)
  # Already in cache, be silent
  set(CLOOG_FIND_QUIETLY TRUE)
endif (CLOOG_INCLUDE_DIR AND CLOOG_LIBRARIES)

find_path(GMP_INCLUDE_DIR NAMES cloog )
find_library(CLOOG_LIBRARIES NAMES cloog-isl)
MESSAGE(STATUS "CLOOG libs: " ${CLOOG_LIBRARIES} )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLOOG DEFAULT_MSG CLOOG_INCLUDE_DIR CLOOG_LIBRARIES)

mark_as_advanced(CLOOG_INCLUDE_DIR CLOOG_LIBRARIES)

