#ifndef __SIMD_H
#define __SIMD_H

#include <wasm_simd128.h>

#ifdef __INTELLISENSE__
// 這段只給 VSCode IntelliSense 看
v128_t __builtin_wasm_shuffle_i8x16(v128_t, v128_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
#endif

typedef struct {v128_t low, high;} v256_t;
typedef __i8x16 i8x16;
typedef __u8x16 u8x16;
typedef __i16x8 i16x8;
typedef __u16x8 u16x8;
typedef __i32x4 i32x4;
typedef __u32x4 u32x4;
typedef __i64x2 i64x2;
typedef __u64x2 u64x2;
typedef __f16x8 f16x8;
typedef __f32x4 f32x4;
typedef __f64x2 f64x2;

static inline v256_t wasm_v256_load(const void *ptr) {return (v256_t){.low = wasm_v128_load(ptr), .high = wasm_v128_load((uint8_t*)ptr + sizeof(v128_t))};}
static inline void wasm_v256_store(void *ptr, v256_t v) {wasm_v128_store(ptr, v.low); wasm_v128_store((uint8_t*)ptr + sizeof(v128_t), v.high);}

static inline v256_t wasm_f64x4_splat(double a) {return (v256_t){.low = wasm_f64x2_splat(a), .high = wasm_f64x2_splat(a)};}
#define wasm_f64x4_const_splat(a) (v256_t){.low = wasm_f64x2_const_splat(a), .high = wasm_f64x2_const_splat(a)}
static inline v256_t wasm_f64x4_make(double a, double b, double c, double d) {return (v256_t){.low = wasm_f64x2_make(a, b), .high = wasm_f64x2_make(c, d)};}
#define wasm_f64x4_const(a, b, c, d) (v256_t){.low = wasm_f64x2_const(a, b), .high = wasm_f64x2_const(c, d)}

#define wasm_f64x4_replace_lane(v, i, val) i < 2 ? (v256_t){.low = wasm_f64x2_replace_lane(v.low, i, val), .high = v.high} : (v256_t){.low = v.low, .high = wasm_f64x2_replace_lane(v.high, i, val)}
#define wasm_f64x4_extract_lane(v, i) i < 2 ? wasm_f64x2_extract_lane(v.low, i) : wasm_f64x2_extract_lane(v.high, i)

static inline v256_t wasm_f64x4_add(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_add(a.low, b.low), .high = wasm_f64x2_add(a.high, b.high)};}
static inline v256_t wasm_f64x4_sub(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_sub(a.low, b.low), .high = wasm_f64x2_sub(a.high, b.high)};}
static inline v256_t wasm_f64x4_mul(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_mul(a.low, b.low), .high = wasm_f64x2_mul(a.high, b.high)};}
static inline v256_t wasm_f64x4_div(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_div(a.low, b.low), .high = wasm_f64x2_div(a.high, b.high)};}

static inline v256_t wasm_f64x4_min(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_min(a.low, b.low), .high = wasm_f64x2_min(a.high, b.high)};}
static inline v256_t wasm_f64x4_max(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_max(a.low, b.low), .high = wasm_f64x2_max(a.high, b.high)};}
static inline v256_t wasm_f64x4_pmin(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_pmin(a.low, b.low), .high = wasm_f64x2_pmin(a.high, b.high)};}
static inline v256_t wasm_f64x4_pmax(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_pmax(a.low, b.low), .high = wasm_f64x2_pmax(a.high, b.high)};}
static inline bool wasm_v256_any_true(v256_t a) {return wasm_v128_any_true(a.low) || wasm_v128_any_true(a.high);}
#define wasm_i64x4_shuffle(a, b, c0, c1, c2, c3) (v256_t){.low = wasm_i64x2_shuffle(a.low, b.high, c0, c1), .high = wasm_i64x2_shuffle(a.low, b.high, c2, c3)}

static inline v256_t wasm_f64x4_eq(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_eq(a.low, b.low), .high = wasm_f64x2_eq(a.high, b.high)};}
static inline v256_t wasm_f64x4_ne(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_ne(a.low, b.low), .high = wasm_f64x2_ne(a.high, b.high)};}
static inline v256_t wasm_f64x4_le(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_le(a.low, b.low), .high = wasm_f64x2_le(a.high, b.high)};}
static inline v256_t wasm_f64x4_ge(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_ge(a.low, b.low), .high = wasm_f64x2_ge(a.high, b.high)};}
static inline v256_t wasm_f64x4_lt(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_lt(a.low, b.low), .high = wasm_f64x2_lt(a.high, b.high)};}
static inline v256_t wasm_f64x4_gt(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_gt(a.low, b.low), .high = wasm_f64x2_gt(a.high, b.high)};}

#ifdef __cplusplus

#ifdef USE_DEFAULT_VEC4

#endif

extern "C" {
    void wasm_f32x4_sincos(v128_t x, v128_t *s, v128_t *c);
    v128_t wasm_f32x4_sin(v128_t x);
    v128_t wasm_f32x4_cos(v128_t x);
    void wasm_f64x4_sincos(v256_t x, v256_t *s, v256_t *c);
}

#else

#define USE_SSE3
#define SSE_MATHFUN_WITH_CODE
#include "sse_mathfun.h"

void wasm_f32x4_sincos(v128_t x, v128_t *s, v128_t *c) {
    sincos_ps((v4sf)x, (v4sf*)s, (v4sf*)c);
}
v128_t wasm_f32x4_sin(v128_t x) {
    return sin_ps((v4sf)x);
}
v128_t wasm_f32x4_cos(v128_t x) {
    return cos_ps((v4sf)x);
}
void wasm_f64x4_sincos(v256_t x, v256_t *s, v256_t *c) {
    sincos_pd((v2df)x.low, (v2df*)&s->low, (v2df*)&c->low);
    sincos_pd((v2df)x.high, (v2df*)&s->high, (v2df*)&c->high);
}

#endif // __cplusplus


#endif // __SIMD_H
