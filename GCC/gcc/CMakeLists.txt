#-----------------------------------------------------------------------------
INCLUDE("${GCCCONFIG_SOURCE_DIR}/gcc_make_config_h.cmake")

INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/gcc
  ${GCC_SOURCE_DIR}/gcc/config
  ${GCC_BINARY_DIR}/gcc)

ADD_DEFINITIONS(-DIN_GCC -DHAVE_CONFIG_H)

INCLUDE_REGULAR_EXPRESSION("^([^ri]|i[^n]|in[^s]|ins[^n]|insn[^-]|r[^t]|rt[^l]|rtl[^.])")

SUBDIRS(cp)

IF(WIN32)
  SET(GCC_EXE_EXT ".exe")
ELSE(WIN32)
  SET(GCC_EXE_EXT)
ENDIF(WIN32)
IF(EXECUTABLE_OUTPUT_PATH)
  SET(GCC_GEN_DIR ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
ELSE(EXECUTABLE_OUTPUT_PATH)
  SET(GCC_GEN_DIR ${GCC_BINARY_DIR}/gcc/${CMAKE_CFG_INTDIR})
ENDIF(EXECUTABLE_OUTPUT_PATH)

#-----------------------------------------------------------------------------
# Load the platform configuration.
IF(EXISTS "${GCCCONFIG_BINARY_DIR}/gcc_platform.cmake")
  INCLUDE("${GCCCONFIG_BINARY_DIR}/gcc_platform.cmake")
ELSE(EXISTS "${GCCCONFIG_BINARY_DIR}/gcc_platform.cmake")
  MESSAGE(FATAL_ERROR "Cannot find gcc_platform.cmake.")
ENDIF(EXISTS "${GCCCONFIG_BINARY_DIR}/gcc_platform.cmake")

# Default the target-machine variables that were not explicitly set.
IF(NOT tm_file)
  SET(tm_file ${cpu_type}/${cpu_type}.h)
ENDIF(NOT tm_file)
IF(NOT md_file)
  SET(md_file ${cpu_type}/${cpu_type}.md)
ENDIF(NOT md_file)
IF(NOT out_file)
  SET(out_file ${cpu_type}/${cpu_type}.c)
ENDIF(NOT out_file)

SET(tm_file ${tm_file} defaults.h)
SET(host_xm_file auto-host.h ansidecl.h ${host_xm_file} ${tm_file})
SET(build_xm_file auto-host.h ansidecl.h ${build_xm_file} ${tm_file})
SET(xm_file ansidecl.h ${xm_file} ${tm_file})

# Find the platform-specific sources.
MACRO(FIND_EXTRA_SRCS objs var)
  STRING(REGEX REPLACE "\\.o" ".c" EXTRA_SRCS "${objs}")
  FOREACH(f ${EXTRA_SRCS})
    IF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
      SET(${var} ${${var}} ${GCC_SOURCE_DIR}/gcc/config/${f})
    ELSE(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
      IF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${f})
        SET(${var} ${${var}} ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${f})
      ELSE(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${f})
        MESSAGE(SEND_ERROR "Cannot find platform-specific source ${f}.")
      ENDIF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${f})
    ENDIF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
  ENDFOREACH(f)
ENDMACRO(FIND_EXTRA_SRCS)
FIND_EXTRA_SRCS("${extra_objs}" extra_srcs)
FIND_EXTRA_SRCS("${c_target_objs}" c_target_srcs)
FIND_EXTRA_SRCS("${cxx_target_objs}" cxx_target_srcs)

#-----------------------------------------------------------------------------
# Generate configuration header files.

# Generate hconfig.h.
SET(typedefs "#ifdef IN_GCC\nstruct rtx_def;\ntypedef struct rtx_def *rtx;\nstruct rtvec_def;\ntypedef struct rtvec_def *rtvec;\nunion tree_node;\ntypedef union tree_node *tree;\n#endif\n#define GTY(x)\n")
GCC_MAKE_CONFIG_H(hconfig.h GCC_HCONFIG_H "${typedefs}" "${tm_defines}"
  "${build_xm_defines}" "${build_xm_file}" "" "" "${target_cpu_default}")

