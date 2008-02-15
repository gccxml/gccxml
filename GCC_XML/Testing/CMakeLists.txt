SET(EXE_DIR ".")
IF(EXECUTABLE_OUTPUT_PATH)
  SET(EXE_DIR "${EXECUTABLE_OUTPUT_PATH}")
ENDIF(EXECUTABLE_OUTPUT_PATH)

INCLUDE_DIRECTORIES(
  "${GCCXML_BINARY_DIR}/GXFront"
  "${GCCXML_SOURCE_DIR}/GXFront"
)

SET(gccxml_dashI_args "")
GET_DIRECTORY_PROPERTY(GX_INCS INCLUDE_DIRECTORIES)
FOREACH(inc ${GX_INCS})
  SET(gccxml_dashI_args ${gccxml_dashI_args} "-I${inc}")
ENDFOREACH(inc)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/TestFullPathInclude.cxx.in"
  "${CMAKE_CURRENT_BINARY_DIR}/TestFullPathInclude.cxx"
  @ONLY
)

SET(GX_TESTS
  "${GCCXML_SOURCE_DIR}/GXFront/gxConfiguration.cxx"
  "${GCCXML_SOURCE_DIR}/GXFront/gxConfiguration.h"
  "${GCCXML_SOURCE_DIR}/GXFront/gxDocumentation.cxx"
  "${GCCXML_SOURCE_DIR}/GXFront/gxDocumentation.h"
  "${GCCXML_SOURCE_DIR}/GXFront/gxFlagsParser.cxx"
  "${GCCXML_SOURCE_DIR}/GXFront/gxFlagsParser.h"
  "${GCCXML_SOURCE_DIR}/GXFront/gxFront.cxx"
  "${GCCXML_SOURCE_DIR}/GXFront/gxSystemTools.cxx"
  "${GCCXML_SOURCE_DIR}/GXFront/gxSystemTools.h"
  "${CMAKE_CURRENT_BINARY_DIR}/TestFullPathInclude.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/TestUsualInclude.cxx"
)

FOREACH(test ${GX_TESTS})
  GET_FILENAME_COMPONENT(name "${test}" NAME)
  GET_FILENAME_COMPONENT(name_we "${test}" NAME_WE)
  ADD_TEST(${name}
    ${EXE_DIR}/gccxml ${gccxml_dashI_args} ${test} -fxml=${name}.gcc.xml
  )
ENDFOREACH(test)
