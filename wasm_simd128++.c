#define USE_SSE3
#define SSE_MATHFUN_WITH_CODE
#include <wasm_simd128.h>
#include "sse_mathfun.h"

void wasm_f32x4_sincos(v128_t x, v128_t *s, v128_t *c) {
    return sincos_ps((v4sf)x, (v4sf*)s, (v4sf*)c);
}
v128_t wasm_f32x4_sin(v128_t x) {
    return sin_ps((v4sf)x);
}
v128_t wasm_f32x4_cos(v128_t x) {
    return cos_ps((v4sf)x);
}
