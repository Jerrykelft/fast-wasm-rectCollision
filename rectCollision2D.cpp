#include <cmath>
#include <array>
#include <concepts>

#define USE_DEFAULT_VEC4
#include "simd.c"

extern "C" {
    void sincosf(float x, float *s, float *c);
    void sincos(double x, double *s, double *c);
}

#define FLOAT_TEMPLATE \
template <typename FLOAT> \
requires std::floating_point<FLOAT>

FLOAT_TEMPLATE struct Vec2x4;

// 向量
FLOAT_TEMPLATE struct Vec2 {
    FLOAT x, y;
    inline Vec2() {}
    inline Vec2(FLOAT _x, FLOAT _y): x(_x), y(_y) {}
    inline FLOAT dot(Vec2 other) const {return x * other.x + y * other.y;}
    inline Vec4<FLOAT> dot(Vec2x4<FLOAT> other) const {return Vec4<FLOAT>(x) * other.data.vector.vx + Vec4<FLOAT>(y) * other.data.vector.vy;}
};
template <>
struct Vec2x4<float> {
    union {
        struct {v128_t vx, vy;} vector;
        struct {std::array<float, 4> x, y;} scalar;
    } data;
    inline Vec2x4() {}
    inline Vec2x4(float x, float y) {data.vector.vx = wasm_f32x4_splat(x); data.vector.vy = wasm_f32x4_splat(y);}
    inline Vec2x4(std::array<float, 4> x, std::array<float, 4> y) {data.scalar.x = x; data.scalar.y = y;}
    inline Vec2x4(v128_t x, v128_t y) {data.vector.vx = x; data.vector.vy = y;}
    inline Vec2x4(Vec4<float> x, Vec4<float> y) {data.vector.vx = x.data.vector; data.vector.vy = y.data.vector;}
};
template <>
struct Vec2x4<double> {
    union {
        struct {v256_t vx, vy;} vector;
        struct {std::array<double, 4> x, y;} scalar;
    } data;
    inline Vec2x4() {}
    inline Vec2x4(double x, double y) {data.vector.vx = wasm_f64x4_splat(x); data.vector.vy = wasm_f64x4_splat(y);}
    inline Vec2x4(std::array<double, 4> x, std::array<double, 4> y) {data.scalar.x = x; data.scalar.y = y;}
    inline Vec2x4(v256_t x, v256_t y) {data.vector.vx = x; data.vector.vy = y;}
    inline Vec2x4(Vec4<double> x, Vec4<double> y) {data.vector.vx = x.data.vector; data.vector.vy = y.data.vector;}
};

static inline void _sincos(float x, float *s, float *c) {sincosf(x, s, c);}
static inline void _sincos(double x, double *s, double *c) {sincos(x, s, c);}

FLOAT_TEMPLATE static inline bool _rectCollision(
    double x1, double y1, FLOAT w1, FLOAT h1, FLOAT r1,
    double x2, double y2, FLOAT w2, FLOAT h2, FLOAT r2
) {
    FLOAT dx = x2 - x1, dy = y2 - y1;
    if (abs(r1) > static_cast<FLOAT>(1e-5)) {
        FLOAT s, c;
        _sincos(-r1, &s, &c);
        // 旋轉並平移回原位
        dx = dx * c - dy * s;
        dy = dx * s + dy * c;
    }

    FLOAT hw1 = w1 * 0.5f,
          hh1 = h1 * 0.5f,
          hw2 = w2 * 0.5f,
          hh2 = h2 * 0.5f;

    FLOAT s1 = static_cast<FLOAT>(0.0), c1 = static_cast<FLOAT>(1.0), s2, c2;
    _sincos(r2 - r1, &s2, &c2);

    Vec2<FLOAT> axis[4] = {{c1, s1}, {-s1, c1}, {c2, s2}, {-s2, c2}};
    Vec2x4<FLOAT> vaxis = {std::array<FLOAT, 4>{c1, -s1, c2, -s2}, std::array<FLOAT, 4>{s1, c1, s2, c2}};

    Vec4<FLOAT> limit = (Vec4<FLOAT>(hw1) * axis[0].dot(vaxis)).abs() +
                        (Vec4<FLOAT>(hh1) * axis[1].dot(vaxis)).abs() +
                        (Vec4<FLOAT>(hw2) * axis[2].dot(vaxis)).abs() +
                        (Vec4<FLOAT>(hh2) * axis[3].dot(vaxis)).abs();
    return !(Vec2<FLOAT>(dx, dy).dot(vaxis).abs() > limit).any_true();
}

extern "C" {
#ifdef __FAST_MATH__
    bool rectCollisionF32fast
#else
    bool rectCollisionF32
#endif
    (
        double x1, double y1, float w1, float h1, float r1,
        double x2, double y2, float w2, float h2, float r2
    ) {
        return _rectCollision<float>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
    }
#ifdef __FAST_MATH__
    bool rectCollisionF64fast
#else
    bool rectCollisionF64
#endif
    (
        double x1, double y1, double w1, double h1, double r1,
        double x2, double y2, double w2, double h2, double r2
    ) {
        return _rectCollision<double>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
    }
}

/*
wat2wasm rectCollision2D.wat -o rectCollision2D.wasm
wasm-opt rectCollision2D.wasm -o rectCollision2D.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce
wasm2wat rectCollision2D.wasm -o rectCollision2D.wat
*/