# Build corresponding file list.
SET(tm_file_list)
FOREACH(f ${tm_file})
  SET(tm_file_list ${tm_file_list} ${GCC_SOURCE_DIR}/gcc/config/${f})
ENDFOREACH(f)
SET(tm_file_list ${tm_file_list} ${GCC_SOURCE_DIR}/gcc/defaults.h)

# Generate tm_p.h.
GCC_MAKE_CONFIG_H(tm_p.h GCC_TM_P_H "" "" "" "${tm_p_file}" "tm-preds.h" "" "")

# Generate config.h.
GCC_MAKE_CONFIG_H(config.h GCC_CONFIG_H "${typedefs}" "${tm_defines}"
  "${host_xm_defines}" "${host_xm_file}" ""
  "#ifndef GENERATOR_FILE\n# include \"insn-constants.h\"\n# include \"insn-flags.h\"\n#endif\n"
  "${target_cpu_default}")

# Build corresponding file list.
SET(host_xm_file_list)
FOREACH(f ${host_xm_file})
  IF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
    SET(host_xm_file_list ${host_xm_file_list} ${GCC_SOURCE_DIR}/gcc/config/${f})
  ELSE(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
    IF(EXISTS ${GCC_BINARY_DIR}/gcc/${f})
      SET(host_xm_file_list ${host_xm_file_list} ${GCC_BINARY_DIR}/gcc/${f})
    ENDIF(EXISTS ${GCC_BINARY_DIR}/gcc/${f})
  ENDIF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
ENDFOREACH(f)
SET(host_xm_file_list ${GCC_SOURCE_DIR}/include/ansidecl.h ${host_xm_file_list})

# Generate tconfig.h.
GCC_MAKE_CONFIG_H(tconfig.h GCC_TCONFIG_H "${typedefs}" "${tm_defines}"
  "${xm_defines}" "${xm_file}" "" "" "")

#-----------------------------------------------------------------------------
# Rules to generate genrtl.h and genrtl.c.

SET(gengenrtl_SRCS gengenrtl.c)
SET(GCC_gengenrtl_EXE "${GCC_GEN_DIR}/gengenrtl${GCC_EXE_EXT}")
ADD_EXECUTABLE(gengenrtl ${gengenrtl_SRCS})
TARGET_LINK_LIBRARIES(gengenrtl iberty)
SET_SOURCE_FILES_PROPERTIES(${gengenrtl_SRCS} PROPERTIES
  COMPILE_FLAGS -DGENERATOR_FILE)

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/genrtl.c
  COMMAND ${GCC_gengenrtl_EXE}
  ARGS > "${GCC_BINARY_DIR}/gcc/genrtl.c"
  DEPENDS ${GCC_gengenrtl_EXE}
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/genrtl.h
  COMMAND ${GCC_gengenrtl_EXE}
  ARGS -h > "${GCC_BINARY_DIR}/gcc/genrtl.h"
  DEPENDS ${GCC_gengenrtl_EXE}
  )

#-----------------------------------------------------------------------------
# Generate gtyp-gen.h.
SET(GTFILES_C
  c-lang.c c-parse.in c-tree.h c-decl.c c-common.c c-common.h c-pragma.c
  c-objc-common.c
  )

SET(GTFILES_CXX
  cp/mangle.c cp/cp-tree.h cp/decl.h cp/lex.h cp/call.c cp/decl.c
  cp/decl2.c cp/parse.y cp/pt.c cp/repo.c cp/spew.c cp/tree.c c-common.c
  c-common.h c-pragma.c
  )

SET(GTFILES
  location.h bitmap.h function.h rtl.h optabs.h tree.h libfuncs.h
  hashtable.h real.h varray.h ssa.h insn-addr.h cselib.h c-common.h c-tree.h
  basic-block.h alias.c bitmap.c cselib.c dwarf2out.c emit-rtl.c
  except.c explow.c expr.c fold-const.c function.c gcse.c integrate.c
  lists.c optabs.c profile.c ra-build.c regclass.c reg-stack.c
  sdbout.c stmt.c stor-layout.c tree.c varasm.c
  )

