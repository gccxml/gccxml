#ifndef __SGI_STL_INTERNAL_LOCALE_H_B
#define __SGI_STL_INTERNAL_LOCALE_H_B

#include <stl_config.h>

__STL_BEGIN_NAMESPACE

template <class _CharT> class collate;
template <class _Facet> inline const _Facet& use_facet(const locale& __loc);

__STL_END_NAMESPACE

#include_next <stl_locale.h>

#endif /* __SGI_STL_INTERNAL_LOCALE_H_B */

