# Install ./ header file wrappers.
FOREACH(f
    algorithm bitset cctype clocale cmath complex csetjmp csignal cstdarg
    cstddef cstdio cstdlib cstring ctime cwchar cwctype deque exception
    fstream functional iomanip iosfwd iostream iterator list map memory
    new numeric queue set sstream stack stdexcept string strstream
    typeinfo utility valarray vector
    )
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/2.95 FILES
    ${GCCXML_SUPPORT}/GCC/2.95/${f})
ENDFOREACH(f)

# Install std/ header file wrappers.
FOREACH(f
    complext.h gslice_array.h
    indirect_array.h mask_array.h slice_array.h
    std_valarray.h valarray_meta.h
    )
  INSTALL_FILES(${GCCXML_INSTALL_ROOT}${GCCXML_DATA_DIR}/GCC/2.95/std FILES
    ${GCCXML_SUPPORT}/GCC/2.95/std/${f})
ENDFOREACH(f)
