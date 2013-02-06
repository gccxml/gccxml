#-----------------------------------------------------------------------------
INCLUDE("${GCCCONFIG_SOURCE_DIR}/gcc_make_config_h.cmake")
#INCLUDE("${GCCCONFIG_SOURCE_DIR}/gcc_make_constants_h.cmake")
INCLUDE(${GCCCONFIG_SOURCE_DIR}/today.cmake)
TODAY(GCC_DATESTAMP)

INCLUDE_DIRECTORIES(BEFORE
  ${GCC_SOURCE_DIR}/include
  ${GCC_SOURCE_DIR}/libcpp/include
  ${GCC_SOURCE_DIR}/libdecnumber
  ${GCC_SOURCE_DIR}/libdecnumber/dpd
  ${GCC_SOURCE_DIR}/gcc
  ${GCC_SOURCE_DIR}/gcc/config
  ${GCC_BINARY_DIR}/gcc)

#-----------------------------------------------------------------------------
# GCC backend version information
SET(BUILDING_GCC_MAJOR      4)
SET(BUILDING_GCC_MINOR      7)
SET(BUILDING_GCC_PATCHLEVEL 2)
SET(GCC_BASEVER             "4.7.2")
SET(GCC_VERSION "${BUILDING_GCC_MAJOR}${BUILDING_GCC_MINOR}${BUILDING_GCC_PATCHLEVEL}")
MATH(EXPR BUILDING_GCC_VERSION "(${BUILDING_GCC_MAJOR} * 1000 + ${BUILDING_GCC_MINOR})")
#SET(BUILDING_GCC_VERSION    "(BUILDING_GCC_MAJOR * 1000 + BUILDING_GCC_MINOR)")

ADD_DEFINITIONS(-DIN_GCC -DHAVE_CONFIG_H)

INCLUDE_REGULAR_EXPRESSION("^([^ri]|i[^n]|in[^s]|ins[^n]|insn[^-]|r[^t]|rt[^l]|rtl[^.])")

SUBDIRS(cp)

#-----------------------------------------------------------------------------

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

# Generate arch-dependent builtins header, e.g. i386-builtin-types.inc
ADD_SUBDIRECTORY(config)
# INCLUDE("${GCC_SOURCE_DIR}/config_cmake/archs/t-${cpu_type}.cmake")
SET_SOURCE_FILES_PROPERTIES(
    ${CMAKE_CURRENT_BINARY_DIR}/config/${cpu_type}-builtin-types.inc 
    PROPERTIES GENERATED 1
    )

#ADD_CUSTOM_COMMAND(
#    OUTPUT ${cpu_type}
#    COMMAND ${CMAKE_COMMAND}
#    DEPENDS ${cpu_type}-builtin-types.inc
#  )

SET(build_auto auto-host.h) # or auto-build.h for cross compiling?

SET(tm_file ${tm_file})
SET(tm_p_file ${tm_p_file} tm-preds.h)
SET(host_xm_file auto-host.h ansidecl.h ${host_xm_file})
SET(host_xm_file_list  ${host_xm_file})
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
  IF ("${f}" MATCHES "^defaults\\.h$")
    LIST(APPEND tm_include_list ${GCC_SOURCE_DIR}/gcc/${f})
    LIST(APPEND tm_file_list    ${GCC_SOURCE_DIR}/gcc/${f})
  ELSE("${f}" MATCHES "^defaults\\.h$")
    IF(EXISTS "./${f}")
      LIST(APPEND tm_include_list ${f})
      LIST(APPEND tm_file_list    ${f})
    ELSE(EXISTS "./${f}")
      LIST(APPEND tm_include_list ${GCC_SOURCE_DIR}/gcc/config/${f})
      LIST(APPEND tm_file_list    ${GCC_SOURCE_DIR}/gcc/config/${f})
    ENDIF(EXISTS "./${f}")
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
# Support --enable-initfini-array.

IF("${ENABLE_INITFINI_ARRAY}")
  SET(tm_file_list ${tm_file_list} ${GCC_SOURCE_DIR}/gcc/config/initfini-array.h)
ENDIF("${ENABLE_INITFINI_ARRAY}")

#-----------------------------------------------------------------------------
# Generate configuration header files.

