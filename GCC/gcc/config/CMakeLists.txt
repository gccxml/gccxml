INCLUDE_DIRECTORIES(${GCC_BINARY_DIR}/gcc/config/${cpu_type})

ADD_CUSTOM_COMMAND(
    OUTPUT ${cpu_type}-builtin-types.inc
    COMMAND ${AWK_EXECUTABLE} 
            -f ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${cpu_type}-builtin-types.awk 
            ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${cpu_type}-builtin-types.def
            > ${GCC_BINARY_DIR}/gcc/${cpu_type}-builtin-types.inc
    DEPENDS ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${cpu_type}-builtin-types.def
            gencondmd
    )

ADD_CUSTOM_TARGET(generate_${cpu_type}
    DEPENDS ${cpu_type}-builtin-types.inc )

