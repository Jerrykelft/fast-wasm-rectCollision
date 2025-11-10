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

// 向量
FLOAT_TEMPLATE struct Vec2 {
    FLOAT x, y;
    inline Vec2() {}
    inline Vec2(FLOAT _x, FLOAT _y): x(_x), y(_y) {}
    inline FLOAT dot(Vec2 other) const {return x * other.x + y * other.y;}
};
static inline void _sincos(float x, float *s, float *c) {sincosf(x, s, c);}
static inline void _sincos(double x, double *s, double *c) {sincos(x, s, c);}

#define s1 static_cast<FLOAT>(0.0)
#define c1 static_cast<FLOAT>(1.0)

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

    FLOAT hw1 = w1 * static_cast<FLOAT>(0.5),
          hh1 = h1 * static_cast<FLOAT>(0.5),
          hw2 = w2 * static_cast<FLOAT>(0.5),
          hh2 = h2 * static_cast<FLOAT>(0.5);

    FLOAT s2, c2;
    _sincos(r2 - r1, &s2, &c2);

    Vec4<FLOAT> vaxis[2] = {{c1, s1, c2, -s2}, {s1, c1, s2, c2}};

    Vec4<FLOAT> limit = (vaxis[0] * Vec4<FLOAT>(hw1 + hw2 * c2 - hh2 * s2)).abs() +
                        (vaxis[1] * Vec4<FLOAT>(hh1 + hw2 * s2 + hh2 * c2)).abs();

    Vec4<FLOAT> project = (Vec4<FLOAT>(dx) * vaxis[0] + Vec4<FLOAT>(dy) * vaxis[1]).abs();
    return !(project > limit).any_true();
}


FLOAT_TEMPLATE static inline bool __rectCollision(
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

    FLOAT s2, c2;
    _sincos(r2 - r1, &s2, &c2);

    Vec2<FLOAT> axis[4] = {{c1, s1}, {s1, c1}, {c2, s2}, {-s2, c2}};
    Vec2<FLOAT> D(dx, dy);
    FLOAT hw1 = w1 * static_cast<FLOAT>(0.5),
          hh1 = h1 * static_cast<FLOAT>(0.5),
          hw2 = w2 * static_cast<FLOAT>(0.5),
          hh2 = h2 * static_cast<FLOAT>(0.5);
    for (int i = 0; i < 4; ++i) {
        FLOAT r = abs(w1 * axis[i].dot(axis[0])) +
                  abs(h1 * axis[i].dot(axis[1])) +
                  abs(w2 * axis[i].dot(axis[2])) +
                  abs(h2 * axis[i].dot(axis[3]));
        if (abs(D.dot(axis[i])) > r) return false;
    }
    return true;
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
