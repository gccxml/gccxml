# Create an executable to detect VS installations and copy and patch
# the headers.
ADD_EXECUTABLE(gccxml_vcinstall vcInstall.cxx
  ${GCCXML_SOURCE_DIR}/GXFront/gxSystemTools.cxx)
TARGET_LINK_LIBRARIES(gccxml_vcinstall gxsys)

# Install the gccxml_vcinstall executable.
INSTALL(TARGETS gccxml_vcinstall
  RUNTIME DESTINATION ${GCCXML_INSTALL_ROOT}bin
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_LIBRARY})

# Create the MSVC support directories.
# This will just execute on every build because we don't know
# what files and directories it may create.
GET_TARGET_PROPERTY(GCCXML_VCINSTALL_EXE gccxml_vcinstall LOCATION)
ADD_CUSTOM_COMMAND(
  OUTPUT ${GCCXML_BINARY_DIR}/vcInstall_stamp.c

  COMMAND ${GCCXML_VCINSTALL_EXE}
  ${GCCXML_SOURCE_DIR}/VcInstall ${GCCXML_ROOT}
  ${GCCXML_BINARY_DIR}/vcInstall_stamp.c

  DEPENDS gccxml_vcinstall
  ${GCCXML_SOURCE_DIR}/VcInstall/vc6Include.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc7Include.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc7PlatformSDK.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc71Include.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc71PlatformSDK.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc8Include.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc8PlatformSDK.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc8sp1Include.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc8sp1PlatformSDK.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc8ExpressInclude.patch
  ${GCCXML_SOURCE_DIR}/VcInstall/vc8ExpressPlatformSDK.patch
  )
ADD_CUSTOM_TARGET(vcInstallPatch DEPENDS
  ${GCCXML_BINARY_DIR}/vcInstall_stamp.c)

# Create a script to run the gccxml_vcinstall executable in an
# already-installed GCC-XML tree.
STRING(REGEX REPLACE "/" "\\\\" VC_DATA_DIR "${GCCXML_DATA_DIR}")
CONFIGURE_FILE(${GCCXML_SOURCE_DIR}/VcInstall/gccxml_vcupdate.bat.in
  ${GCCXML_BINARY_DIR}/VcInstall/gccxml_vcupdate.bat
  @ONLY IMMEDIATE)

# Install the patch files.
FILE(GLOB VCPATCH_FILES "vc*.patch")
INSTALL(
  FILES ${VCPATCH_FILES}
  DESTINATION ${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/VcInstall
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_LIBRARY})

# Install the update script helper files.
INSTALL(
  FILES
    ${GCCXML_SOURCE_DIR}/VcInstall/vcCat.readme.txt
    ${GCCXML_SOURCE_DIR}/VcInstall/vcPatch.readme.txt
    ${GCCXML_SOURCE_DIR}/VcInstall/vc_helper
  DESTINATION ${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/VcInstall
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_LIBRARY})
INSTALL(
  PROGRAMS ${GCCXML_SOURCE_DIR}/VcInstall/vcCat.exe
           ${GCCXML_SOURCE_DIR}/VcInstall/vcPatch.exe
  DESTINATION ${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/VcInstall
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_LIBRARY})

# Install the update script.
INSTALL(
  PROGRAMS ${GCCXML_BINARY_DIR}/VcInstall/gccxml_vcupdate.bat
  DESTINATION ${GCCXML_INSTALL_ROOT}bin
  ${GCCXML_INSTALL_COMPONENT_RUNTIME_EXECUTABLE})

# Run the update script during installation.
#IF(NOT GCCXML_NO_VCINSTALL_UPDATE)
#  INSTALL(CODE "
#EXECUTE_PROCESS(COMMAND \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${GCCXML_INSTALL_ROOT}bin/gccxml_vcupdate.bat\")
#")
#ENDIF(NOT GCCXML_NO_VCINSTALL_UPDATE)