GCC_MAKE_CONFIG_H(bconfig.h GCC_BCONFIG_H "${build_xm_defines}" "${build_xm_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(config.h GCC_CONFIG_H "${host_xm_defines}" "${host_xm_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(tconfig.h GCC_TCONFIG_H "USED_FOR_TARGET;${xm_defines}" "${xm_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(tm_p.h GCC_TM_P_H "" "${tm_p_include_list}" "${target_cpu_default}")
GCC_MAKE_CONFIG_H(tm.h GCC_TM_H "${tm_defines}" "${tm_include_list}" "${target_cpu_default}")

#-----------------------------------------------------------------------------
# Rules to generate specs.h

SET(LANG_SPEC_FILES
    cp/lang-specs.h
    go/lang-specs.h
    lto/lang-specs.h
    objcp/lang-specs.h
    fortran/lang-specs.h
    java/lang-specs.h
    objc/lang-specs.h
    )
FILE(WRITE specs.h )
FOREACH(f ${LANG_SPEC_FILES})
    FILE(APPEND specs.h "#include \"${f}\"\n")
ENDFOREACH()

#lsf="$(lang_specs_files)"; for f in $$lsf; do \
#	    echo "#include \"$$f\""; \
#	done | sed 's|$(srcdir)/||' > tmp-specs.h

#-----------------------------------------------------------------------------
# Rules to generate options.h and options.c


SET(ALL_OPT_FILES
  ${GCC_SOURCE_DIR}/gcc/c-family/c.opt
  ${GCC_SOURCE_DIR}/gcc/common.opt
  ${GCC_SOURCE_DIR}/gcc/fortran/lang.opt
  ${GCC_SOURCE_DIR}/gcc/go/lang.opt
  ${GCC_SOURCE_DIR}/gcc/java/lang.opt
  ${GCC_SOURCE_DIR}/gcc/lto/lang.opt
  )
FOREACH(f ${extra_options})
  LIST(APPEND ALL_OPT_FILES ${GCC_SOURCE_DIR}/gcc/config/${f})
ENDFOREACH(f)

# Find an awk tool.
IF(WIN32 AND NOT CYGWIN)
    SET(AWK_EXECUTABLE "${GCC_SOURCE_DIR}/config_cmake/nawk.exe" CACHE INTERNAL "Awk Executable")
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
  COMMAND ${AWK_EXECUTABLE} -f ${GCC_SOURCE_DIR}/gcc/opt-functions.awk -f ${GCC_SOURCE_DIR}/gcc/opt-read.awk
                            -f ${GCC_SOURCE_DIR}/gcc/opth-gen.awk < optionlist > options.h
  DEPENDS ${GCC_SOURCE_DIR}/gcc/opt-functions.awk ${GCC_SOURCE_DIR}/gcc/opth-gen.awk ${GCC_BINARY_DIR}/gcc/optionlist
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/options.c
  COMMAND ${AWK_EXECUTABLE} -f ${GCC_SOURCE_DIR}/gcc/opt-functions.awk -f ${GCC_SOURCE_DIR}/gcc/opt-read.awk
                            -f ${GCC_SOURCE_DIR}/gcc/optc-gen.awk
                            -v "header_name=\"config.h system.h coretypes.h tm.h\"" < optionlist > options.c
  DEPENDS ${GCC_SOURCE_DIR}/gcc/opt-functions.awk ${GCC_SOURCE_DIR}/gcc/opt-read.awk ${GCC_SOURCE_DIR}/gcc/optc-gen.awk ${GCC_BINARY_DIR}/gcc/optionlist
  )

ADD_CUSTOM_COMMAND(OUTPUT ${GCC_BINARY_DIR}/gcc/options-save.c
  COMMAND ${AWK_EXECUTABLE} -f ${GCC_SOURCE_DIR}/gcc/opt-functions.awk -f ${GCC_SOURCE_DIR}/gcc/opt-read.awk
                            -f ${GCC_SOURCE_DIR}/gcc/optc-save-gen.awk
                            -v "header_name=\"config.h system.h coretypes.h tm.h\"" < optionlist > options-save.c
  DEPENDS ${GCC_SOURCE_DIR}/gcc/opt-functions.awk ${GCC_SOURCE_DIR}/gcc/opt-read.awk ${GCC_BINARY_DIR}/gcc/optionlist
  )

ADD_CUSTOM_TARGET(genoptions ALL DEPENDS
  ${GCC_BINARY_DIR}/gcc/options.h
  ${GCC_BINARY_DIR}/gcc/options.c
  ${GCC_BINARY_DIR}/gcc/options-save.c
  )

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

FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cache.cmake")
FILE(APPEND "${CMAKE_CURRENT_BINARY_DIR}/cache.cmake" "SET(GCCCONFIG_SOURCE_DIR ${GCCCONFIG_SOURCE_DIR})\n")
FILE(APPEND "${CMAKE_CURRENT_BINARY_DIR}/cache.cmake" "SET(GCC_BINARY_DIR ${GCC_BINARY_DIR})\n")

# Need to remake tm.h, now including insn-constants.h
ADD_CUSTOM_COMMAND(
  OUTPUT tm.h-full
  COMMAND ${CMAKE_COMMAND} -P ${GCCCONFIG_SOURCE_DIR}/gcc_make_constants_h.cmake 
          "\"${tm_defines}\"" "\"${tm_include_list}\"" "\"${target_cpu_default}\""
  DEPENDS ${GCC_BINARY_DIR}/gcc/insn-constants.h
  )

#SET(tm_include_list insn-constants.h ${tm_include_list})
#GCC_MAKE_CONFIG_H(tm.h GCC_TM_H "${tm_defines}" "${tm_include_list}" "${target_cpu_default}")

#-----------------------------------------------------------------------------
# Rules to generate insn-config.h, insn-codes.h, insn-attr.h, 
# insn-attr-common.h and insn-flags.h.

FOREACH(tgt config codes attr attr-common flags)
  ADD_EXECUTABLE(gen${tgt} gen${tgt}.c)
  TARGET_LINK_LIBRARIES(gen${tgt} gen)
  SET_TARGET_PROPERTIES(gen${tgt} PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

  SET(GCC_gen${tgt}_EXE "${GCC_GEN_DIR}/gen${tgt}${GCC_EXE_EXT}")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${GCC_BINARY_DIR}/gcc/insn-${tgt}.h
    COMMAND ${GCC_gen${tgt}_EXE} ${MD_FILE} ${MD_COND} > insn-${tgt}.h
    DEPENDS gen${tgt} ${MD_FILE} ${MD_COND} tm.h-full
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# Rules to generate insn-modes.h, insn-modes.c, and min-insn-modes.c

ADD_EXECUTABLE(genmodes genmodes.c errors.c)
ADD_DEPENDENCIES(genmodes genoptions tm.h-full insn-flags.h)
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
# Rules to generate all-tree.h

set (CMAKE_EXTRA_INCLUDE_FILES ${GCC_BINARY_DIR}/gcc/all-tree.def)
set (LANG_TREE_FILES
            c-family/c-common.def
            cp/cp-tree.def
            # java/java-tree.def
            # ada/gcc-interface/ada-tree.def
            # objc/objc-tree.def
    )

set (TREE_H tree.h
            all-tree.def
            tree.def
            c-family-c-common.def
            cp/cp-tree.def

            # java/java-tree.def
            # ada/gcc-interface/ada-tree.def
            # objc/objc-tree.def

            machmode.h
            mode-classes.def
            insn-modes.h
#            tree-check.h
            builtins.def
            sync-builtins.def
            omp-builtins.def
            gtm-builtins.def
            ${GCC_SOURCE_DIR}/libcpp/include/line-map.h
            input.h
            statistics.h
            vec.h
            statistics.h
            treestruct.def
            ${GCC_SOURCE_DIR}/include/hashtab.h
            double-int.h
            alias.h
            ${GCC_SOURCE_DIR}/libcpp/include/symtab.h
            ${GCC_SOURCE_DIR}/include/obstack.h
            flags.h
            coretypes.h
            flag-types.h
            options.h
            flag-types.h
            vecir.h
            real.h
            fixed-value.h
            double-int.h
)

FILE(WRITE ${GCC_BINARY_DIR}/gcc/all-tree.def)
FILE(APPEND ${GCC_BINARY_DIR}/gcc/all-tree.def
            "#include \"tree.def\"\n")
FILE(APPEND ${GCC_BINARY_DIR}/gcc/all-tree.def
            "END_OF_BASE_TREE_CODES\n")
FOREACH(_file ${LANG_TREE_FILES})
    FILE(APPEND ${GCC_BINARY_DIR}/gcc/all-tree.def 
                 "#include \"${_file}\"\n" )
ENDFOREACH()

#-----------------------------------------------------------------------------
# Rules to generate genrtl.h and genrtl.c.

ADD_EXECUTABLE(gengenrtl gengenrtl.c errors.c)
ADD_DEPENDENCIES(gengenrtl genoptions ${GCC_BINARY_DIRECTORY}/gcc/insn-flags.h ${GCC_BINARY_DIRECTORY}/gcc/gtype-desc.h)
TARGET_LINK_LIBRARIES(gengenrtl iberty)
SET_TARGET_PROPERTIES(gengenrtl PROPERTIES COMPILE_FLAGS "-DGENERATOR_FILE -I${CMAKE_CURRENT_BINARY_DIR}")

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

## LANGUAGE-SPECIFIC LIBRARIES

SET(GCC_SUB_SOURCE_DIR "${GCC_SOURCE_DIR}/gcc")

SET(ADA_LANG_FILES
  ${GCC_SUB_SOURCE_DIR}/ada/gcc-interface/ada-tree.h    ${GCC_SUB_SOURCE_DIR}/ada/gcc-interface/gigi.h
  ${GCC_SUB_SOURCE_DIR}/ada/gcc-interface/decl.c        ${GCC_SUB_SOURCE_DIR}/ada/gcc-interface/trans.c
  ${GCC_SUB_SOURCE_DIR}/ada/gcc-interface/utils.c       ${GCC_SUB_SOURCE_DIR}/ada/gcc-interface/misc.c
  )      # ada/config-lang.in

SET(CP_LANG_FILES
    ${GCC_SUB_SOURCE_DIR}/cp/rtti.c             ${GCC_SUB_SOURCE_DIR}/cp/mangle.c
    ${GCC_SUB_SOURCE_DIR}/cp/name-lookup.h      ${GCC_SUB_SOURCE_DIR}/cp/name-lookup.c
    ${GCC_SUB_SOURCE_DIR}/cp/cp-tree.h          ${GCC_SUB_SOURCE_DIR}/cp/decl.h
    ${GCC_SUB_SOURCE_DIR}/cp/call.c             ${GCC_SUB_SOURCE_DIR}/cp/decl.c
    ${GCC_SUB_SOURCE_DIR}/cp/decl2.c            ${GCC_SUB_SOURCE_DIR}/cp/pt.c
    ${GCC_SUB_SOURCE_DIR}/cp/repo.c             ${GCC_SUB_SOURCE_DIR}/cp/semantics.c
    ${GCC_SUB_SOURCE_DIR}/cp/tree.c             ${GCC_SUB_SOURCE_DIR}/cp/parser.h
    ${GCC_SUB_SOURCE_DIR}/cp/parser.c           ${GCC_SUB_SOURCE_DIR}/cp/method.c
    ${GCC_SUB_SOURCE_DIR}/cp/typeck2.c          ${GCC_SUB_SOURCE_DIR}/c-family/c-common.c
    ${GCC_SUB_SOURCE_DIR}/c-family/c-common.h   ${GCC_SUB_SOURCE_DIR}/c-family/c-objc.h 
    ${GCC_SUB_SOURCE_DIR}/c-family/c-lex.c      ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.h
    ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.c   ${GCC_SUB_SOURCE_DIR}/cp/class.c
    ${GCC_SUB_SOURCE_DIR}/cp/cp-objcp-common.c  ${GCC_SUB_SOURCE_DIR}/cp/cp-lang.c
    ${GCC_SUB_SOURCE_DIR}/cp/except.c
) #   cp/config-lang.in

SET(FORT_LANG_FILES 
  ${GCC_SUB_SOURCE_DIR}/fortran/f95-lang.c      ${GCC_SUB_SOURCE_DIR}/fortran/trans-decl.c
  ${GCC_SUB_SOURCE_DIR}/fortran/trans-intrinsic.c ${GCC_SUB_SOURCE_DIR}/fortran/trans-io.c
  ${GCC_SUB_SOURCE_DIR}/fortran/trans-stmt.c    ${GCC_SUB_SOURCE_DIR}/fortran/trans-types.c
  ${GCC_SUB_SOURCE_DIR}/fortran/trans-types.h   ${GCC_SUB_SOURCE_DIR}/fortran/trans.h 
  ${GCC_SUB_SOURCE_DIR}/fortran/trans-const.h
  ) # fortran/config-lang.in

SET(GO_LANG_FILES 
  ${GCC_SUB_SOURCE_DIR}/go/go-lang.c    ${GCC_SUB_SOURCE_DIR}/go/go-c.h
  )   # go/config-lang.in

SET(JAVA_LANG_FILES 
  ${GCC_SUB_SOURCE_DIR}/java/java-tree.h    ${GCC_SUB_SOURCE_DIR}/java/jcf.h
  ${GCC_SUB_SOURCE_DIR}/java/parse.h        ${GCC_SUB_SOURCE_DIR}/java/builtins.c
  ${GCC_SUB_SOURCE_DIR}/java/class.c        ${GCC_SUB_SOURCE_DIR}/java/constants.c
  ${GCC_SUB_SOURCE_DIR}/java/decl.c         ${GCC_SUB_SOURCE_DIR}/java/expr.c
  ${GCC_SUB_SOURCE_DIR}/java/jcf-parse.c    ${GCC_SUB_SOURCE_DIR}/java/lang.c
  ${GCC_SUB_SOURCE_DIR}/java/mangle.c       ${GCC_SUB_SOURCE_DIR}/java/resource.c
  )     # java/config-lang.in

SET(LTO_LANG_FILES 
    ${GCC_SUB_SOURCE_DIR}/lto/lto-tree.h    ${GCC_SUB_SOURCE_DIR}/lto/lto-lang.c
    ${GCC_SUB_SOURCE_DIR}/lto/lto.c
    )      # lto/config-lang.in

SET(OBJC_LANG_FILES 
  ${GCC_SUB_SOURCE_DIR}/objc/objc-map.h         ${GCC_SUB_SOURCE_DIR}/c-family/c-objc.h
  ${GCC_SUB_SOURCE_DIR}/objc/objc-act.h         ${GCC_SUB_SOURCE_DIR}/objc/objc-act.c
  ${GCC_SUB_SOURCE_DIR}/objc/objc-runtime-shared-support.c  ${GCC_SUB_SOURCE_DIR}/objc/objc-gnu-runtime-abi-01.c
  ${GCC_SUB_SOURCE_DIR}/objc/objc-next-runtime-abi-01.c     ${GCC_SUB_SOURCE_DIR}/objc/objc-next-runtime-abi-02.c
  ${GCC_SUB_SOURCE_DIR}/c-parser.c              ${GCC_SUB_SOURCE_DIR}/c-tree.h
  ${GCC_SUB_SOURCE_DIR}/c-decl.c                ${GCC_SUB_SOURCE_DIR}/c-lang.h 
  ${GCC_SUB_SOURCE_DIR}/c-objc-common.c         ${GCC_SUB_SOURCE_DIR}/c-family/c-common.c
  ${GCC_SUB_SOURCE_DIR}/c-family/c-common.h     ${GCC_SUB_SOURCE_DIR}/c-family/c-cppbuiltin.c
  ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.h     ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.c
  ) # objc/config-lang.in

SET(OBJCP_LANG_FILES 
  ${GCC_SUB_SOURCE_DIR}/cp/rtti.c         ${GCC_SUB_SOURCE_DIR}/cp/mangle.c 
  ${GCC_SUB_SOURCE_DIR}/cp/name-lookup.h  ${GCC_SUB_SOURCE_DIR}/cp/name-lookup.c
  ${GCC_SUB_SOURCE_DIR}/cp/cp-tree.h      ${GCC_SUB_SOURCE_DIR}/cp/decl.h
  ${GCC_SUB_SOURCE_DIR}/cp/call.c         ${GCC_SUB_SOURCE_DIR}/cp/decl.c
  ${GCC_SUB_SOURCE_DIR}/cp/decl2.c        ${GCC_SUB_SOURCE_DIR}/cp/pt.c
  ${GCC_SUB_SOURCE_DIR}/cp/repo.c         ${GCC_SUB_SOURCE_DIR}/cp/semantics.c
  ${GCC_SUB_SOURCE_DIR}/cp/tree.c         ${GCC_SUB_SOURCE_DIR}/cp/parser.h
  ${GCC_SUB_SOURCE_DIR}/cp/parser.c       ${GCC_SUB_SOURCE_DIR}/cp/method.c
  ${GCC_SUB_SOURCE_DIR}/cp/typeck2.c      ${GCC_SUB_SOURCE_DIR}/c-family/c-common.c
  ${GCC_SUB_SOURCE_DIR}/c-family/c-common.h  ${GCC_SUB_SOURCE_DIR}/c-family/c-objc.h 
  ${GCC_SUB_SOURCE_DIR}/c-family/c-lex.c  ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.h
  ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.c  ${GCC_SUB_SOURCE_DIR}/cp/class.c
  ${GCC_SUB_SOURCE_DIR}/cp/cp-objcp-common.c ${GCC_SUB_SOURCE_DIR}/objc/objc-map.h
  ${GCC_SUB_SOURCE_DIR}/objc/objc-act.h   ${GCC_SUB_SOURCE_DIR}/objc/objc-act.c
  ${GCC_SUB_SOURCE_DIR}/objc/objc-runtime-shared-support.c ${GCC_SUB_SOURCE_DIR}/objc/objc-gnu-runtime-abi-01.c
  ${GCC_SUB_SOURCE_DIR}/objc/objc-next-runtime-abi-01.c    ${GCC_SUB_SOURCE_DIR}/objc/objc-next-runtime-abi-02.c
  ${GCC_SUB_SOURCE_DIR}/c-family/c-cppbuiltin.c
)  # objcp/config-lang.in

SET(C_LANG_FILES
  ${GCC_SUB_SOURCE_DIR}/c-lang.c          ${GCC_SUB_SOURCE_DIR}/c-tree.h
  ${GCC_SUB_SOURCE_DIR}/c-decl.c          ${GCC_SUB_SOURCE_DIR}/c-family/c-common.c
  ${GCC_SUB_SOURCE_DIR}/c-family/c-common.h ${GCC_SUB_SOURCE_DIR}/c-family/c-objc.h
  ${GCC_SUB_SOURCE_DIR}/c-family/c-cppbuiltin.c ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.h
  ${GCC_SUB_SOURCE_DIR}/c-family/c-pragma.c ${GCC_SUB_SOURCE_DIR}/c-objc-common.c
  ${GCC_SUB_SOURCE_DIR}/c-parser.c        ${GCC_SUB_SOURCE_DIR}/c-lang.h
)


#####################################

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

#CONFIGURE_FILE(${GCCCONFIG_SOURCE_DIR}/gtyp-gen.h.in
#               ${GCC_BINARY_DIR}/gcc/gtyp-gen.h @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Rules to generate gtype-desc.h and gtype-desc.c.

ADD_EXECUTABLE(gengtype
  gengtype.c gengtype-lex.c gengtype-parse.c
  gengtype-state.c version.c errors.c 
  )

SET_SOURCE_FILES_PROPERTIES(version.c
  PROPERTIES COMPILE_DEFINITIONS "BASEVER=\"${GCC_BASEVER}\";BUGURL=\"${PACKAGE_BUGURL}\";DATESTAMP=\"${GCC_DATESTAMP}\";PKGVERSION=\"${GCC_BASEVER}\";DEVPHASE=\"\";REVISION=\"\"")

ADD_DEPENDENCIES(gengtype genoptions version.c)
TARGET_LINK_LIBRARIES(gengtype iberty cpp intl decnumber)
SET_TARGET_PROPERTIES(gengtype PROPERTIES
    COMPILE_FLAGS "-DGENERATOR_FILE -DYY_NO_UNISTD_H ${LDFLAGS}")

SET(GCC_gengtype_EXE "${GCC_GEN_DIR}/gengtype${GCC_EXE_EXT}")

SET(_GTFILES 
  ${GCC_SOURCE_DIR}/libcpp/include/line-map.h
  ${GCC_SOURCE_DIR}/libcpp/include/cpplib.h
  ${GCC_SOURCE_DIR}/libcpp/include/cpp-id-data.h
  ${GCC_SUB_SOURCE_DIR}/input.h             ${GCC_SUB_SOURCE_DIR}/coretypes.h
  ${GCC_SUB_SOURCE_DIR}/vecprim.h           ${GCC_SUB_SOURCE_DIR}/vecir.h
  ${host_xm_file_list}
  ${GCC_BINARY_DIR}/gcc/options.h
  ${tm_file_list}
  ${GCC_SOURCE_DIR}/gcc/defaults.h
  ${GCC_SOURCE_DIR}/include/hashtab.h
  ${GCC_SOURCE_DIR}/include/splay-tree.h
  ${GCC_SUB_SOURCE_DIR}/bitmap.h            ${GCC_SUB_SOURCE_DIR}/alias.h
  ${GCC_SUB_SOURCE_DIR}/coverage.c          ${GCC_SUB_SOURCE_DIR}/rtl.h
  ${GCC_SUB_SOURCE_DIR}/optabs.h            ${GCC_SUB_SOURCE_DIR}/tree.h
  ${GCC_SUB_SOURCE_DIR}/libfuncs.h          
  ${GCC_SOURCE_DIR}/libcpp/include/symtab.h
  ${GCC_SOURCE_DIR}/include/obstack.h
  ${GCC_SUB_SOURCE_DIR}/real.h              ${GCC_SUB_SOURCE_DIR}/function.h
  ${GCC_SUB_SOURCE_DIR}/insn-addr.h         ${GCC_SUB_SOURCE_DIR}/hwint.h
  ${GCC_SUB_SOURCE_DIR}/fixed-value.h       ${GCC_SUB_SOURCE_DIR}/output.h
  ${GCC_SUB_SOURCE_DIR}/cfgloop.h           ${GCC_SUB_SOURCE_DIR}/cselib.h
  ${GCC_SUB_SOURCE_DIR}/basic-block.h       ${GCC_SUB_SOURCE_DIR}/ipa-ref.h
  ${GCC_SUB_SOURCE_DIR}/cgraph.h            ${GCC_SUB_SOURCE_DIR}/reload.h
  ${GCC_SUB_SOURCE_DIR}/caller-save.c       ${GCC_SUB_SOURCE_DIR}/alias.c
  ${GCC_SUB_SOURCE_DIR}/bitmap.c            ${GCC_SUB_SOURCE_DIR}/cselib.c 
  ${GCC_SUB_SOURCE_DIR}/cgraph.c            ${GCC_SUB_SOURCE_DIR}/ipa-prop.c
  ${GCC_SUB_SOURCE_DIR}/ipa-cp.c            ${GCC_SUB_SOURCE_DIR}/matrix-reorg.c
  ${GCC_SUB_SOURCE_DIR}/dbxout.c            ${GCC_SUB_SOURCE_DIR}/dwarf2out.h
  ${GCC_SUB_SOURCE_DIR}/dwarf2asm.c         ${GCC_SUB_SOURCE_DIR}/dwarf2cfi.c
  ${GCC_SUB_SOURCE_DIR}/dwarf2out.c         ${GCC_SUB_SOURCE_DIR}/tree-vect-generic.c
  ${GCC_SUB_SOURCE_DIR}/dojump.c            ${GCC_SUB_SOURCE_DIR}/emit-rtl.c
  ${GCC_SUB_SOURCE_DIR}/except.h            ${GCC_SUB_SOURCE_DIR}/explow.c 
  ${GCC_SUB_SOURCE_DIR}/expr.c              ${GCC_SUB_SOURCE_DIR}/expr.h
  ${GCC_SUB_SOURCE_DIR}/function.c          ${GCC_SUB_SOURCE_DIR}/except.c
  ${GCC_SUB_SOURCE_DIR}/gcse.c              ${GCC_SUB_SOURCE_DIR}/godump.c
  ${GCC_SUB_SOURCE_DIR}/integrate.c         ${GCC_SUB_SOURCE_DIR}/lists.c
  ${GCC_SUB_SOURCE_DIR}/optabs.c
  ${GCC_SUB_SOURCE_DIR}/profile.c           ${GCC_SUB_SOURCE_DIR}/mcf.c
  ${GCC_SUB_SOURCE_DIR}/reg-stack.c         ${GCC_SUB_SOURCE_DIR}/cfglayout.c
  ${GCC_SUB_SOURCE_DIR}/cfglayout.h         ${GCC_SUB_SOURCE_DIR}/sdbout.c
  ${GCC_SUB_SOURCE_DIR}/stor-layout.c       ${GCC_SUB_SOURCE_DIR}/stringpool.c
  ${GCC_SUB_SOURCE_DIR}/tree.c              ${GCC_SUB_SOURCE_DIR}/varasm.c
  ${GCC_SUB_SOURCE_DIR}/gimple.h            ${GCC_SUB_SOURCE_DIR}/gimple.c
  ${GCC_SUB_SOURCE_DIR}/tree-mudflap.c      ${GCC_SUB_SOURCE_DIR}/tree-flow.h
  ${GCC_SUB_SOURCE_DIR}/tree-ssanames.c     ${GCC_SUB_SOURCE_DIR}/tree-eh.c
  ${GCC_SUB_SOURCE_DIR}/tree-ssa-address.c  ${GCC_SUB_SOURCE_DIR}/tree-cfg.c
  ${GCC_SUB_SOURCE_DIR}/tree-dfa.c          ${GCC_SUB_SOURCE_DIR}/tree-iterator.c
  ${GCC_SUB_SOURCE_DIR}/gimplify.c          ${GCC_SUB_SOURCE_DIR}/tree-chrec.h
  ${GCC_SUB_SOURCE_DIR}/tree-scalar-evolution.c
  ${GCC_SUB_SOURCE_DIR}/tree-ssa-operands.h
  ${GCC_SUB_SOURCE_DIR}/tree-profile.c
  ${GCC_SUB_SOURCE_DIR}/tree-nested.c
  ${GCC_SUB_SOURCE_DIR}/varpool.c
  ${GCC_SUB_SOURCE_DIR}/tree-parloops.c
  ${GCC_SUB_SOURCE_DIR}/omp-low.c
  ${GCC_SUB_SOURCE_DIR}/targhooks.c
  ${GCC_SUB_SOURCE_DIR}/config/${out_file}
  ${GCC_SUB_SOURCE_DIR}/passes.c
  ${GCC_SUB_SOURCE_DIR}/cgraphunit.c
  ${GCC_SUB_SOURCE_DIR}/tree-ssa-propagate.c
  ${GCC_SUB_SOURCE_DIR}/tree-phinodes.c
  ${GCC_SUB_SOURCE_DIR}/lto-symtab.c
  ${GCC_SUB_SOURCE_DIR}/tree-ssa-alias.h
  ${GCC_SUB_SOURCE_DIR}/ipa-prop.h
  ${GCC_SUB_SOURCE_DIR}/trans-mem.c
  ${GCC_SUB_SOURCE_DIR}/lto-streamer.h
  ${GCC_SUB_SOURCE_DIR}/target-globals.h
  ${GCC_SUB_SOURCE_DIR}/ipa-inline.h
  #  [ada]
  #${ADA_LANG_FILES}
  [cp]
  ${CP_LANG_FILES}
  #[fortran]
  #${FORT_LANG_FILES}
  #[go]
  #${GO_LANG_FILES}
  #[java]
  #${JAVA_LANG_FILES}
  #[lto]
  #${LTO_LANG_FILES}
  #[objc]
  #${OBJC_LANG_FILES}
  #[objcp]
  #${OBJCP_LANG_FILES}
  [c]
  ${C_LANG_FILES}
)

FILE(RELATIVE_PATH RELATIVE_SOURCE_PATH ${GCC_BINARY_DIR}/gcc ${GCC_SOURCE_DIR}/gcc)
FILE(RELATIVE_PATH RELATIVE_BINARY_PATH ${GCC_BINARY_DIR}/gcc ${GCC_BINARY_DIR}/gcc)
FILE(RELATIVE_PATH RELATIVE_LIBCPP_PATH ${GCC_BINARY_DIR}/gcc ${GCC_SOURCE_DIR}/libcpp)
FILE(RELATIVE_PATH RELATIVE_INCLUDE_PATH ${GCC_BINARY_DIR}/gcc ${GCC_SOURCE_DIR}/include)
#MESSAGE("RELATIVE_PATH ${RELATIVE_SOURCE_PATH}")
FILE(WRITE ${GCC_BINARY_DIR}/gcc/gtyp-input.list)
FOREACH(_header ${_GTFILES})
#    MESSAGE("INPUT HEADER:  ${_header}")
    STRING(REGEX REPLACE "^${GCC_SOURCE_DIR}/gcc/(.*)$" "${RELATIVE_SOURCE_PATH}/\\1" _path "${_header}")
    STRING(REGEX REPLACE "^${GCC_SOURCE_DIR}/libcpp/(.*)$" "${RELATIVE_LIBCPP_PATH}/\\1" _path "${_header}")
    STRING(REGEX REPLACE "^${GCC_SOURCE_DIR}/include/(.*)$" "${RELATIVE_INCLUDE_PATH}/\\1" _path "${_header}")
    STRING(REGEX REPLACE "^${GCC_BINARY_DIR}/gcc/(.*)$" "\\1" _path "${_path}")
#    MESSAGE("OUTPUT HEADER: ${_path}")
    FILE(APPEND ${GCC_BINARY_DIR}/gcc/gtyp-input.list "${_path}\n")
ENDFOREACH()

ADD_CUSTOM_COMMAND(
    OUTPUT ${GCC_BINARY_DIR}/gcc/gtype.state
    COMMAND ${GCC_gengtype_EXE} 
        -S ${GCC_SOURCE_DIR}/gcc 
        -I ${GCC_BINARY_DIR}/gcc/gtyp-input.list 
        -w ${GCC_BINARY_DIR}/gcc/gtype.state
    DEPENDS gengtype ${GCC_BINARY_DIR}/gcc/gtyp-input.list
)

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/gtype-desc.c ${GCC_BINARY_DIR}/gcc/gtype-desc.h
  COMMAND ${GCC_gengtype_EXE} -r ${GCC_BINARY_DIR}/gcc/gtype.state
  DEPENDS gengtype ${GCC_BINARY_DIR}/gcc/gtype.state
)

#-----------------------------------------------------------------------------
# Rules to generate tree-check.h.

ADD_EXECUTABLE(gencheck gencheck.c)
ADD_DEPENDENCIES(gencheck genoptions
                ${GCC_BINARY_DIR}/gcc/insn-modes.c
                ${GCC_BINARY_DIR}/gcc/insn-modes.h 
                ${GCC_BINARY_DIR}/gcc/gtype-desc.h
                ${GCC_BINARY_DIR}/gcc/insn-flags.h
                )
TARGET_LINK_LIBRARIES(gencheck iberty)
SET_TARGET_PROPERTIES(gencheck PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_gencheck_EXE "${GCC_GEN_DIR}/gencheck${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT tree-check.h
  COMMAND ${GCC_gencheck_EXE} > tree-check.h
  DEPENDS gencheck tree.def c-family/c-common.def gimple.def
          ${LANG_TREE_FILES}
  )

#-----------------------------------------------------------------------------
# Generate bversion.h, plugin-version.h and configargs.h

CONFIGURE_FILE(${GCCCONFIG_SOURCE_DIR}/bversion.h.in
               ${GCC_BINARY_DIR}/gcc/bversion.h)

SET(GCCPLUGIN_VERSION_MAJOR ${BUILDING_GCC_MAJOR})
SET(GCCPLUGIN_VERSION_MINOR ${BUILDING_GCC_MINOR})
SET(GCCPLUGIN_VERSION_PATCHLEVEL ${BUILDING_GCC_PATCHLEVEL})
CONFIGURE_FILE(${GCC_SOURCE_DIR}/config_cmake/plugin-version.h.in
               ${GCC_BINARY_DIR}/gcc/plugin-version.h)

SET(GCC_CONFIG_ARGUMENTS_STR "")
SET(THREAD_FILE "posix")
SET(CONFIGURE_DEFAULT_OPTIONS "{ { \"cpu\", \"generic\" }, { \"arch\", \"x86-64\" } }")

CONFIGURE_FILE(${GCC_SOURCE_DIR}/config_cmake/configargs.h.in
               ${GCC_BINARY_DIR}/gcc/configargs.h)

#-----------------------------------------------------------------------------
# Sources used in several generator executables.

ADD_LIBRARY(gen
  rtl.c read-rtl.c ggc-none.c vec.c
  min-insn-modes.c
  gensupport.c print-rtl.c 
  read-md.c errors.c

  ${GCC_BINARY_DIR}/gcc/min-insn-modes.c
  ${GCC_BINARY_DIR}/gcc/insn-modes.h
  ${GCC_BINARY_DIR}/gcc/tree-check.h
  ${GCC_BINARY_DIR}/gcc/gtype-desc.h
  ${GCC_BINARY_DIR}/gcc/gtyp-input.list
  )
ADD_DEPENDENCIES(gen genoptions ${GCC_BINARY_DIR}/gcc/gtype-desc.h ${GCC_BINARY_DIR}/gcc/insn-flags.h)
TARGET_LINK_LIBRARIES(gen iberty)
SET_TARGET_PROPERTIES(gen PROPERTIES COMPILE_FLAGS "-DGENERATOR_FILE -I${CMAKE_CURRENT_BINARY_DIR}")

#-----------------------------------------------------------------------------
# Rules to generate insn-enums.c.

ADD_EXECUTABLE(genenums genenums.c)
TARGET_LINK_LIBRARIES(genenums gen)
SET_TARGET_PROPERTIES(genenums PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_genenums_EXE "${GCC_GEN_DIR}/genenums${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/insn-enums.c
  COMMAND ${GCC_genenums_EXE} ${MD_FILE} > insn-enums.c
  DEPENDS genenums ${MD_FILE}
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
# Rules to generate target-hooks-def.h, c-family/c-target-hooks-def.h, and
# common/common-target-hooks-def.h

ADD_EXECUTABLE(genhooks genhooks.c)
ADD_DEPENDENCIES(genhooks ${GCC_BINARY_DIR}/gcc/gtype-desc.h ${GCC_BINARY_DIR}/gcc/insn-flags.h)
TARGET_LINK_LIBRARIES(genhooks gen)
SET_TARGET_PROPERTIES(genhooks PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GENHOOK_HEADERS
  target-hooks-def.h
  c-family/c-target-hooks-def.h
  common/common-target-hooks-def.h
  )

SET(GCC_genhooks_EXE "${GCC_GEN_DIR}/genhooks${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/target-hooks-def.h
  COMMAND ${GCC_genhooks_EXE} "Target Hook" > target-hooks-def.h
  DEPENDS genhooks
  )

FILE( MAKE_DIRECTORY ${GCC_BINARY_DIR}/gcc/c-family )
ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/c-family/c-target-hooks-def.h
  COMMAND ${GCC_genhooks_EXE} "C Target Hook" > c-family/c-target-hooks-def.h
  DEPENDS genhooks
  )

FILE( MAKE_DIRECTORY ${GCC_BINARY_DIR}/gcc/common )
ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/common/common-target-hooks-def.h
  COMMAND ${GCC_genhooks_EXE} "Common Target Hook" > common/common-target-hooks-def.h
  DEPENDS genhooks
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
  DEPENDS genconditions ${MD_FILE} ${GENHOOK_HEADERS} tree-check.h
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
    DEPENDS gen${tgt} ${MD_FILE} ${MD_COND} insn-attr.h insn-config.h insn-codes.h
            ${GCC_BINARY_DIR}/gcc/insn-attr-common.h genrtl.h ${GCC_BINARY_DIR}/gcc/insn-flags.h  tm.h-full
    )
ENDFOREACH(tgt)

#-----------------------------------------------------------------------------
# Rules to generate gcov-iov.h.

ADD_EXECUTABLE(gcov_iov gcov-iov.c)
SET_TARGET_PROPERTIES(gcov_iov PROPERTIES COMPILE_FLAGS -DGENERATOR_FILE)

SET(GCC_gcov_iov_EXE "${GCC_GEN_DIR}/gcov_iov${GCC_EXE_EXT}")

ADD_CUSTOM_COMMAND(
  OUTPUT ${GCC_BINARY_DIR}/gcc/gcov-iov.h
  COMMAND ${GCC_gcov_iov_EXE} ${GCC_BASEVER} '' > gcov-iov.h
  DEPENDS gcov_iov
  )

#-----------------------------------------------------------------------------
# Special flags for some source files.

SET(LIBSUBDIR  "${CMAKE_LIBRARY_PATH}/gcc/${target}/${GCC_VERSION}")

INCLUDE(GNUInstallDirs)

SET(PREPROCESSOR_DEFINES
  GCC_INCLUDE_DIR=\"${LIBSUBDIR}/include\"
  FIXED_INCLUDE_DIR=\"${LIBSUBDIR}/include-fixed\"
  GPLUSPLUS_INCLUDE_DIR=\"${GCC_GXX_INCLUDE_DIR}\"
  GPLUSPLUS_INCLUDE_DIR_ADD_SYSROOT=0
  GPLUSPLUS_TOOL_INCLUDE_DIR=\"${GCC_GXX_INCLUDE_DIR}/${target}\"
  GPLUSPLUS_BACKWARD_INCLUDE_DIR=\"${GCC_GXX_INCLUDE_DIR}/backward\"
  LOCAL_INCLUDE_DIR=\"$(local_includedir)\"
  CROSS_INCLUDE_DIR=\"${CROSS_SYSTEM_HEADER_DIR}\"
  TOOL_INCLUDE_DIR=\"${GCC_TOOLDIR}/include\"
  NATIVE_SYSTEM_HEADER_DIR=\"${NATIVE_SYSTEM_HEADER_DIR}\"
  PREFIX=\"$(prefix)/\"
  STANDARD_EXEC_PREFIX=\"${CMAKE_INSTALL_LIBDIR}/gcc/\"
  ${TARGET_SYSTEM_ROOT_DEFINE}
  )

SET_SOURCE_FILES_PROPERTIES(toplev.c
  PROPERTIES COMPILE_DEFINITIONS "TARGET_NAME=\"${target}\"")
SET_SOURCE_FILES_PROPERTIES(cppbuiltin.c
  PROPERTIES COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINES};BASEVER=\"${GCC_BASEVER}\"")
