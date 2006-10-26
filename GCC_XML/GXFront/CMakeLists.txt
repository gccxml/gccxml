# The GCC-XML forwarding exectuable.
ADD_EXECUTABLE(gccxml
  gxSystemTools.cxx
  gxConfiguration.cxx
  gxDocumentation.cxx
  gxFlagsParser.cxx
  gxFront.cxx
)

TARGET_LINK_LIBRARIES(gccxml gxsys)

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

#-----------------------------------------------------------------------------
# Generate documentation.
SET(CMD_DIR ${GCCXML_EXECUTABLE_DIR}/${CMAKE_CFG_INTDIR})
IF(WIN32)
  SET(CMD_EXT ".exe")
ELSE(WIN32)
  SET(CMD_EXT "")
ENDIF(WIN32)
SET(CMD ${CMD_DIR}/gccxml${CMD_EXT})

MAKE_DIRECTORY(${GCCXML_EXECUTABLE_DIR}/doc)

ADD_CUSTOM_TARGET(documentation ALL DEPENDS
  ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.1
  ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.txt
  ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.html
  ${GCCXML_EXECUTABLE_DIR}/doc/Copyright.txt)
ADD_DEPENDENCIES(documentation gccxml)

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.1
  COMMAND ${CMD}
  ARGS --man > ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.1
  DEPENDS ${CMD}
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.txt
  COMMAND ${CMD}
  ARGS --help > ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.txt
  DEPENDS ${CMD}
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.html
  COMMAND ${CMD}
  ARGS --help-html > ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.html
  DEPENDS ${CMD}
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCCXML_EXECUTABLE_DIR}/doc/Copyright.txt
  COMMAND ${CMD}
  ARGS --copyright > ${GCCXML_EXECUTABLE_DIR}/doc/Copyright.txt
  DEPENDS ${CMD}
  )

IF(UNIX)
  INSTALL(TARGETS gccxml
    RUNTIME DESTINATION ${GCCXML_INSTALL_ROOT_CM24}/bin
    COMPONENT Development)
  INSTALL(FILES ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.1
    DESTINATION ${GCCXML_INSTALL_ROOT_CM24}${GCCXML_MAN_DIR}/man1
    COMPONENT Development)
  INSTALL(FILES
    ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.txt
    ${GCCXML_EXECUTABLE_DIR}/doc/gccxml.html
    ${GCCXML_EXECUTABLE_DIR}/doc/Copyright.txt
    DESTINATION ${GCCXML_INSTALL_ROOT_CM24}${GCCXML_DOC_DIR}
    COMPONENT Development)
ENDIF(UNIX)
