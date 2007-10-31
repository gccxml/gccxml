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

extra_objs="${host_extra_objs} ${extra_objs}"
extra_gcc_objs="${host_extra_gcc_objs} ${extra_gcc_objs}"

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
SET(target_cpu_default ${target_cpu_default})
SET(out_host_hook_obj ${out_host_hook_obj})
EOF

"${cmake_command}" -E copy_if_different "${outfile}.in" "${outfile}"
rm -f "${outfile}.in"
