INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/find_flags
  DESTINATION ${GCCXML_INSTALL_ROOT_CM24}${GCCXML_DATA_DIR}/GCC
  COMPONENT Development)

MACRO(GCCXML_GCC_SUPPORT_DIRECTORY_SIMPLE GCCVersion)
  # Install ./ header file wrappers.
  FILE(GLOB HEADERS [a-z]*)
  INSTALL(FILES  "${HEADERS}"
    DESTINATION
    ${GCCXML_INSTALL_ROOT_CM24}${GCCXML_DATA_DIR}/GCC/${GCCVersion}
    COMPONENT Development)
ENDMACRO(GCCXML_GCC_SUPPORT_DIRECTORY_SIMPLE GCCVersion)

MACRO(GCCXML_GCC_SUPPORT_DIRECTORY GCCVersion special)
  # Install ./ header file wrappers.
  FILE(GLOB HEADERS [a-z]*)
  FOREACH(f ${HEADERS})
    IF("${f}" MATCHES "/${special}$")
    ELSE("${f}" MATCHES "/${special}$")
      INSTALL(FILES  "${f}"
        DESTINATION
        ${GCCXML_INSTALL_ROOT_CM24}${GCCXML_DATA_DIR}/GCC/${GCCVersion}
        COMPONENT Development)
    ENDIF("${f}" MATCHES "/${special}$")
  ENDFOREACH(f)

  # Install ${special}/ header file wrappers.
  FILE(GLOB HEADERS ${special}/[a-z]*)
  FOREACH(f ${HEADERS})
    INSTALL(FILES  "${f}"
      DESTINATION
      ${GCCXML_INSTALL_ROOT_CM24}${GCCXML_DATA_DIR}/GCC/${GCCVersion}/${special}
      COMPONENT Development)
  ENDFOREACH(f)
ENDMACRO(GCCXML_GCC_SUPPORT_DIRECTORY)

SUBDIRS(2.95 2.96 3.0 3.1 3.2 3.3 3.4 4.0 4.1)
