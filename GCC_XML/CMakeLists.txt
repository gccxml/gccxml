PROJECT(GCCXML)

# Set the version number.
SET(GCCXML_VERSION_MAJOR 0)
SET(GCCXML_VERSION_MINOR 3)
SET(GCCXML_VERSION_PATCH development)

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
  GXFront/gxDocumentation.cxx
  GXFront/gxFlagsParser.cxx
  GXFront/gxFront.cxx
)

# Figure out what is being used to build GCC-XML.
SET(GCCXML_CXX_COMPILER "${CMAKE_CXX_COMPILER}")
SET(GCCXML_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

IF(CMAKE_BUILD_TOOL MATCHES "nmake")
  SET(GCCXML_CXX_COMPILER "cl")
ENDIF(CMAKE_BUILD_TOOL MATCHES "nmake")

IF(CMAKE_BUILD_TOOL MATCHES "msdev")
  SET(GCCXML_CXX_COMPILER "msvc6")
ENDIF(CMAKE_BUILD_TOOL MATCHES "msdev")

IF(CMAKE_BUILD_TOOL MATCHES "devenv")
  SET(GCCXML_CXX_COMPILER "msvc7")
ENDIF(CMAKE_BUILD_TOOL MATCHES "devenv")


IF(UNIX)
  # Installation rules for UNIX.
  SET(GCCXML_ROOT ${GCCXML_SOURCE_DIR})
  INSTALL_TARGETS(/bin gccxml)
  INSTALL_PROGRAMS(/share/GCC_XML gccxml_find_flags)
  SUBDIRS (GccInclude MproInclude)
ELSE(UNIX)
  # Include file patch program for Windows.
  SET(GCCXML_ROOT ${GCCXML_EXECUTABLE_DIR})
  INCLUDE_DIRECTORIES(${GCCXML_SOURCE_DIR}/GXFront)
  ADD_EXECUTABLE(vcInstall VcInstall/vcInstall.cxx GXFront/gxSystemTools.cxx)
  UTILITY_SOURCE(GCCXML_VCINSTALL_EXE vcInstall .)
  MARK_AS_ADVANCED(GCCXML_VCINSTALL_EXE)

  # Create the MSVC support directories.
  # This will just execute on every build because we don't know
  # what files and directories it may create.
  ADD_CUSTOM_TARGET(vcInstallPatch ALL ${GCCXML_VCINSTALL_EXE}
                    ${GCCXML_SOURCE_DIR}/VcInstall
                    ${GCCXML_ROOT})
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