SET_SOURCE_FILES_PROPERTIES(cppdefault.c
  PROPERTIES COMPILE_DEFINITIONS "${PREPROCESSOR_DEFINES}")

#SET_SOURCE_FILES_PROPERTIES(collect2.c
#  PROPERTIES COMPILE_FLAGS "-DTARGET_MACHINE=\"${TARGET}\"")

#-----------------------------------------------------------------------------
# Compiler backend library.

SET(unlibsubdir ../../..)

# Get relative path from LIBDIR to PREFIX
#----------------------------------------
# 1. Strip prefix from libdir
#STRING(REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}" "" _relpath "${CMAKE_INSTALL_LIBDIR}" )
# 2. Strip trailing slash
#STRING(REGEX REPLACE "/$" "" _relpath "${_relpath}" )
# 3. Add leading slash
#STRING(REGEX REPLACE "^[^/]" "/" _relpath "${_relpath}" )
# Replace each directory in path with ../
#STRING(REGEX REPLACE "/[^/]*" "../" _relpath "${_relpath}" )

ADD_DEFINITIONS(
  -DSTANDARD_STARTFILE_PREFIX="${unlibsubdir}"
  -DSTANDARD_EXEC_PREFIX="${CMAKE_INSTALL_LIBDIR}/gcc/"
  -DSTANDARD_LIBEXEC_PREFIX="${CMAKE_INSTALL_LIBEXECDIR}/gcc/"
  -DDEFAULT_TARGET_VERSION="${GCC_VERSION}"
  -DDEFAULT_TARGET_MACHINE="${TARGET}"
  -DSTANDARD_BINDIR_PREFIX="${CMAKE_INSTALL_BINDIR}"
  -DTOOLDIR_BASE_PREFIX="${unlibsubdir}${_relpath}"
  -DPREFIX="${CMAKE_INSTALL_PREFIX}" 
  )

