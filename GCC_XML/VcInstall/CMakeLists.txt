ADD_EXECUTABLE(vcInstall vcInstall.cxx
  ${GCCXML_SOURCE_DIR}/GXFront/gxSystemTools.cxx)
TARGET_LINK_LIBRARIES(vcInstall gxsys)

