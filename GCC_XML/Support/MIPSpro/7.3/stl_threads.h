#ifndef __SGI_STL_INTERNAL_THREADS_H_GCCXML
#define __SGI_STL_INTERNAL_THREADS_H_GCCXML

#define __add_and_fetch(__l,__v) add_then_test((unsigned long*)__l,__v)  
#define __test_and_set(__l,__v)  test_and_set(__l,__v)
#define __lock_release(__l) asm("sync"); *__l = 0

#include_next <stl_threads.h>

#endif

