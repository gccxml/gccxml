#define __builtin_apply(x,y,z) ((void*)0)
#define __builtin_nan(x) (0.0)
#define __builtin_nanf(x) (0.0f)
#define __builtin_nanl(x) (0.0l)
#define __builtin_huge_val(x) (0.0)
#define __builtin_huge_valf(x) (0.0f)
#define __builtin_huge_vall(x) (0.0l)
#define __builtin_apply_args(x) ((void*)0)
#define __builtin_types_compatible_p(x,y) 0
#define __builtin_choose_expr(x,y,z) int
#define __builtin_constant_p(x) 0
void __builtin_return (void *RESULT);
void * __builtin_return_address (unsigned int LEVEL);
void * __builtin_frame_address (unsigned int LEVEL);
long __builtin_expect (long EXP, long C);
void __builtin_prefetch (const void *ADDR, ...);
double __builtin_inf (void);
float __builtin_inff (void);
long double __builtin_infl (void);
double __builtin_nans (const char *str);
float __builtin_nansf (const char *str);
long double __builtin_nansl (const char *str);