SET(GTYP_GEN_LANG_FILES)
SET(GTYP_GEN_LANGS_FOR_LANG_FILES)
SET(GTYP_GEN_ALL_FILES)
FOREACH(f
    ${GCC_BINARY_DIR}/gcc/config.h
    ${host_xm_file_list}
    ${GCC_SOURCE_DIR}/include/hashtab.h
    ${GCC_SOURCE_DIR}/gcc/config/${out_file}
    )
  SET(GTYP_GEN_ALL_FILES "${GTYP_GEN_ALL_FILES}\"${f}\",\n")
ENDFOREACH(f)
FOREACH(f ${GTFILES})
  SET(GTYP_GEN_ALL_FILES
    "${GTYP_GEN_ALL_FILES}\"${GCC_SOURCE_DIR}/gcc/${f}\",\n")
ENDFOREACH(f)
SET(GTYP_GEN_ALL_FILES "${GTYP_GEN_ALL_FILES}\"${GCC_SOURCE_DIR}/gcc/config/${out_file}\",\n")
FOREACH(f ${GTFILES_CXX})
  SET(GTYP_GEN_ALL_FILES
    "${GTYP_GEN_ALL_FILES}\"${GCC_SOURCE_DIR}/gcc/${f}\",\n")
  SET(GTYP_GEN_LANG_FILES
    "${GTYP_GEN_LANG_FILES}\"${GCC_SOURCE_DIR}/gcc/${f}\",\n")
  SET(GTYP_GEN_LANGS_FOR_LANG_FILES
    "${GTYP_GEN_LANGS_FOR_LANG_FILES}\"cp\",\n")
ENDFOREACH(f)
FOREACH(f ${GTFILES_C})
  SET(GTYP_GEN_ALL_FILES
    "${GTYP_GEN_ALL_FILES}\"${GCC_SOURCE_DIR}/gcc/${f}\",\n")
  SET(GTYP_GEN_LANG_FILES
    "${GTYP_GEN_LANG_FILES}\"${GCC_SOURCE_DIR}/gcc/${f}\",\n")
  SET(GTYP_GEN_LANGS_FOR_LANG_FILES
    "${GTYP_GEN_LANGS_FOR_LANG_FILES}\"c\",\n")
ENDFOREACH(f)
FOREACH(f ${extra_srcs})
  SET(GTYP_GEN_ALL_FILES "${GTYP_GEN_ALL_FILES}\"${f}\",\n")
ENDFOREACH(f)

CONFIGURE_FILE(${GCCCONFIG_SOURCE_DIR}/gtyp-gen.h.in
               ${GCC_BINARY_DIR}/gcc/gtyp-gen.h @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Rules to generate gtype-desc.h and gtype-desc.c.

SET(gengtype_SRCS gengtype.c gengtype-lex.c gengtype-yacc.c)
ADD_EXECUTABLE(gengtype ${gengtype_SRCS})
SET(GCC_gengtype_EXE "${GCC_GEN_DIR}/gengtype${GCC_EXE_EXT}")
TARGET_LINK_LIBRARIES(gengtype iberty)
SET_SOURCE_FILES_PROPERTIES(${gengtype_SRCS} PROPERTIES
  COMPILE_FLAGS "-DGENERATOR_FILE -DYY_NO_UNISTD_H")

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/gtype-desc.h
  COMMAND echo
  DEPENDS ${GCC_BINARY_DIR}/gcc/gtype-desc.c
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/gtype-desc.c
  COMMAND ${GCC_gengtype_EXE}
  DEPENDS ${GCC_gengtype_EXE}
  )

#-----------------------------------------------------------------------------
# Rules to generate tm-preds.h.

SET(genpreds_SRCS genpreds.c)
ADD_EXECUTABLE(genpreds ${genpreds_SRCS})
SET(GCC_genpreds_EXE "${GCC_GEN_DIR}/genpreds${GCC_EXE_EXT}")
TARGET_LINK_LIBRARIES(genpreds iberty)
SET_SOURCE_FILES_PROPERTIES(${genpreds_SRCS} PROPERTIES
  COMPILE_FLAGS -DGENERATOR_FILE)

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/tm-preds.h
  COMMAND ${GCC_genpreds_EXE}
  ARGS > "${GCC_BINARY_DIR}/gcc/tm-preds.h"
  DEPENDS ${GCC_genpreds_EXE}
  )