SET(GCC_GXX_INCLUDE_DIR "${LIBSUBDIR}/${unlibsubdir}/include/c++/${GCC_VERSION}")
SET(GCC_TOOLDIR "${LIBSUBDIR}/${unlibsubdir}/${target}")

#DRIVER_DEFINES = \
#  -DSTANDARD_STARTFILE_PREFIX=\"$(unlibsubdir)/\" \
#  -DSTANDARD_EXEC_PREFIX=\"$(libdir)/gcc/\" \
#  -DSTANDARD_LIBEXEC_PREFIX=\"$(libexecdir)/gcc/\" \
#  -DDEFAULT_TARGET_VERSION=\"$(version)\" \
#  -DDEFAULT_TARGET_MACHINE=\"$(target_noncanonical)\" \
#  -DSTANDARD_BINDIR_PREFIX=\"$(bindir)/\" \
#  -DTOOLDIR_BASE_PREFIX=\"$(libsubdir_to_prefix)$(prefix_to_exec_prefix)\" \
#  @TARGET_SYSTEM_ROOT_DEFINE@ \
#  $(VALGRIND_DRIVER_DEFINES) \
#  `test "X$${SHLIB}" = "X" || test "@enable_shared@" != "yes" || echo "-DENABLE_SHARED_LIBGCC"` \
#  -DCONFIGURE_SPECS="\"@CONFIGURE_SPECS@\""

