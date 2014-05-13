# The GCC-XML forwarding exectuable.
ADD_EXECUTABLE(gccxml
  gxSystemTools.cxx
  gxConfiguration.cxx
  gxDocumentation.cxx
  gxFlagsParser.cxx
  gxFront.cxx
)

TARGET_LINK_LIBRARIES(gccxml gxsys)

ADD_DEPENDENCIES(gccxml vcInstallPatch)

# If we are inside a project that is building gccxml_cc1plus for us,
# add the dependency to build it first.
IF(GCCXML_NATIVE_CC1PLUS)
  ADD_DEPENDENCIES(gccxml gccxml_cc1plus)
ENDIF(GCCXML_NATIVE_CC1PLUS)

#-----------------------------------------------------------------------------
# Generate documentation.
MAKE_DIRECTORY(${GCCXML_BINARY_DIR}/doc)

ADD_CUSTOM_TARGET(documentation ALL DEPENDS
  ${GCCXML_BINARY_DIR}/doc/gccxml.1
  ${GCCXML_BINARY_DIR}/doc/gccxml.txt
  ${GCCXML_BINARY_DIR}/doc/gccxml.html
  ${GCCXML_BINARY_DIR}/doc/Copyright.txt)
ADD_DEPENDENCIES(documentation gccxml)

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_BINARY_DIR}/doc/gccxml.1
  COMMAND gccxml --man > ${GCCXML_BINARY_DIR}/doc/gccxml.1
  DEPENDS gccxml
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_BINARY_DIR}/doc/gccxml.txt
  COMMAND gccxml --help > ${GCCXML_BINARY_DIR}/doc/gccxml.txt
  DEPENDS gccxml
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_BINARY_DIR}/doc/gccxml.html
  COMMAND gccxml --help-html > ${GCCXML_BINARY_DIR}/doc/gccxml.html
  DEPENDS gccxml
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_BINARY_DIR}/doc/Copyright.txt
  COMMAND gccxml --copyright > ${GCCXML_BINARY_DIR}/doc/Copyright.txt
  DEPENDS gccxml
  )

INSTALL(TARGETS gccxml
  RUNTIME DESTINATION ${GCCXML_INSTALL_ROOT}bin
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_EXECUTABLE})
INSTALL(FILES ${GCCXML_BINARY_DIR}/doc/gccxml.1
  DESTINATION ${GCCXML_INSTALL_ROOT}${GCCXML_MAN_DIR}/man1
  ${GCCXML_INSTALL_COMPONENT_DOCUMENTATION})
INSTALL(FILES
  ${GCCXML_BINARY_DIR}/doc/gccxml.txt
  ${GCCXML_BINARY_DIR}/doc/gccxml.html
  ${GCCXML_BINARY_DIR}/doc/Copyright.txt
  DESTINATION ${GCCXML_INSTALL_ROOT}${GCCXML_DOC_DIR}
  ${GCCXML_INSTALL_COMPONENT_DOCUMENTATION})
