# The GCC-XML forwarding exectuable.
ADD_EXECUTABLE(gccxml
  gxSystemTools.cxx
  gxConfiguration.cxx
  gxDocumentation.cxx
  gxFlagsParser.cxx
  gxFront.cxx
)

IF(UNIX)
  INSTALL_TARGETS(${GCCXML_INSTALL_ROOT}/bin gccxml)
ENDIF(UNIX)

# If we are inside a project that is building gccxml_cc1plus for us,
# add the dependency to build it first.
IF(GCCXML_NATIVE_CC1PLUS)
  ADD_DEPENDENCIES(gccxml gccxml_cc1plus)
ENDIF(GCCXML_NATIVE_CC1PLUS)
