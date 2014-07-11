#-----------------------------------------------------------------------------
INCLUDE("${GCCCONFIG_SOURCE_DIR}/gcc_make_config_h.cmake")

INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libcpp/include
  ${GCC_SOURCE_DIR}/libdecnumber
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
  MESSAGE(FATAL_ERROR "Cannot find '${GCCCONFIG_BINARY_DIR}/gcc_platform.cmake'")
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

SET(build_auto auto-host.h) # or auto-build.h for cross compiling?

SET(tm_file ${tm_file} defaults.h)
SET(tm_p_file ${tm_p_file} tm-preds.h)
SET(host_xm_file auto-host.h ansidecl.h ${host_xm_file})
SET(build_xm_file ${build_auto} ansidecl.h ${build_xm_file})
SET(xm_file auto-host.h ansidecl.h ${xm_file})

# Build file and include lists.
FOREACH(m host_ build_ "")
  SET(${m}xm_include_list)
  SET(${m}xm_file_list)
  FOREACH(f ${${m}xm_file})
    IF("${f}" MATCHES "^ansidecl\\.h")
      LIST(APPEND ${m}xm_include_list ${f})
      LIST(APPEND ${m}xm_file_list ${GCC_SOURCE_DIR}/include/${f})
    ELSEIF("${f}" MATCHES "^auto-(build|host)\\.h$")
      LIST(APPEND ${m}xm_include_list ${f})
      LIST(APPEND ${m}xm_file_list ${GCC_BINARY_DIR}/gcc/${f})
    ELSE("${f}" MATCHES "^ansidecl\\.h")
      LIST(APPEND ${m}xm_include_list config/${f})
      LIST(APPEND ${m}xm_file_list ${GCC_SOURCE_DIR}/gcc/config/${f})
    ENDIF("${f}" MATCHES "^ansidecl\\.h")
  ENDFOREACH(f)
ENDFOREACH(m)

SET(tm_include_list options.h)
SET(tm_file_list)
FOREACH(f ${tm_file})
  IF("${f}" MATCHES "^defaults\\.h$")
    LIST(APPEND tm_include_list ${f})
    LIST(APPEND tm_file_list ${GCC_SOURCE_DIR}/gcc/${f})
  ELSE("${f}" MATCHES "^defaults\\.h$")
    LIST(APPEND tm_include_list config/${f})
    LIST(APPEND tm_file_list ${GCC_SOURCE_DIR}/gcc/config/${f})
  ENDIF("${f}" MATCHES "^defaults\\.h$")
ENDFOREACH(f)

SET(tm_p_include_list)
SET(tm_p_file_list)
FOREACH(f ${tm_p_file})
  IF("${f}" MATCHES "^tm-preds\\.h$")
    LIST(APPEND tm_p_include_list ${f})
    LIST(APPEND tm_p_file_list ${GCC_SOURCE_DIR}/gcc/${f})
  ELSE("${f}" MATCHES "^tm-preds\\.h$")
    LIST(APPEND tm_p_include_list config/${f})
    LIST(APPEND tm_p_file_list ${GCC_SOURCE_DIR}/gcc/config/${f})
  ENDIF("${f}" MATCHES "^tm-preds\\.h$")
ENDFOREACH(f)

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
        IF(EXISTS ${GCC_SOURCE_DIR}/gcc/${f})
          SET(${var} ${${var}} ${GCC_SOURCE_DIR}/gcc/${f})
        ELSE(EXISTS ${GCC_SOURCE_DIR}/gcc/${f})
          MESSAGE(SEND_ERROR "Cannot find platform-specific source ${f}.")
        ENDIF(EXISTS ${GCC_SOURCE_DIR}/gcc/${f})
      ENDIF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${cpu_type}/${f})
    ENDIF(EXISTS ${GCC_SOURCE_DIR}/gcc/config/${f})
  ENDFOREACH(f)
ENDMACRO(FIND_EXTRA_SRCS)
FIND_EXTRA_SRCS("${extra_objs}" extra_srcs)
FIND_EXTRA_SRCS("${c_target_objs}" c_target_srcs)
FIND_EXTRA_SRCS("${cxx_target_objs}" cxx_target_srcs)
FIND_EXTRA_SRCS("${out_host_hook_obj}" host_hook_srcs)

SET(MD_FILE ${GCC_SOURCE_DIR}/gcc/config/${md_file})

#-----------------------------------------------------------------------------
# Generate configuration header files.

