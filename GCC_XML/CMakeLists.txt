PROJECT(GCCXML)

ADD_EXECUTABLE(gccxml GXFront/gxSystemTools.cxx
                      GXFront/gxConfiguration.cxx
                      GXFront/gxFlagsParser.cxx
                      GXFront/gxFront.cxx)

INSTALL_TARGETS(/bin gccxml)

IF(UNIX)
  INSTALL_PROGRAMS(/share/GCC_XML gccxml_find_flags)
  SUBDIRS (GccInclude MproInclude)
ENDIF(UNIX)