#-----------------------------------------------------------------------------
# Rules to generate tree-check.h.

SET(gencheck_SRCS gencheck.c)
ADD_EXECUTABLE(gencheck ${gencheck_SRCS})
SET(GCC_gencheck_EXE "${GCC_GEN_DIR}/gencheck${GCC_EXE_EXT}")
TARGET_LINK_LIBRARIES(gencheck iberty)
SET_SOURCE_FILES_PROPERTIES(${gencheck_SRCS} PROPERTIES
  COMPILE_FLAGS -DGENERATOR_FILE)

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/tree-check.h
  COMMAND ${GCC_gencheck_EXE}
  ARGS > "${GCC_BINARY_DIR}/gcc/tree-check.h"
  DEPENDS ${GCC_gencheck_EXE}
  )

#-----------------------------------------------------------------------------
# Sources used in several generator executables.

SET(GEN_SRCS rtl.c read-rtl.c bitmap.c ggc-none.c gensupport.c
  dummy-conditions.c errors.c)
SET(GEN_OBJDEPS
  ${GCC_BINARY_DIR}/gcc/genrtl.h
  ${GCC_BINARY_DIR}/gcc/gtype-desc.h)
SET_SOURCE_FILES_PROPERTIES(${GEN_SRCS} PROPERTIES
  COMPILE_FLAGS -DGENERATOR_FILE
  OBJECT_DEPENDS "${GEN_OBJDEPS}")

#-----------------------------------------------------------------------------
# Rules to generate insn-constants.h and insn-flags.h.

FOREACH(tgt constants flags)
  SET(gen${tgt}_SRCS gen${tgt}.c)
  ADD_EXECUTABLE(gen${tgt} ${gen${tgt}_SRCS} ${GEN_SRCS})
  SET(GCC_gen${tgt}_EXE "${GCC_GEN_DIR}/gen${tgt}${GCC_EXE_EXT}")
  TARGET_LINK_LIBRARIES(gen${tgt} iberty)
  SET_SOURCE_FILES_PROPERTIES(${gen${tgt}_SRCS} PROPERTIES
    COMPILE_FLAGS -DGENERATOR_FILE
    OBJECT_DEPENDS "${GEN_OBJDEPS}")

  ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/insn-${tgt}.h
    COMMAND ${GCC_gen${tgt}_EXE}
    ARGS "${GCC_SOURCE_DIR}/gcc/config/${md_file}"
    > "${GCC_BINARY_DIR}/gcc/insn-${tgt}.h"
    DEPENDS ${GCC_gen${tgt}_EXE}
    ${GCC_SOURCE_DIR}/gcc/config/${md_file}
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# Another source used in several generators.

SET(printrtl1_OBJDEPS ${GCC_BINARY_DIR}/gcc/insn-constants.h
  ${GCC_BINARY_DIR}/gcc/insn-flags.h ${GCC_BINARY_DIR}/gcc/tree-check.h)
SET_SOURCE_FILES_PROPERTIES(print-rtl1.c PROPERTIES
  COMPILE_FLAGS -DGENERATOR_FILE
  OBJECT_DEPENDS "${printrtl1_OBJDEPS}")

#-----------------------------------------------------------------------------
# Rules to generate insn-config.h, insn-codes.h, and insn-attr.h.

FOREACH(tgt config codes attr)
  SET(gen${tgt}_SRCS gen${tgt}.c)
  ADD_EXECUTABLE(gen${tgt} ${gen${tgt}_SRCS} ${GEN_SRCS} print-rtl1.c)
  SET(GCC_gen${tgt}_EXE "${GCC_GEN_DIR}/gen${tgt}${GCC_EXE_EXT}")
  TARGET_LINK_LIBRARIES(gen${tgt} iberty)
  SET(gen${tgt}_OBJDEPS ${GCC_BINARY_DIR}/gcc/insn-constants.h
    ${GCC_BINARY_DIR}/gcc/insn-flags.h)
  SET_SOURCE_FILES_PROPERTIES(${gen${tgt}_SRCS} PROPERTIES
    COMPILE_FLAGS -DGENERATOR_FILE
    OBJECT_DEPENDS "${gen${tgt}_OBJDEPS}")

  ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/insn-${tgt}.h
    COMMAND ${GCC_gen${tgt}_EXE}
    ARGS "${GCC_SOURCE_DIR}/gcc/config/${md_file}"
    > "${GCC_BINARY_DIR}/gcc/insn-${tgt}.h"
    DEPENDS ${GCC_gen${tgt}_EXE}
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# Rules to generate insn-conditions.c insn-emit.c, insn-extract.c,
# insn-opinit.c, insn-peep.c, insn-output.c, insn-recog.c, and
# insn-attrtab.c.