#-----------------------------------------------------------------------------
# Target Arch builtin type defs.


ADD_LIBRARY(backend

  # ${GCC_BINARY_DIR}/gcc/insn-attr.h
  ${GCC_BINARY_DIR}/gcc/insn-attrtab.c
  ${GCC_BINARY_DIR}/gcc/insn-automata.c
  # ${GCC_BINARY_DIR}/gcc/insn-codes.h
  # ${GCC_BINARY_DIR}/gcc/insn-config.h
  ${GCC_BINARY_DIR}/gcc/insn-emit.c
  ${GCC_BINARY_DIR}/gcc/insn-enums.c
  ${GCC_BINARY_DIR}/gcc/insn-extract.c
  # ${GCC_BINARY_DIR}/gcc/insn-flags.h
  ${GCC_BINARY_DIR}/gcc/insn-modes.c
  ${GCC_BINARY_DIR}/gcc/insn-opinit.c
  ${GCC_BINARY_DIR}/gcc/insn-output.c
  ${GCC_BINARY_DIR}/gcc/insn-peep.c
  ${GCC_BINARY_DIR}/gcc/insn-preds.c
  ${GCC_BINARY_DIR}/gcc/insn-recog.c
  # tree-gimple.c
  # GCC allows you to choose a garbage collector. Let's just use these:
  ggc-common.c
  ggc-page.c

  alias.c
  alloc-pool.c
  auto-inc-dec.c
  #backend-print-rtl.c
  #backend-rtl.c
  #backend-vec.c
  bb-reorder.c
  bitmap.c
  bt-load.c
  builtins.c
  caller-save.c
  calls.c
  cfg.c
  cfganal.c
  cfgbuild.c
  cfgcleanup.c
  cfgexpand.c
  cfghooks.c
  cfglayout.c
  cfgloop.c
  cfgloopanal.c
  cfgloopmanip.c
  cfgrtl.c
  cgraph.c
  cgraphbuild.c
  cgraphunit.c
  combine.c
  combine-stack-adj.c
  compare-elim.c
  # conflict.c
  convert.c
  coverage.c
  cppbuiltin.c
  cppdefault.c
  cprop.c
  cse.c
  cselib.c
  data-streamer.c
  data-streamer-in.c
  data-streamer-out.c
  dbxout.c
  dbgcnt.c
  dce.c
  ddg.c
  debug.c
  df-core.c
  df-problems.c
  df-scan.c
  dfp.c
  # diagnostic.c
  dojump.c
  dominance.c
  domwalk.c
  double-int.c
  dse.c
  dwarf2asm.c
  dwarf2cfi.c
  dwarf2out.c
  ebitmap.c
  emit-rtl.c
  et-forest.c
  except.c
  explow.c
  expmed.c
  expr.c
  final.c
  fixed-value.c
  # flow.c
  fold-const.c
  function.c
  fwprop.c
  gcse.c
  # genrtl.c

  gimple.c
  gimple-iterator.c
  gimple-fold.c
  gimple-low.c
  gimple-pretty-print.c
  gimple-streamer-in.c
  gimple-streamer-out.c
  gimplify.c
  godump.c
  #global.c
  graph.c
  graphds.c
  graphite.c
  graphite-blocking.c
  graphite-clast-to-gimple.c
  graphite-cloog-util.c
  graphite-dependences.c
  graphite-flattening.c
  graphite-interchange.c
  graphite-poly.c
  graphite-ppl.c
  graphite-scop-detection.c
  graphite-sese-to-poly.c
  gtype-desc.c

  #${GCC_BINARY_DIR}/gcc/genrtl.h
  ${GCC_BINARY_DIR}/gcc/gcov-iov.h

  haifa-sched.c
  hw-doloop.c
  hwint.c
  #hooks.c
  ifcvt.c
  incpath.c
  init-regs.c
  integrate.c
  internal-fn.c
  #intl.c
  ipa-cp.c
  ipa-split.c
  ipa-inline.c
  ipa-inline-analysis.c
  ipa-inline-transform.c
  ipa-prop.c
  ipa-pure-const.c
  ipa-reference.c
  ipa-ref.c
  #ipa-type-escape.c
  ipa-utils.c
  ipa.c
  ira.c
  ira-build.c
  ira-costs.c
  ira-conflicts.c
  ira-color.c
  ira-emit.c
  ira-lives.c
  jump.c
  #lambda-code.c
  #lambda-mat.c
  #lambda-trans.c
  langhooks.c
  lcm.c
  lists.c
  #local-alloc.c
  loop-doloop.c
  loop-init.c
  loop-invariant.c
  loop-iv.c
  loop-unroll.c
  loop-unswitch.c
  lower-subreg.c
  lto-cgraph.c
  lto-streamer.c
  lto-streamer-in.c
  lto-streamer-out.c
  lto-section-in.c
  lto-section-out.c
  lto-symtab.c
  lto-opts.c
  lto-compress.c
  matrix-reorg.c
  mcf.c
  mode-switching.c
  modulo-sched.c
  omega.c
  omp-low.c
  optabs.c
  options-save.c
  opts-global.c
  #opts-common.c
  #opts.c
  #params.c
  passes.c
  plugin.c
  pointer-set.c
  postreload-gcse.c
  postreload.c
  predict.c
  #pretty-print.c
  print-rtl.c
  print-tree.c
  profile.c
  real.c
  realmpfr.c
  recog.c
  ree.c
  reg-stack.c
  regcprop.c
  reginfo.c
  #regclass.c
  regmove.c
  regrename.c
  regstat.c
  reload.c
  reload1.c
  reorg.c
  resource.c
  rtl-error.c
  #rtl-factoring.c
  rtl.c
  rtlanal.c
  rtlhooks.c
  sbitmap.c
  sched-deps.c
  sched-ebb.c
  sched-rgn.c
  sched-vis.c
  sdbout.c
  sel-sched-ir.c
  sel-sched-dump.c
  sel-sched.c
  sese.c
  #see.c
  simplify-rtx.c
  sparseset.c
  sreal.c
  stack-ptr-mod.c
  statistics.c
  stmt.c
  stor-layout.c
  store-motion.c
  streamer-hooks.c
  stringpool.c
  #struct-equiv.c
  target-globals.c
  targhooks.c
  timevar.c
  toplev.c
  tracer.c
  trans-mem.c
  tree-affine.c
  tree-call-cdce.c
  tree-cfg.c
  tree-cfgcleanup.c
  tree-chrec.c
  tree-complex.c
  tree-data-ref.c
  tree-dfa.c
  tree-diagnostic.c
  tree-dump.c
  tree-eh.c
  tree-emutls.c
  tree-if-conv.c
  tree-inline.c
  tree-into-ssa.c
  tree-iterator.c
  #tree-loop-linear.c
  tree-loop-distribution.c
  tree-nested.c
  tree-nomudflap.c
  tree-nrv.c
  tree-object-size.c
  tree-optimize.c
  tree-outof-ssa.c
  tree-parloops.c
  tree-phinodes.c
  tree-predcom.c
  tree-pretty-print.c
  tree-profile.c
  tree-scalar-evolution.c
  tree-sra.c
  tree-switch-conversion.c
  tree-ssa-address.c
  tree-ssa-alias.c
  tree-ssa-ccp.c
  tree-ssa-coalesce.c
  tree-ssa-copy.c
  tree-ssa-copyrename.c
  tree-ssa-dce.c
  tree-ssa-dom.c
  tree-ssa-dse.c
  tree-ssa-forwprop.c
  tree-ssa-ifcombine.c
  tree-ssa-live.c
  tree-ssa-loop-ch.c
  tree-ssa-loop-im.c
  tree-ssa-loop-ivcanon.c
  tree-ssa-loop-ivopts.c
  tree-ssa-loop-manip.c
  tree-ssa-loop-niter.c
  tree-ssa-loop-prefetch.c
  tree-ssa-loop-unswitch.c
  tree-ssa-loop.c
  tree-ssa-math-opts.c
  tree-ssa-operands.c
  tree-ssa-phiopt.c 
  tree-ssa-phiprop.c 
  tree-ssa-pre.c
  tree-ssa-propagate.c
  tree-ssa-reassoc.c
  tree-ssa-sccvn.c
  tree-ssa-sink.c
  tree-ssa-strlen.c
  tree-ssa-structalias.c
  tree-ssa-tail-merge.c
  tree-ssa-ter.c
  tree-ssa-threadedge.c
  tree-ssa-threadupdate.c
  tree-ssa-uncprop.c
  tree-ssa-uninit.c
  tree-ssa.c
  tree-ssanames.c
  tree-stdarg.c
  tree-streamer.c
  tree-streamer-in.c
  tree-streamer-out.c
  tree-tailcall.c
  #tree-vect-analyze.c
  tree-vect-data-refs.c
  tree-vect-generic.c
  tree-vect-loop.c
  tree-vect-loop-manip.c
  tree-vect-patterns.c
  tree-vect-slp.c
  tree-vect-stmts.c
  #tree-vect-transform.c
  tree-vectorizer.c
  #tree-vn.c
  tree-vrp.c
  tree.c
  value-prof.c
  var-tracking.c
  varasm.c
  varpool.c
  #varray.c
  #version.c
  vmsdbgout.c
  web.c
  xcoffout.c

  ${GCC_SOURCE_DIR}/gcc/config/${out_file}
  ${extra_srcs}
  ${host_hook_srcs}

  #${GCC_BINARY_DIR}/gcc/options.c
  )