GCC_MAKE_CONFIG_H(bconfig.h GCC_BCONFIG_H "${build_xm_defines}" "${build_xm_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(config.h GCC_CONFIG_H "${host_xm_defines}" "${host_xm_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(tconfig.h GCC_TCONFIG_H "USED_FOR_TARGET;${xm_defines}" "${xm_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(tm_p.h GCC_TM_P_H "" "${tm_p_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(tm.h GCC_TM_H "${tm_defines}" "${tm_include_list}" "${target_cpu_default}")

#-----------------------------------------------------------------------------
# Rules to generate options.h and options.c

SET(ALL_OPT_FILES
  ${GCC_SOURCE_DIR}/gcc/c.opt
  ${GCC_SOURCE_DIR}/gcc/common.opt
  )
FOREACH(f ${extra_options})
  LIST(APPEND ALL_OPT_FILES ${GCC_SOURCE_DIR}/gcc/config/${f})
ENDFOREACH(f)

# Find an awk tool.
IF(WIN32 AND NOT CYGWIN)
  SET(AWK_EXECUTABLE "${GCC_SOURCE_DIR}/config_cmake/nawk.exe")
ELSE(WIN32 AND NOT CYGWIN)
  # Prefer GNU awk at any location over the native awk if possible.
  # The native awk on opensolaris causes opt-gather.awk to sort incorrectly.
  FIND_PROGRAM(AWK_EXECUTABLE NAMES gawk)
  FIND_PROGRAM(AWK_EXECUTABLE NAMES awk)
  MARK_AS_ADVANCED(AWK_EXECUTABLE)
ENDIF(WIN32 AND NOT CYGWIN)

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/optionlist
  COMMAND ${AWK_EXECUTABLE} -f ${GCC_SOURCE_DIR}/gcc/opt-gather.awk ${ALL_OPT_FILES} > optionlist
  DEPENDS ${GCC_SOURCE_DIR}/gcc/opt-gather.awk ${ALL_OPT_FILES}
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/options.h
  COMMAND ${AWK_EXECUTABLE} -f ${GCC_SOURCE_DIR}/gcc/opt-functions.awk -f ${GCC_SOURCE_DIR}/gcc/opth-gen.awk < optionlist > options.h
  DEPENDS ${GCC_SOURCE_DIR}/gcc/opt-functions.awk ${GCC_SOURCE_DIR}/gcc/opth-gen.awk ${GCC_BINARY_DIR}/gcc/optionlist
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/options.c
  COMMAND ${AWK_EXECUTABLE} -f ${GCC_SOURCE_DIR}/gcc/opt-functions.awk -f ${GCC_SOURCE_DIR}/gcc/optc-gen.awk
                            -v "header_name=config.h system.h coretypes.h tm.h" < optionlist > options.c
  DEPENDS ${GCC_SOURCE_DIR}/gcc/opt-functions.awk ${GCC_SOURCE_DIR}/gcc/optc-gen.awk ${GCC_BINARY_DIR}/gcc/optionlist
  )

ADD_CUSTOM_TARGET(genoptions ALL DEPENDS
  ${GCC_BINARY_DIR}/gcc/options.h
  ${GCC_BINARY_DIR}/gcc/options.c
  )

#-----------------------------------------------------------------------------
# Rules to generate insn-modes.h, insn-modes.c, and min-insn-modes.c

ADD_EXECUTABLE(genmodes genmodes.c errors.c)
ADD_DEPENDENCIES(genmodes genoptions)
TARGET_LINK_LIBRARIES(genmodes iberty)
SET_TARGET_PROPERTIES(genmodes PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_genmodes_EXE "${GCC_GEN_DIR}/genmodes${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/insn-modes.h
  COMMAND ${GCC_genmodes_EXE} -h > insn-modes.h
  DEPENDS genmodes
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/insn-modes.c
  COMMAND ${GCC_genmodes_EXE} > insn-modes.c
  DEPENDS genmodes
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/min-insn-modes.c
  COMMAND ${GCC_genmodes_EXE} -m > min-insn-modes.c
  DEPENDS genmodes
  )

#-----------------------------------------------------------------------------
# Rules to generate genrtl.h and genrtl.c.

ADD_EXECUTABLE(gengenrtl gengenrtl.c errors.c)
ADD_DEPENDENCIES(gengenrtl genoptions)
TARGET_LINK_LIBRARIES(gengenrtl iberty)
SET_TARGET_PROPERTIES(gengenrtl PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_gengenrtl_EXE "${GCC_GEN_DIR}/gengenrtl${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/genrtl.c
  COMMAND ${GCC_gengenrtl_EXE} > genrtl.c
  DEPENDS gengenrtl
  )

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/genrtl.h
  COMMAND ${GCC_gengenrtl_EXE} -h > genrtl.h
  DEPENDS gengenrtl
  )

#-----------------------------------------------------------------------------
# Generate gtyp-gen.h.
SET(GTFILES_C
  c-lang.c c-tree.h c-decl.c c-common.c c-common.h c-pragma.c
  c-objc-common.c c-parser.c
  )

SET(GTFILES_CXX
  cp/rtti.c cp/mangle.c cp/name-lookup.h cp/name-lookup.c cp/cp-tree.h
  cp/decl.h cp/call.c cp/decl.c cp/decl2.c cp/pt.c cp/repo.c
  cp/semantics.c cp/tree.c cp/parser.c cp/method.c cp/typeck2.c
  c-common.c c-common.h c-lex.c c-pragma.c cp/class.c
  cp/cp-objcp-common.c
  )

SET(GTFILES
  # input.h coretypes.h # these files are listed in order below
  # bitmap.h coverage.c rtl.h optabs.h tree.h function.h libfuncs.h
  real.h varray.h insn-addr.h hwint.h ipa-reference.h output.h cselib.h
  basic-block.h cgraph.h c-common.h c-tree.h reload.h alias.c bitmap.c
  cselib.c cgraph.c ipa-prop.c ipa-cp.c ipa-inline.c dbxout.c
  dwarf2out.c dwarf2asm.c dojump.c tree-profile.c emit-rtl.c except.c
  explow.c expr.c function.c except.h gcse.c integrate.c lists.c
  optabs.c profile.c regclass.c reg-stack.c cfglayout.c sdbout.c
  stor-layout.c stringpool.c tree.c varasm.c tree-mudflap.c tree-flow.h
  c-objc-common.c c-common.c c-parser.c tree-ssanames.c tree-eh.c
  tree-ssa-address.c tree-phinodes.c tree-cfg.c tree-dfa.c
  tree-ssa-propagate.c tree-iterator.c gimplify.c tree-chrec.h
  tree-vect-generic.c tree-ssa-operands.h tree-ssa-operands.c
  tree-profile.c tree-nested.c ipa-reference.c tree-ssa-structalias.h
  tree-ssa-structalias.c c-pragma.h omp-low.c targhooks.c cgraphunit.c
  )

SET(GTYP_GEN_LANG_FILES)
SET(GTYP_GEN_LANGS_FOR_LANG_FILES)
SET(GTYP_GEN_ALL_FILES)
FOREACH(f
    ${GCC_SOURCE_DIR}/gcc/input.h
    ${GCC_SOURCE_DIR}/gcc/coretypes.h
    ${GCC_SOURCE_DIR}/libcpp/include/line-map.h
    ${GCC_SOURCE_DIR}/libcpp/include/cpplib.h
    ${GCC_SOURCE_DIR}/libcpp/include/cpp-id-data.h
    ${host_xm_file_list}
    ${tm_file_list}
    ${GCC_BINARY_DIR}/gcc/options.h
    ${GCC_SOURCE_DIR}/include/hashtab.h
    ${GCC_SOURCE_DIR}/include/splay-tree.h
    ${GCC_SOURCE_DIR}/gcc/bitmap.h
    ${GCC_SOURCE_DIR}/gcc/coverage.c
    ${GCC_SOURCE_DIR}/gcc/rtl.h
    ${GCC_SOURCE_DIR}/gcc/optabs.h
    ${GCC_SOURCE_DIR}/gcc/tree.h
    ${GCC_SOURCE_DIR}/gcc/function.h
    ${GCC_SOURCE_DIR}/gcc/libfuncs.h
    ${GCC_SOURCE_DIR}/libcpp/include/symtab.h
    ${GCC_SOURCE_DIR}/gcc/config/${out_file}
    )
  SET(GTYP_GEN_ALL_FILES "${GTYP_GEN_ALL_FILES}\"${f}\",\n")
ENDFOREACH(f)
FOREACH(f ${GTFILES})
  SET(GTYP_GEN_ALL_FILES
    "${GTYP_GEN_ALL_FILES}\"${GCC_SOURCE_DIR}/gcc/${f}\",\n")
ENDFOREACH(f)
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

ADD_EXECUTABLE(gengtype
  gengtype.c gengtype-lex.c gengtype-yacc.c errors.c
  )
ADD_DEPENDENCIES(gengtype genoptions)
TARGET_LINK_LIBRARIES(gengtype iberty)
SET_TARGET_PROPERTIES(gengtype PROPERTIES
  COMPILE_FLAGS "-DGENERATOR_FILE -DYY_NO_UNISTD_H")

SET(GCC_gengtype_EXE "${GCC_GEN_DIR}/gengtype${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/gtype-desc.c
         ${GCC_BINARY_DIR}/gcc/gtype-desc.h
  COMMAND ${GCC_gengtype_EXE}
  DEPENDS gengtype
  )

#-----------------------------------------------------------------------------
# Rules to generate tree-check.h.

ADD_EXECUTABLE(gencheck gencheck.c)
ADD_DEPENDENCIES(gencheck genoptions)
TARGET_LINK_LIBRARIES(gencheck iberty)
SET_TARGET_PROPERTIES(gencheck PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_gencheck_EXE "${GCC_GEN_DIR}/gencheck${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/tree-check.h
  COMMAND ${GCC_gencheck_EXE} > tree-check.h
  DEPENDS gencheck
  )

#-----------------------------------------------------------------------------
# Sources used in several generator executables.

ADD_LIBRARY(gen
  rtl.c read-rtl.c ggc-none.c vec.c
  gensupport.c print-rtl.c errors.c

  ${GCC_BINARY_DIR}/gcc/min-insn-modes.c
  ${GCC_BINARY_DIR}/gcc/insn-modes.h
  ${GCC_BINARY_DIR}/gcc/tree-check.h
  ${GCC_BINARY_DIR}/gcc/gtype-desc.h
  )
ADD_DEPENDENCIES(gen genoptions)
TARGET_LINK_LIBRARIES(gen iberty)
SET_TARGET_PROPERTIES(gen PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

#-----------------------------------------------------------------------------
# Rules to generate insn-constants.h.

ADD_EXECUTABLE(genconstants genconstants.c)
TARGET_LINK_LIBRARIES(genconstants gen)
SET_TARGET_PROPERTIES(genconstants PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_genconstants_EXE "${GCC_GEN_DIR}/genconstants${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/insn-constants.h
  COMMAND ${GCC_genconstants_EXE} ${MD_FILE} > insn-constants.h
  DEPENDS genconstants ${MD_FILE}
  )

#-----------------------------------------------------------------------------
# Rules to generate tm-preds.h, tm-constrs.h, and insn-preds.c.

ADD_EXECUTABLE(genpreds genpreds.c)
TARGET_LINK_LIBRARIES(genpreds gen)
SET_TARGET_PROPERTIES(genpreds PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_genpreds_EXE "${GCC_GEN_DIR}/genpreds${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/tm-preds.h
  COMMAND ${GCC_genpreds_EXE} -h ${MD_FILE} > tm-preds.h
  DEPENDS genpreds ${MD_FILE}
  )

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/tm-constrs.h
  COMMAND ${GCC_genpreds_EXE} -c ${MD_FILE} > tm-constrs.h
  DEPENDS genpreds ${MD_FILE}
  )

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/insn-preds.c
  COMMAND ${GCC_genpreds_EXE} ${MD_FILE} > insn-preds.c
  DEPENDS genpreds ${MD_FILE}
  )

#-----------------------------------------------------------------------------
# Rules to generate gencondmd.c.

ADD_EXECUTABLE(genconditions genconditions.c)
TARGET_LINK_LIBRARIES(genconditions gen)
SET_TARGET_PROPERTIES(genconditions PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_genconditions_EXE "${GCC_GEN_DIR}/genconditions${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/gencondmd.c
  COMMAND ${GCC_genconditions_EXE} ${MD_FILE} > gencondmd.c
  DEPENDS genconditions ${MD_FILE}
  )

#-----------------------------------------------------------------------------
# Rules to generate insn-conditions.md.

SET(MD_COND ${GCC_BINARY_DIR}/gcc/insn-conditions.md)

ADD_EXECUTABLE(gencondmd
  ${GCC_BINARY_DIR}/gcc/gencondmd.c
  ${GCC_BINARY_DIR}/gcc/insn-constants.h
  ${GCC_BINARY_DIR}/gcc/tm-preds.h
  ${GCC_BINARY_DIR}/gcc/tm-constrs.h
  )
TARGET_LINK_LIBRARIES(gencondmd gen)
SET_TARGET_PROPERTIES(gencondmd PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_gencondmd_EXE "${GCC_GEN_DIR}/gencondmd${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${MD_COND}
  COMMAND ${GCC_gencondmd_EXE} > ${MD_COND}
  DEPENDS gencondmd
  )

#-----------------------------------------------------------------------------
# Rules to generate insn-config.h, insn-codes.h, insn-attr.h, and insn-flags.h.

FOREACH(tgt config codes attr flags)
  ADD_EXECUTABLE(gen${tgt} gen${tgt}.c)
  TARGET_LINK_LIBRARIES(gen${tgt} gen)
  SET_TARGET_PROPERTIES(gen${tgt} PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

  SET(GCC_gen${tgt}_EXE "${GCC_GEN_DIR}/gen${tgt}${GCC_EXE_EXT}")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${GCC_BINARY_DIR}/gcc/insn-${tgt}.h
    COMMAND ${GCC_gen${tgt}_EXE} ${MD_FILE} ${MD_COND} > insn-${tgt}.h
    DEPENDS gen${tgt} ${MD_FILE} ${MD_COND}
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# Rules to generate insn-emit.c, insn-extract.c, insn-opinit.c,
# insn-peep.c, insn-output.c, insn-recog.c, insn-attrtab.c, and
# insn-automata.c.

SET(genautomata_EXTRA_LIBS m)
IF(WIN32)
  IF(NOT MINGW)
    SET(genautomata_EXTRA_LIBS)
  ENDIF(NOT MINGW)
ENDIF(WIN32)
FOREACH(tgt emit extract opinit peep output recog attrtab automata)
  ADD_EXECUTABLE(gen${tgt} gen${tgt}.c)
  TARGET_LINK_LIBRARIES(gen${tgt} gen ${gen${tgt}_EXTRA_LIBS})
  SET_TARGET_PROPERTIES(gen${tgt} PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

  SET(GCC_gen${tgt}_EXE "${GCC_GEN_DIR}/gen${tgt}${GCC_EXE_EXT}")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${GCC_BINARY_DIR}/gcc/insn-${tgt}.c
    COMMAND ${GCC_gen${tgt}_EXE} ${MD_FILE} ${MD_COND} > insn-${tgt}.c
    DEPENDS gen${tgt} ${MD_FILE} ${MD_COND}
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# Rules to generate gcov-iov.h.

ADD_EXECUTABLE(gcov_iov gcov-iov.c)
SET_TARGET_PROPERTIES(gcov_iov PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_gcov_iov_EXE "${GCC_GEN_DIR}/gcov_iov${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/gcov-iov.h
  COMMAND ${GCC_gcov_iov_EXE} '4.2.1' '' > gcov-iov.h
  DEPENDS gcov_iov
  )

#-----------------------------------------------------------------------------
# Special flags for some source files.

SET_SOURCE_FILES_PROPERTIES(toplev.c
  PROPERTIES COMPILE_FLAGS "-DTARGET_NAME=${target}")

#-----------------------------------------------------------------------------
# Compiler backend library.

ADD_LIBRARY(backend
  double-int.c tree-chrec.c tree-scalar-evolution.c tree-data-ref.c
  tree-cfg.c tree-dfa.c tree-eh.c tree-ssa.c tree-optimize.c
  tree-gimple.c gimplify.c tree-pretty-print.c tree-into-ssa.c
  tree-outof-ssa.c tree-ssa-ccp.c tree-vn.c tree-ssa-uncprop.c
  tree-ssa-dce.c tree-ssa-copy.c tree-nrv.c tree-ssa-copyrename.c
  tree-ssa-pre.c tree-ssa-live.c tree-ssa-operands.c tree-ssa-alias.c
  tree-ssa-phiopt.c tree-ssa-forwprop.c tree-nested.c tree-ssa-dse.c
  tree-ssa-dom.c domwalk.c tree-tailcall.c gimple-low.c tree-iterator.c
  omp-low.c tree-phinodes.c tree-ssanames.c tree-sra.c tree-complex.c
  tree-vect-generic.c tree-ssa-loop.c tree-ssa-loop-niter.c
  tree-ssa-loop-manip.c tree-ssa-threadupdate.c tree-ssa-threadedge.c
  tree-vectorizer.c tree-vect-analyze.c tree-vect-transform.c
  tree-vect-patterns.c tree-ssa-loop-prefetch.c tree-ssa-loop-ivcanon.c
  tree-ssa-propagate.c tree-ssa-address.c tree-ssa-math-opts.c
  tree-ssa-loop-ivopts.c tree-if-conv.c tree-ssa-loop-unswitch.c alias.c
  bb-reorder.c bitmap.c builtins.c caller-save.c calls.c cfg.c cfganal.c
  cfgbuild.c cfgcleanup.c cfglayout.c cfgloop.c cfgloopanal.c
  cfgloopmanip.c loop-init.c loop-unswitch.c loop-unroll.c cfgrtl.c
  combine.c conflict.c convert.c coverage.c cse.c cselib.c dbxout.c
  ddg.c tree-ssa-loop-ch.c loop-invariant.c tree-ssa-loop-im.c debug.c
  df-core.c df-problems.c df-scan.c dfp.c diagnostic.c dojump.c
  dominance.c loop-doloop.c dwarf2asm.c dwarf2out.c emit-rtl.c except.c
  explow.c loop-iv.c expmed.c expr.c final.c flow.c fold-const.c
  function.c gcse.c genrtl.c ggc-common.c global.c graph.c
  haifa-sched.c hooks.c ifcvt.c
  integrate.c intl.c jump.c langhooks.c lcm.c lists.c local-alloc.c
  mode-switching.c modulo-sched.c optabs.c
  ${GCC_BINARY_DIR}/gcc/options.c
  opts.c opts-common.c params.c postreload.c postreload-gcse.c predict.c
  pointer-set.c backend-print-rtl.c print-tree.c
  profile.c value-prof.c var-tracking.c real.c recog.c reg-stack.c
  regclass.c regmove.c regrename.c reload.c reload1.c reorg.c resource.c
  backend-rtl.c rtlanal.c rtl-error.c sbitmap.c sched-deps.c sched-ebb.c
  sched-rgn.c sched-vis.c sdbout.c see.c simplify-rtx.c sreal.c stmt.c
  stor-layout.c stringpool.c struct-equiv.c targhooks.c timevar.c
  toplev.c tracer.c tree.c tree-dump.c varasm.c varray.c
  backend-vec.c version.c
  vmsdbgout.c xcoffout.c alloc-pool.c et-forest.c cfghooks.c bt-load.c
  pretty-print.c ggc-page.c web.c passes.c tree-profile.c rtlhooks.c
  cfgexpand.c lambda-mat.c lambda-trans.c lambda-code.c
  tree-loop-linear.c tree-ssa-sink.c tree-vrp.c tree-stdarg.c
  tree-cfgcleanup.c tree-ssa-reassoc.c tree-ssa-structalias.c
  tree-object-size.c rtl-factoring.c

  ${GCC_SOURCE_DIR}/gcc/config/${out_file} ${extra_srcs} ${host_hook_srcs}

  tree-inline.c cgraph.c cgraphunit.c tree-nomudflap.c ipa.c
  ipa-inline.c ipa-utils.c ipa-reference.c ipa-pure-const.c
  ipa-type-escape.c ipa-prop.c ipa-cp.c

  ${GCC_BINARY_DIR}/gcc/gtype-desc.c
  ${GCC_BINARY_DIR}/gcc/insn-attrtab.c
  ${GCC_BINARY_DIR}/gcc/insn-emit.c
  ${GCC_BINARY_DIR}/gcc/insn-modes.c
  ${GCC_BINARY_DIR}/gcc/insn-extract.c
  ${GCC_BINARY_DIR}/gcc/insn-opinit.c
  ${GCC_BINARY_DIR}/gcc/insn-output.c
  ${GCC_BINARY_DIR}/gcc/insn-peep.c
  ${GCC_BINARY_DIR}/gcc/insn-recog.c
  ${GCC_BINARY_DIR}/gcc/insn-preds.c
  ${GCC_BINARY_DIR}/gcc/insn-automata.c

  ${GCC_BINARY_DIR}/gcc/insn-flags.h
  ${GCC_BINARY_DIR}/gcc/insn-config.h
  ${GCC_BINARY_DIR}/gcc/insn-codes.h
  ${GCC_BINARY_DIR}/gcc/insn-attr.h
  ${GCC_BINARY_DIR}/gcc/genrtl.h
  ${GCC_BINARY_DIR}/gcc/gcov-iov.h
  )
