PROJECT(GCCXML)

# Configure the build.
IF(EXECUTABLE_OUTPUT_PATH)
  SET(GCCXML_EXECUTABLE_DIR ${EXECUTABLE_OUTPUT_PATH})
ELSE(EXECUTABLE_OUTPUT_PATH)
  SET(GCCXML_EXECUTABLE_DIR ${GCCXML_BINARY_DIR})
ENDIF(EXECUTABLE_OUTPUT_PATH)
CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/gxConfigure.h.in
               ${GCCXML_BINARY_DIR}/GXFront/gxConfigure.h)
INCLUDE_DIRECTORIES(${GCCXML_BINARY_DIR}/GXFront)

# The GCC-XML wrapper exectuable.
ADD_EXECUTABLE(gccxml
  GXFront/gxSystemTools.cxx
  GXFront/gxConfiguration.cxx
  GXFront/gxFlagsParser.cxx
  GXFront/gxFront.cxx
)

IF(UNIX)
  # Installation rules for UNIX.
  SET(GCCXML_ROOT ${GCCXML_SOURCE_DIR})
  INSTALL_TARGETS(/bin gccxml)
  INSTALL_PROGRAMS(/share/GCC_XML gccxml_find_flags)
  SUBDIRS (GccInclude MproInclude)
ELSE(UNIX)
  # Include file patch program for Windows (MSVC 6).
  SET(GCCXML_ROOT ${GCCXML_EXECUTABLE_DIR})
  INCLUDE_DIRECTORIES(${GCCXML_SOURCE_DIR}/GXFront)
  ADD_EXECUTABLE(vcInstall VcInstall/vcInstall.cxx GXFront/gxSystemTools.cxx)
  UTILITY_SOURCE(GCCXML_VCINSTALL_EXE vcInstall .)
  ADD_CUSTOM_COMMAND(SOURCE ${GCCXML_SOURCE_DIR}/VcInstall/vcInclude.patch
                     COMMAND ${GCCXML_VCINSTALL_EXE}
                     ARGS ${GCCXML_SOURCE_DIR}/VcInstall/vcInclude.patch
                          ${GCCXML_EXECUTABLE_DIR}/VcInclude
                     TARGET gccxml
                     OUTPUTS ${GCCXML_EXECUTABLE_DIR}/VcInclude/xstring)
ENDIF(UNIX)

# Create a configuration file for use from the build directory.
CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/config_build.in
               ${GCCXML_EXECUTABLE_DIR}/config)

IF(UNIX)
  # Create a configuration file for use from the install directory.
  CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/config_install.in
                 ${GCCXML_BINARY_DIR}/Install/Hide/config)
  INSTALL_FILES(/share/GCC_XML "" Install/Hide/config)
ENDIF(UNIX)
