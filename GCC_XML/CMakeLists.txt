PROJECT(GCCXML)

# Set the version number.
SET(GCCXML_VERSION_MAJOR 0)
SET(GCCXML_VERSION_MINOR 5)
SET(GCCXML_VERSION_PATCH development)
SET(GCCXML_VERSION_STRING
    "${GCCXML_VERSION_MAJOR}.${GCCXML_VERSION_MINOR}.${GCCXML_VERSION_PATCH}")

IF(CableSwig_SOURCE_DIR)
  ADD_DEFINITIONS(-DGCCXML_NATIVE_BUILD)
ENDIF(CableSwig_SOURCE_DIR)


# Configure the build.
IF(EXECUTABLE_OUTPUT_PATH)
  SET(GCCXML_EXECUTABLE_DIR ${EXECUTABLE_OUTPUT_PATH})
ELSE(EXECUTABLE_OUTPUT_PATH)
  SET(GCCXML_EXECUTABLE_DIR ${GCCXML_BINARY_DIR})
ENDIF(EXECUTABLE_OUTPUT_PATH)
CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/gxConfigure.h.in
               ${GCCXML_BINARY_DIR}/GXFront/gxConfigure.h)
INCLUDE_DIRECTORIES(${GCCXML_BINARY_DIR}/GXFront)

# Be sure to use ANSI compiler flags.
IF(CMAKE_ANSI_CXXFLAGS)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_ANSI_CXXFLAGS}")
ENDIF(CMAKE_ANSI_CXXFLAGS)

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

IF(CMAKE_GENERATOR MATCHES "Visual Studio 6")
  SET(GCCXML_CXX_COMPILER "msvc6")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 6")

IF(CMAKE_GENERATOR MATCHES "Visual Studio 7$")
  SET(GCCXML_CXX_COMPILER "msvc7")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 7$")

IF(CMAKE_GENERATOR MATCHES "Visual Studio 7 .NET 2003")
  SET(GCCXML_CXX_COMPILER "msvc71")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 7 .NET 2003")

IF(UNIX)
  # Installation rules for UNIX.
  SET(GCCXML_ROOT ${GCCXML_SOURCE_DIR})
  SET(GCCXML_SHARE "/share/gccxml-${GCCXML_VERSION_STRING}")
  SET(GCCXML_DOC "/share/doc/gccxml-${GCCXML_VERSION_STRING}")
  INSTALL_TARGETS(/bin gccxml)
  INSTALL_PROGRAMS(${GCCXML_SHARE} gccxml_find_flags)
  INSTALL_FILES(/man/man1 .1 gccxml)
  INSTALL_FILES(${GCCXML_DOC} .txt Copyright)
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
                 ${GCCXML_BINARY_DIR}/Install/config)
  IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.4)
    INSTALL_FILES(${GCCXML_SHARE} FILES ${GCCXML_BINARY_DIR}/Install/config)
  ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.4)
ENDIF(UNIX)
