# Install ./ header file wrappers.
FILE(GLOB HEADERS [a-z]*)
FOREACH(f ${HEADERS})
  IF("${f}" MATCHES "/std$")
  ELSE("${f}" MATCHES "/std$")
    INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/2.95 FILES "${f}")
  ENDIF("${f}" MATCHES "/std$")
ENDFOREACH(f)

# Install std/ header file wrappers.
FILE(GLOB HEADERS std/[a-z]*)
FOREACH(f ${HEADERS})
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/2.95/std FILES "${f}")
ENDFOREACH(f)
