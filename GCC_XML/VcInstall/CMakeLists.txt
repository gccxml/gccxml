ADD_EXECUTABLE(vcInstall vcInstall.cxx
  ${GCCXML_SOURCE_DIR}/GXFront/gxSystemTools.cxx)
TARGET_LINK_LIBRARIES(vcInstall gxsys)

IF(WIN32)
  IF(NOT UNIX)
    CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/VcInstall/gccxml_vcupdate.bat.in
                   ${GCCXML_BINARY_DIR}/VcInstall/gccxml_vcupdate.bat
                   @ONLY IMMEDIATE)
  ENDIF(NOT UNIX)
ENDIF(WIN32)
