PROJECT(GCCXML)

# Set the version number.
SET(GCCXML_VERSION_MAJOR 0)
SET(GCCXML_VERSION_MINOR 5)
SET(GCCXML_VERSION_PATCH 0)
SET(GCCXML_VERSION "${GCCXML_VERSION_MAJOR}.${GCCXML_VERSION_MINOR}")
SET(GCCXML_VERSION_FULL "${GCCXML_VERSION}.${GCCXML_VERSION_PATCH}")

# Configure the build.
IF(EXECUTABLE_OUTPUT_PATH)
  SET(GCCXML_EXECUTABLE_DIR ${EXECUTABLE_OUTPUT_PATH})
ELSE(EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${GCCXML_BINARY_DIR})
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
  SET(GCCXML_ROOT ${GCCXML_SOURCE_DIR}/Support)
  SET(GCCXML_SHARE "/share/gccxml-${GCCXML_VERSION}")
  SET(GCCXML_DOC "/share/doc/gccxml-${GCCXML_VERSION}")
  INSTALL_TARGETS(${GCCXML_INSTALL_ROOT}/bin gccxml)
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}/man/man1 .1 gccxml)
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DOC} .txt Copyright)
  SUBDIRS (Support)
ELSE(UNIX)
  # Include file patch program for Windows.
  SET(GCCXML_ROOT ${GCCXML_EXECUTABLE_DIR})
  INCLUDE_DIRECTORIES(${GCCXML_SOURCE_DIR}/GXFront)
  ADD_EXECUTABLE(vcInstall VcInstall/vcInstall.cxx GXFront/gxSystemTools.cxx)
  UTILITY_SOURCE(GCCXML_VCINSTALL_EXE vcInstall .)
  MARK_AS_ADVANCED(GCCXML_VCINSTALL_EXE)
  
  # Use a dummy executable to enforce dependencies on generation
  # of patched compiler headers.  This is necessary to get around
  # dependency limitations on custom targets in CMake 1.6.  Later
  # cmakes do not have this limitation.
  ADD_EXECUTABLE(vcInstallPatch ${GCCXML_BINARY_DIR}/vcInstall_stamp.c)
  SET_SOURCE_FILES_PROPERTIES(${GCCXML_BINARY_DIR}/vcInstall_stamp.c
    PROPERTIES GENERATED 1)
  
  # Create the MSVC support directories.
  # This will just execute on every build because we don't know
  # what files and directories it may create.
  ADD_CUSTOM_COMMAND(TARGET vcInstallPatch
    SOURCE ${GCCXML_SOURCE_DIR}/VcInstall/vcPatch.readme.txt
    COMMAND ${GCCXML_VCINSTALL_EXE}
    ARGS ${GCCXML_SOURCE_DIR}/VcInstall ${GCCXML_ROOT}
         ${GCCXML_BINARY_DIR}/vcInstall_stamp.c
    OUTPUTS ${GCCXML_BINARY_DIR}/vcInstall_stamp.c
    DEPENDS ${GCCXML_VCINSTALL_EXE}
    ${GCCXML_SOURCE_DIR}/VcInstall/vc6Include.patch
    ${GCCXML_SOURCE_DIR}/VcInstall/vc7Include.patch
    ${GCCXML_SOURCE_DIR}/VcInstall/vc7PlatformSDK.patch
    ${GCCXML_SOURCE_DIR}/VcInstall/vc71Include.patch
    ${GCCXML_SOURCE_DIR}/VcInstall/vc71PlatformSDK.patch
    )
  
  ADD_DEPENDENCIES(gccxml vcInstallPatch)
  
  # If we are inside a project that is building gccxml_cc1plus for us,
  # add the dependency to build it first.
  IF(GCCXML_NATIVE_CC1PLUS)
    ADD_DEPENDENCIES(gccxml gccxml_cc1plus)
  ENDIF(GCCXML_NATIVE_CC1PLUS)
ENDIF(UNIX)

# Create a configuration file for use from the build directory.
CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/config_build.in
               ${GCCXML_EXECUTABLE_DIR}/gccxml_config)

IF(UNIX)
  # Create a configuration file for use from the install directory.
  CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/config_install.in
                 ${GCCXML_BINARY_DIR}/GXFront/InstallOnly/gccxml_config)
  IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.4)
    INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_SHARE} FILES
                  ${GCCXML_BINARY_DIR}/GXFront/InstallOnly/gccxml_config)
  ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.4)
ENDIF(UNIX)
