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
  GXFront/gxDocumentation.cxx
  GXFront/gxFlagsParser.cxx
  GXFront/gxFront.cxx
)

# Figure out what is being used to build GCC-XML.
SET(GCCXML_CXX_COMPILER "${CMAKE_CXX_COMPILER}")
SET(GCCXML_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

IF(CMAKE_BUILD_TOOL MATCHES "msdev")
  SET(GCCXML_VCINSTALL 1)
  SET(GCCXML_VCINSTALL_VC6 1)
  SET(GCCXML_CXX_COMPILER "msvc6")
ENDIF(CMAKE_BUILD_TOOL MATCHES "msdev")

IF(CMAKE_BUILD_TOOL MATCHES "devenv")
  SET(GCCXML_VCINSTALL 1)
  SET(GCCXML_VCINSTALL_VC7 1)
  SET(GCCXML_CXX_COMPILER "msvc7")
ENDIF(CMAKE_BUILD_TOOL MATCHES "devenv")


IF(UNIX)
  # Installation rules for UNIX.
  SET(GCCXML_ROOT ${GCCXML_SOURCE_DIR})
  INSTALL_TARGETS(/bin gccxml)
  INSTALL_PROGRAMS(/share/GCC_XML gccxml_find_flags)
  SUBDIRS (GccInclude MproInclude)
ENDIF(UNIX)

IF(GCCXML_VCINSTALL)
  # Include file patch program for Windows.
  SET(GCCXML_VCINSTALL_DIR "${GCCXML_SOURCE_DIR}/VcInstall")
  SET(GCCXML_ROOT ${GCCXML_EXECUTABLE_DIR})
  INCLUDE_DIRECTORIES(${GCCXML_SOURCE_DIR}/GXFront)
  ADD_EXECUTABLE(vcInstall VcInstall/vcInstall.cxx GXFront/gxSystemTools.cxx)
  UTILITY_SOURCE(GCCXML_VCINSTALL_EXE vcInstall .)

  IF(GCCXML_VCINSTALL_VC6)
    # Create patched MSVC6 header directory.
    ADD_CUSTOM_COMMAND(SOURCE ${GCCXML_VCINSTALL_DIR}/vc6Include.patch
                       COMMAND ${GCCXML_VCINSTALL_EXE}
                       ARGS ${GCCXML_VCINSTALL_DIR}/vc6Include.patch
                            ${GCCXML_EXECUTABLE_DIR}/Vc6/Include
                       TARGET gccxml
                       DEPENDS ${GCCXML_VCINSTALL_EXE}
                       OUTPUTS ${GCCXML_EXECUTABLE_DIR}/Vc6/Include/xstring)
  ENDIF(GCCXML_VCINSTALL_VC6)

  IF(GCCXML_VCINSTALL_VC7)
    # Create patched MSVC7 header directory.
    # Need to make directories now to prevent MSVC7 from creating them
    # with lower-case names.
    MAKE_DIRECTORY("${GCCXML_EXECUTABLE_DIR}/Vc7/Include")
    MAKE_DIRECTORY("${GCCXML_EXECUTABLE_DIR}/Vc7/PlatformSDK")
    ADD_CUSTOM_COMMAND(SOURCE ${GCCXML_VCINSTALL_DIR}/vc7Include.patch
                       COMMAND ${GCCXML_VCINSTALL_EXE}
                       ARGS ${GCCXML_VCINSTALL_DIR}/vc7Include.patch
                            ${GCCXML_EXECUTABLE_DIR}/Vc7/Include
                       TARGET gccxml
                       DEPENDS ${GCCXML_VCINSTALL_EXE}
                       OUTPUTS ${GCCXML_EXECUTABLE_DIR}/Vc7/Include/xstddef)
    ADD_CUSTOM_COMMAND(SOURCE ${GCCXML_VCINSTALL_DIR}/vc7PlatformSDK.patch
                       COMMAND ${GCCXML_VCINSTALL_EXE}
                       ARGS ${GCCXML_VCINSTALL_DIR}/vc7PlatformSDK.patch
                            ${GCCXML_EXECUTABLE_DIR}/Vc7/PlatformSDK
                       TARGET gccxml
                       DEPENDS ${GCCXML_VCINSTALL_EXE}
                       OUTPUTS ${GCCXML_EXECUTABLE_DIR}/Vc7/PlatformSDK/winnt.h)
  ENDIF(GCCXML_VCINSTALL_VC7)
ENDIF(GCCXML_VCINSTALL)

# Create a configuration file for use from the build directory.
CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/config_build.in
               ${GCCXML_EXECUTABLE_DIR}/config)

IF(UNIX)
  # Create a configuration file for use from the install directory.
  CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/GXFront/config_install.in
                 ${GCCXML_BINARY_DIR}/Install/Hide/config)
  INSTALL_FILES(/share/GCC_XML "" Install/Hide/config)
ENDIF(UNIX)