SET(genattrtab_EXTRA_SRCS varray.c genautomata.c)
SET(genattrtab_EXTRA_LIBS m)
IF(WIN32)
  IF(NOT MINGW)
    SET(genattrtab_EXTRA_LIBS)
  ENDIF(NOT MINGW)
ENDIF(WIN32)
FOREACH(tgt emit extract opinit peep output recog attrtab)
  SET(gen${tgt}_PRINTRTL_SRC print-rtl1.c)
ENDFOREACH(tgt)
FOREACH(tgt emit extract opinit peep output recog attrtab)
  SET(gen${tgt}_SRCS gen${tgt}.c ${gen${tgt}_EXTRA_SRCS})
  ADD_EXECUTABLE(gen${tgt} ${gen${tgt}_SRCS} ${GEN_SRCS}
    ${gen${tgt}_PRINTRTL_SRC})
  SET(GCC_gen${tgt}_EXE "${GCC_GEN_DIR}/gen${tgt}${GCC_EXE_EXT}")
  TARGET_LINK_LIBRARIES(gen${tgt} iberty ${gen${tgt}_EXTRA_LIBS})
  SET_SOURCE_FILES_PROPERTIES(${gen${tgt}_SRCS} PROPERTIES
    COMPILE_FLAGS -DGENERATOR_FILE
    OBJECT_DEPENDS ${GCC_BINARY_DIR}/gcc/insn-config.h)

  ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/insn-${tgt}.c
    COMMAND ${GCC_gen${tgt}_EXE}
    ARGS "${GCC_SOURCE_DIR}/gcc/config/${md_file}"
    > "${GCC_BINARY_DIR}/gcc/insn-${tgt}.c"
    DEPENDS ${GCC_gen${tgt}_EXE}
    ${GCC_SOURCE_DIR}/gcc/config/${md_file}
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# C preprocessor library (cpp).

SET(cpp_SRCS cpplib.c cpplex.c cppmacro.c cppexp.c cppfiles.c
  cpptrad.c cpphash.c cpperror.c cppinit.c cppdefault.c cppmain.c
  hashtable.c line-map.c mkdeps.c prefix.c mbchar.c)
