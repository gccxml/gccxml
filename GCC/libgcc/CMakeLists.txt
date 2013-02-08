INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libgcc/include
  ${GCC_BINARY_DIR}/libgcc
  )

SET(c_SRCS
  )


# Flags to pass to a recursive make.
SET(FLAGS_TO_PASS 
	"AR=${AR}"
	"AR_FLAGS=${AR_FLAGS}"
	"CC=${CC}"
	"CFLAGS=${CFLAGS}"
	"DESTDIR=${DESTDIR}"
	"EXTRA_OFILES=${EXTRA_OFILES}"
	"HDEFINES=${HDEFINES}"
	"INSTALL=${INSTALL}"
	"INSTALL_DATA=${INSTALL_DATA}"
	"INSTALL_PROGRAM=${INSTALL_PROGRAM}"
	"LDFLAGS=${LDFLAGS}"
	"LOADLIBES=${LOADLIBES}"
	"RANLIB=${RANLIB}"
	"SHELL=${SHELL}"
	"prefix=${prefix}"
	"exec_prefix=${exec_prefix}"
	"libdir=${libdir}"
	"libsubdir=${libsubdir}"
	"tooldir=${tooldir}"
  )

ADD_LIBRARY(libgcc ${c_SRCS})
TARGET_LINK_LIBRARIES(libgcc iberty)

