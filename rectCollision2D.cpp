#include <cmath>
#include <array>
#include <concepts>

#define USE_DEFAULT_VEC4
#include "simd.c"

#define inline __attribute__((__always_inline__)) inline

extern "C" {
    void sincosf(float x, float *s, float *c);
    void sincos(double x, double *s, double *c);
}
static inline void sincos(float x, float *s, float *c) {sincosf(x, s, c);}


template <typename FLOAT>
requires std::floating_point<FLOAT>
struct Vec2 {
    FLOAT x, y;
    inline Vec4<FLOAT> dot(std::array<Vec4<FLOAT>, 2> other) const {return Vec4<FLOAT>(x) * other[0] + Vec4<FLOAT>(y) * other[1];}
};

template <typename FLOAT>
requires std::floating_point<FLOAT>
static inline bool _rectCollision(
    double x1, double y1, FLOAT w1, FLOAT h1, FLOAT r1,
    double x2, double y2, FLOAT w2, FLOAT h2, FLOAT r2
) {
    FLOAT dx = x2 - x1, dy = y2 - y1;
    if (abs(r1) > static_cast<FLOAT>(1e-5)) {
        FLOAT s, c;
        sincos(-r1, &s, &c);
        FLOAT _dx = dx, _dy = dy;
        dx = _dx * c - _dy * s;
        dy = _dx * s + _dy * c;
    }

    FLOAT hw1 = w1 * static_cast<FLOAT>(0.5),
          hh1 = h1 * static_cast<FLOAT>(0.5),
          hw2 = w2 * static_cast<FLOAT>(0.5),
          hh2 = h2 * static_cast<FLOAT>(0.5);

    FLOAT s2, c2;
    sincos(r2 - r1, &s2, &c2);

    Vec2<FLOAT> axis[2] = {{c2, s2}, {-s2, c2}};
    std::array<Vec4<FLOAT>, 2> vaxis = {
        (Vec4<FLOAT>){static_cast<FLOAT>(1.0), static_cast<FLOAT>(0.0), c2, -s2},
        (Vec4<FLOAT>){static_cast<FLOAT>(0.0), static_cast<FLOAT>(1.0), s2, c2}
    };

    Vec4<FLOAT> limit = (Vec4<FLOAT>(hw1) * vaxis[0]).abs() +
                        (Vec4<FLOAT>(hh1) * vaxis[1]).abs() +
                        (Vec4<FLOAT>(hw2) * axis[0].dot(vaxis)).abs() +
                        (Vec4<FLOAT>(hh2) * axis[1].dot(vaxis)).abs();

    Vec4<FLOAT> project = (Vec4<FLOAT>(dx) * vaxis[0] + Vec4<FLOAT>(dy) * vaxis[1]).abs();
    return !(project > limit).any_true();
}

extern "C" {
    bool rectCollisionf(
        double x1, double y1, float w1, float h1, float r1,
        double x2, double y2, float w2, float h2, float r2
    ) {
        return _rectCollision<float>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
    }
    bool rectCollision(
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
