# Script to re-create tm.h after insn-constants.h has been generated.
# Usage:
#   cmake -P gcc_make_constants_h.cmake  "${tm_defines}" "${tm_include_list}" "${tm_cpu_default}"

SET(tm_defines      ${CMAKE_ARGV3})
SET(tm_include_list ${CMAKE_ARGV4})
SET(tm_cpu_default  ${CMAKE_ARGV5})

#Add insn-constants.h to the list of includes
SET(tm_include_list insn-constants.h ${tm_include_list})  

# Restore GCCCONFIG_SOURCE_DIR variable
INCLUDE(${CMAKE_BINARY_DIR}/cache.cmake)
# Import the macro that creates the config header
INCLUDE(${GCCCONFIG_SOURCE_DIR}/gcc_make_config_h.cmake)

# Re-create file
GCC_MAKE_CONFIG_H(tm.h GCC_TM_H "${tm_defines}" "${tm_include_list}" "${target_cpu_default}")

# touch the insn-constants.h-full file.
FILE(WRITE "tm.h-full" "")