ADD_DEPENDENCIES(backend
  generate_${cpu_type}
  )

#-----------------------------------------------------------------------------
# Rules for libcommon.a and libcommon-target.a

SET_SOURCE_FILES_PROPERTIES(intl.c
  COMPILE_FLAGS -DLOCALEDIR="\\\"${LOCALEDIR}\\\""
  )

ADD_LIBRARY(common diagnostic.c pretty-print.c intl.c input.c version.c)
ADD_DEPENDENCIES(common
    genflags     ${GCC_BINARY_DIR}/gcc/insn-flags.h
    genconstants ${GCC_BINARY_DIR}/gcc/insn-constants.h )

ADD_LIBRARY(common-target
  common/config/${cpu_type}/${cpu_type}-common.c prefix.c params.c opts.c
  opts-common.c options.c vec.c hooks.c common/common-targhooks.c)

ADD_DEPENDENCIES(common-target
    ${GCC_BINARY_DIR}/gcc/gtype-desc.h
    ${GCC_BINARY_DIR}/gcc/insn-flags.h 
    ${GCC_BINARY_DIR}/gcc/insn-modes.h 
    ${GCC_BINARY_DIR}/gcc/insn-constants.h )

SET_SOURCE_FILES_PROPERTIES(prefix.c
  COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR} -I${CMAKE_CURRENT_BINARY_DIR}/cp"
  )


SET(BACKEND_SOURCES main.c ${TREEBROWSER})
SET(BACKEND_LIBS backend  # ../libcommon-target.a ../libcommon.a 
    cpp decnumber
    ${MPFR_LIBRARIES} ${GMP_LIBRARIES} ${MPC_LIBRARIES} ${PPL_LIBRARIES} ${CLOOG_LIBRARIES}
    ${HOST_LIBS} ${ZLIB_LIBRARIES}
    CACHE INTERNAL "Backend libraries")

