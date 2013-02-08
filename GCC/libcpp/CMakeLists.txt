INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libcpp/include
  ${GCC_BINARY_DIR}/libcpp
  #${GCC_SOURCE_DIR}/intl
  )

IF(${UNIX})
  MESSAGE("setting data localed dirs")
  SET(DATAROOTDIR /usr/share)
  SET(LOCALEDIR "${DATAROOTDIR}/locale")
ENDIF(${UNIX})

CONFIGURE_FILE(${GCC_SOURCE_DIR}/libcpp/localedir.h.in
               localedir.h)
SET(cpp_SRCS
  charset.c directives.c directives-only.c errors.c 
  expr.c files.c identifiers.c init.c lex.c line-map.c
  macro.c mkdeps.c pch.c symtab.c traditional.c
  )
IF(HAS_KEEP_INLINE_FUNCTIONS)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fkeep-inline-functions")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fkeep-inline-functions")
ENDIF(HAS_KEEP_INLINE_FUNCTIONS)

# Use the compatibility dirent interface if necessary.
IF(NOT HAVE_DIRENT_H_COMPILED)
  LIST(APPEND cpp_SRCS dirent.c)
ENDIF(NOT HAVE_DIRENT_H_COMPILED)

ADD_LIBRARY(cpp localedir.h ${cpp_SRCS})
TARGET_LINK_LIBRARIES(cpp iberty)
#TARGET_LINK_LIBRARIES(cpp iberty intl)