SET_SOURCE_FILES_PROPERTIES(prefix.c
  PROPERTIES COMPILE_FLAGS "-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
IF(CYGWIN)
  SET_SOURCE_FILES_PROPERTIES(cppdefault.c PROPERTIES
    COMPILE_FLAGS "-DTOOL_INCLUDE_DIR=/usr/include")
ENDIF(CYGWIN)
ADD_LIBRARY(cpp ${cpp_SRCS})

#-----------------------------------------------------------------------------
# Compiler backend library.

# Standard backend sources from the source tree.
SET(backend_STANDARD_SRCS
  ${GCC_SOURCE_DIR}/gcc/config/${out_file}
  alias.c bb-reorder.c builtins.c caller-save.c calls.c cfg.c cfganal.c
  cfgbuild.c cfgcleanup.c cfglayout.c cfgloop.c cfgrtl.c combine.c
  conflict.c convert.c cse.c cselib.c dbxout.c debug.c df.c diagnostic.c
  doloop.c dominance.c dwarf2asm.c dwarf2out.c dwarfout.c emit-rtl.c
  except.c explow.c expmed.c expr.c final.c flow.c fold-const.c
  function.c gcse.c genrtl.c ggc-common.c global.c graph.c haifa-sched.c
  hashtable.c hooks.c ifcvt.c integrate.c intl.c jump.c langhooks.c
  lcm.c lists.c local-alloc.c loop.c mbchar.c optabs.c params.c
  predict.c print-tree.c profile.c ra.c ra-build.c ra-colorize.c
  ra-debug.c ra-rewrite.c real.c recog.c reg-stack.c regclass.c
  regmove.c regrename.c reload.c reload1.c reorg.c resource.c rtlanal.c
  rtl-error.c sbitmap.c sched-deps.c sched-ebb.c sched-rgn.c sched-vis.c
  sdbout.c sibcall.c simplify-rtx.c ssa.c ssa-ccp.c ssa-dce.c stmt.c
  stor-layout.c stringpool.c timevar.c toplev.c tracer.c tree.c
  tree-dump.c tree-inline.c unroll.c varasm.c varray.c version.c
  vmsdbgout.c xcoffout.c et-forest.c ggc-page.c main.c
  ${extra_srcs}
  )

IF(NOT HAVE_DIRENT_H_COMPILED)
  SET(backend_STANDARD_SRCS ${backend_STANDARD_SRCS} dirent.c)
ENDIF(NOT HAVE_DIRENT_H_COMPILED)

# Generated sources for the backend.
SET(backend_GENERATED_SRCS
  ${GCC_BINARY_DIR}/gcc/gtype-desc.c
  ${GCC_BINARY_DIR}/gcc/insn-attrtab.c
  ${GCC_BINARY_DIR}/gcc/insn-emit.c
  ${GCC_BINARY_DIR}/gcc/insn-extract.c
  ${GCC_BINARY_DIR}/gcc/insn-opinit.c
  ${GCC_BINARY_DIR}/gcc/insn-output.c
  ${GCC_BINARY_DIR}/gcc/insn-peep.c
  ${GCC_BINARY_DIR}/gcc/insn-recog.c
)

# Sources shared with the generator executables.
SET(backend_GEN_SRCS rtl.c bitmap.c print-rtl.c)

# Add the library.
ADD_LIBRARY(backend
  ${backend_STANDARD_SRCS}
  ${backend_GENERATED_SRCS}
  ${backend_GEN_SRCS}
)

# Add dependencies on generated headers.
SET(backend_OBJDEPS
  ${GCC_BINARY_DIR}/gcc/insn-config.h
  ${GCC_BINARY_DIR}/gcc/insn-attr.h
  ${GCC_BINARY_DIR}/gcc/insn-codes.h
  ${GCC_BINARY_DIR}/gcc/tree-check.h
  ${GCC_BINARY_DIR}/gcc/tm-preds.h
  )
SET_SOURCE_FILES_PROPERTIES(${backend_STANDARD_SRCS} ${backend_GENERATED_SRCS}
  PROPERTIES OBJECT_DEPENDS "${backend_OBJDEPS}")
SET_SOURCE_FILES_PROPERTIES(toplev.c
  PROPERTIES COMPILE_FLAGS "-DTARGET_NAME=${target}")

#-----------------------------------------------------------------------------
# C Compiler Executable.  This is not needed by GCC-XML, so we do not
# build it by default.

IF(GCC_BUILD_C)
  IF(CMAKE_SYSTEM MATCHES "HP-UX")
    IF(CMAKE_C_COMPILER MATCHES "(^cc$|/cc$)")
      SET_SOURCE_FILES_PROPERTIES(c-opts.c PROPERTIES COMPILE_FLAGS "-Wp,-H,20000")
    ENDIF(CMAKE_C_COMPILER MATCHES "(^cc$|/cc$)")
  ENDIF(CMAKE_SYSTEM MATCHES "HP-UX")

  ADD_EXECUTABLE(cc1
    c-parse.c c-lang.c c-pretty-print.c attribs.c c-errors.c c-lex.c
    c-pragma.c c-decl.c c-typeck.c c-convert.c c-aux-info.c c-common.c
    c-opts.c c-format.c c-semantics.c c-objc-common.c c-dump.c
    ${c_target_srcs}
    )

  TARGET_LINK_LIBRARIES(cc1 backend cpp iberty)
ENDIF(GCC_BUILD_C)
