INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
#  ${GCC_SOURCE_DIR}/libgcc
  ${GCC_SOURCE_DIR}/libdecnumber
  ${GCC_SOURCE_DIR}/libdecnumber/include
  ${GCC_BINARY_DIR}/libdecnumber
  ${GCC_SOURCE_DIR}/gcc
  )

SET(decnumber_SRCS
  # Added for 4.7.2

  # From 4.2 branch
  decNumber.c decContext.c dpd/decimal32.c dpd/decimal64.c
  dpd/decimal128.c

  # Removed from 4.2 branch
  # decUtility.c 
  )

ADD_LIBRARY(decnumber ${decnumber_SRCS})
#TARGET_LINK_LIBRARIES(decnumber iberty)
