# Install ./ header file wrappers.
FILE(GLOB HEADERS [a-z]*)
INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/4.0
  FILES ${HEADERS})

# Install bits/ header file wrappers.
FILE(GLOB HEADERS bits/[a-z]*)
FOREACH(f ${HEADERS})
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/4.0/bits FILES "${f}")
ENDFOREACH(f)
