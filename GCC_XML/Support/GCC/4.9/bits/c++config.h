#ifdef __LONG_DOUBLE_128__
/* Avoid using inline namespace not supported by GCC 4.2 */
#undef __LONG_DOUBLE_128__
#include_next <bits/c++config.h>
#define __LONG_DOUBLE_128__ 1
#else
#include_next <bits/c++config.h>
#endif

/* GCC 4.2 parser does not support __int128 */
#undef _GLIBCXX_USE_INT128
