INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libcpp/include
  ${GCC_BINARY_DIR}/libcpp
  )
CONFIGURE_FILE(${GCC_SOURCE_DIR}/libcpp/localedir.h.in
               ${GCC_BINARY_DIR}/libcpp/localedir.h)
SET(cpp_SRCS
  charset.c directives.c errors.c expr.c files.c identifiers.c init.c
  lex.c line-map.c macro.c mkdeps.c pch.c symtab.c traditional.c
  )

# Use the compatibility dirent interface if necessary.
IF(NOT HAVE_DIRENT_H_COMPILED)
  LIST(APPEND cpp_SRCS dirent.c)
ENDIF(NOT HAVE_DIRENT_H_COMPILED)

ADD_LIBRARY(cpp ${cpp_SRCS})
TARGET_LINK_LIBRARIES(cpp iberty)
