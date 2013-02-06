MACRO(GCC_MAKE_CONFIG_H output blocker defines headers cpu)
  SET(GCC_CONFIG_BLOCKER "${blocker}")
  SET(GCC_CONFIG_CONFIG_H)
  SET(GCC_CONFIG_CPU_DEFAULT)
  SET(GCC_CONFIG_DEFINES)
  SET(GCC_CONFIG_AUTO)
  SET(GCC_CONFIG_HEADERS)
  SET(GCC_CONFIG_TM_H)

  IF("${output}" MATCHES "^config\\.h$")
    SET(GCC_CONFIG_CONFIG_H "#ifdef GENERATOR_FILE
#error config.h is for the host, not build, machine.
#endif
")
  ENDIF("${output}" MATCHES "^config\\.h$")

  SET(GCC_CONFIG_CPU ${cpu})
  IF(GCC_CONFIG_CPU)
    SET(GCC_CONFIG_CPU_DEFAULT "#define TARGET_CPU_DEFAULT ${GCC_CONFIG_CPU}\n")
  ENDIF(GCC_CONFIG_CPU)
  FOREACH(d ${defines})
    STRING(REGEX REPLACE "=.*" "" name "${d}")
    STRING(REGEX REPLACE "=" " " define "${d}")
    SET(GCC_CONFIG_DEFINES
      "${GCC_CONFIG_DEFINES}#ifndef ${name}\n# define ${define}\n#endif\n")
  ENDFOREACH(d)

  SET(GCC_CONFIG_HEADERS_LIST ${headers})
  IF(GCC_CONFIG_HEADERS_LIST)
    LIST(GET GCC_CONFIG_HEADERS_LIST 0 FIRST_HEADER)
    IF("${FIRST_HEADER}" MATCHES "^auto")
      SET(GCC_CONFIG_AUTO "#include \"${FIRST_HEADER}\"\n")
      LIST(REMOVE_AT GCC_CONFIG_HEADERS_LIST 0)
    ENDIF("${FIRST_HEADER}" MATCHES "^auto")
    FOREACH(f ${GCC_CONFIG_HEADERS_LIST})
      SET(GCC_CONFIG_HEADERS "${GCC_CONFIG_HEADERS}# include \"${f}\"\n")
    ENDFOREACH(f)
  ENDIF(GCC_CONFIG_HEADERS_LIST)

  IF("${output}" MATCHES "^tm\\.h$")
    SET(GCC_CONFIG_TM_H "#if defined IN_GCC && !defined GENERATOR_FILE && !defined USED_FOR_TARGET
# include \"insn-flags.h\"
#endif
# include \"defaults.h\"
")
  ENDIF("${output}" MATCHES "^tm\\.h$")

  CONFIGURE_FILE(${GCCCONFIG_SOURCE_DIR}/gcc_config.h.in
                 ${GCC_BINARY_DIR}/gcc/${output} @ONLY IMMEDIATE)
ENDMACRO(GCC_MAKE_CONFIG_H)
