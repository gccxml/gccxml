#ifndef GCCXML_PTHREAD_H
#define GCCXML_PTHREAD_H

/* Some pthread.h files use __thread, which is a keyword for GCC 3.3.
   Change the name for the duration of including pthread.h.  */
#ifndef __thread
# define __thread gccxml__thread
# define gccxml__thread gccxml__thread
#endif
#include_next <pthread.h>
#ifdef gccxml__thread
# undef gccxml__thread
# undef __thread
#endif

#endif
