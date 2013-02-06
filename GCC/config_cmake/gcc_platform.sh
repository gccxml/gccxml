GCC_SOURCE_DIR=`cd "\`echo \"$0\" | sed -n '/\//{s/\/[^\/]*$//;p;}'\`";cd ..;pwd`
srcdir="${GCC_SOURCE_DIR}/gcc"

outfile="$1"
cmake_command="$2"

# Determine the host platforms.
config_sub="${GCC_SOURCE_DIR}/config.sub"
host_alias=`"${GCC_SOURCE_DIR}/config.guess"`
host=`"${GCC_SOURCE_DIR}/config.sub" $host_alias`
target="${host}"

# Collect build-machine-specific information.
. "${GCC_SOURCE_DIR}/gcc/config.build"

# Collect host-machine-specific information.
. "${GCC_SOURCE_DIR}/gcc/config.host"

target_gtfiles=

# Collect target-machine-specific information.
. "${GCC_SOURCE_DIR}/gcc/config.gcc"

C_TARGET_SOURCES=
for obj in ${c_target_objs} ; do
    C_TARGET_SOURCES="$(echo ${obj} | sed 's/\.o$/\.c/') ${C_TARGET_SOURCES}"
done
CXX_TARGET_SOURCES=
for obj in ${cxx_target_objs} ; do
    CXX_TARGET_SOURCES="$(echo ${obj} | sed 's/\.o$/\.c/') ${CXX_TARGET_SOURCES}"
done

extra_objs="${host_extra_objs} ${extra_objs}"
extra_gcc_objs="${host_extra_gcc_objs} ${extra_gcc_objs}"

# The file host-ppc-darwin.o is built from rs6000/host-darwin.c in the
# gcc makefile system
fix_out_host_hook_obj()
{
  echo "$1" | sed 's/host-ppc-darwin/rs6000\/host-darwin/'
}
out_host_hook_obj=`fix_out_host_hook_obj "${out_host_hook_obj}"`

# Report the information back to the CMake process.
cat > "${outfile}.in" <<EOF
SET(target ${host})
SET(cpu_type ${cpu_type})
SET(tm_defines ${tm_defines})
SET(tm_file ${tm_file})
SET(xm_defines ${xm_defines})
SET(xm_file ${xm_file})
SET(host_xm_defines ${host_xm_defines})
SET(host_xm_file ${host_xm_file})
SET(build_xm_defines ${build_xm_defines})
SET(build_xm_file ${build_xm_file})
SET(tm_p_file ${tm_p_file})
SET(extra_modes ${extra_modes})
SET(extra_objs ${extra_objs})
SET(extra_options ${extra_options})
SET(c_target_objs ${c_target_objs})
SET(cxx_target_objs ${cxx_target_objs})
SET(C_TARGET_SOURCES ${C_TARGET_SOURCES})
SET(CXX_TARGET_SOURCES ${CXX_TARGET_SOURCES})
SET(target_cpu_default "${target_cpu_default}")
SET(out_host_hook_obj ${out_host_hook_obj})
EOF

"${cmake_command}" -E copy_if_different "${outfile}.in" "${outfile}"
rm -f "${outfile}.in"
