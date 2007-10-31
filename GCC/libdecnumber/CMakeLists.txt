INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libdecnumber/include
  ${GCC_BINARY_DIR}/libdecnumber
  )
SET(decnumber_SRCS
  decNumber.c decContext.c decUtility.c decimal32.c decimal64.c
  decimal128.c
  )
ADD_LIBRARY(decnumber ${decnumber_SRCS})
#TARGET_LINK_LIBRARIES(decnumber iberty)
