#ifndef __SGI_STL_INTERNAL_LOCALE_H_GCCXML
#define __SGI_STL_INTERNAL_LOCALE_H_GCCXML

#include <stl_config.h>

__STL_BEGIN_NAMESPACE

class locale;
template <class _CharT> class collate;
template <class _Facet> inline const _Facet& use_facet(const locale& __loc);

__STL_END_NAMESPACE

#include_next <stl_locale.h>

#endif

