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
double      __builtin_acos(double);
float       __builtin_acosf(float);
long double __builtin_acosl(long double);
double      __builtin_asin(double);
float       __builtin_asinf(float);
long double __builtin_asinl(long double);
double      __builtin_atan(double);
double      __builtin_atan2(double, double);
float       __builtin_atan2f(float, float);
long double __builtin_atan2l(long double, long double);
float       __builtin_atanf(float);
long double __builtin_atanl(long double);
double      __builtin_ceil(double);
float       __builtin_ceilf(float);
long double __builtin_ceill(long double);
double      __builtin_cos(double);
float       __builtin_cosf(float);
double      __builtin_cosh(double);
float       __builtin_coshf(float);
long double __builtin_coshl(long double);
long double __builtin_cosl(long double);
double      __builtin_exp(double);
float       __builtin_expf(float);
long double __builtin_expl(long double);
double      __builtin_fabs(double);
float       __builtin_fabsf(float);
long double __builtin_fabsl(long double);
double      __builtin_floor(double);
float       __builtin_floorf(float);
long double __builtin_floorl(long double);
float       __builtin_fmodf(float, float);
long double __builtin_fmodl(long double, long double);
double      __builtin_frexp(double, int*);
float       __builtin_frexpf(float, int*);
long double __builtin_frexpl(long double, int*);
double      __builtin_ldexp(double, int);
float       __builtin_ldexpf(float, int);
long double __builtin_ldexpl(long double, int);
double      __builtin_log(double);
double      __builtin_log10(double);
float       __builtin_log10f(float);
long double __builtin_log10l(long double);
float       __builtin_logf(float);
long double __builtin_logl(long double);
float       __builtin_modff(float, float*);
long double __builtin_modfl(long double, long double*);
float       __builtin_powf(float, float);
long double __builtin_powl(long double, long double);
double      __builtin_sin(double);
float       __builtin_sinf(float);
double      __builtin_sinh(double);
float       __builtin_sinhf(float);
long double __builtin_sinhl(long double);
long double __builtin_sinl(long double);
double      __builtin_sqrt(double);
float       __builtin_sqrtf(float);
long double __builtin_sqrtl(long double);
double      __builtin_tan(double);
float       __builtin_tanf(float);
double      __builtin_tanh(double);
float       __builtin_tanhf(float);
long double __builtin_tanhl(long double);
long double __builtin_tanl(long double);
int         __builtin_ctz(int);
int         __builtin_ctzl(long);
int         __builtin_ctzll(long long);
int         __builtin_popcount(int);
int         __builtin_popcountl(long);
int         __builtin_popcountll(long long);
