# Install ./ header file wrappers.
FILE(GLOB HEADERS [a-z]*)
FOREACH(f ${HEADERS})
  IF("${f}" MATCHES "/bits$")
  ELSE("${f}" MATCHES "/bits$")
    INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/4.0 FILES "${f}")
  ENDIF("${f}" MATCHES "/bits$")
ENDFOREACH(f)

# Install bits/ header file wrappers.
FILE(GLOB HEADERS bits/[a-z]*)
FOREACH(f ${HEADERS})
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/4.0/bits FILES "${f}")
ENDFOREACH(f)
