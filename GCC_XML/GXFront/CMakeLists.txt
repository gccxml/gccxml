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

IF(NOT UNIX)
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
ENDIF(NOT UNIX)
