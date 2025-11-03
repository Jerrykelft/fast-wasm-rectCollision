#include <cmath>
#include <array>

extern "C" {
    void sincosf(float x, float *s, float *c);
    void sincos(double x, double *s, double *c);
}

#ifdef __wasm_simd128__
    #include <wasm_simd128.h>
    #include "sse_mathfun.h"
    extern "C" {
        void wasm_f32x4_sincos(v128_t x, v128_t *s, v128_t *c);
    }
#else
    typedef std::array<float, 4> v128_t;
    v128_t wasm_f32x4_make(float a, float b, float c, float d) {return {a, b, c, d};}
    v128_t wasm_f32x4_const(float a, float b, float c, float d) {return {a, b, c, d};}
    v128_t wasm_f32x4_splat(float v) {return {v, v, v, v};}
    v128_t wasm_f32x4_const_splat(float v) {return {v, v, v, v};}
    float wasm_f32x4_extract_lane(v128_t a, uintptr_t i) {return a[i];}
    v128_t wasm_f32x4_replace_lane(v128_t a, uintptr_t i, float v) {a[i] = v; return a;}
    void wasm_f32x4_sincos(v128_t x, v128_t *s, v128_t *c) {
        sincosf(x[0], (float*)&s[0], (float*)&c[0]);
        sincosf(x[1], (float*)&s[1], (float*)&c[1]);
        sincosf(x[2], (float*)&s[2], (float*)&c[2]);
        sincosf(x[3], (float*)&s[3], (float*)&c[3]);
    }
#endif

#ifdef __INTELLISENSE__
// 這段只給 VSCode IntelliSense 看
v128_t __builtin_wasm_shuffle_i8x16(v128_t, v128_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
#endif

// v128_t wasm_i32x4_shuffle_fake(v128_t, v128_t, int, int, int, int);
// #define wasm_i32x4_shuffle(a, b, c, d, e, f) wasm_i32x4_shuffle_fake(a, b, c, d, e, f)

static inline std::array<float, 2> sincos(float x) {float s, c; sincosf(x, &s, &c); return {s, c};}
static inline std::array<double, 2> sincos(double x) {double s, c; sincos(x, &s, &c); return {s, c};}

#define VECTOR_METHOD(T) \
v128_t data; \
inline T(): data(wasm_f32x4_const_splat(0.0f)) {} \
inline T(v128_t v): data(v) {} \
inline T(float splat): data(wasm_f32x4_splat(splat)) {} \
inline float x() const {return wasm_f32x4_extract_lane(data, 0);} \
inline T& x(float x) {data = wasm_f32x4_replace_lane(data, 0, x); return *this;} \
inline float y() const {return wasm_f32x4_extract_lane(data, 1);} \
inline T& y(float y) {data = wasm_f32x4_replace_lane(data, 1, y); return *this;} \
inline float z() const {return wasm_f32x4_extract_lane(data, 2);} \
inline T& z(float z) {data = wasm_f32x4_replace_lane(data, 2, z); return *this;} \
inline T& operator=(T other) {data = other.data; return *this;} \
inline T& operator=(v128_t v) {data = v; return *this;} \
inline T operator+(T other) const {return T(wasm_f32x4_add(data, other.data));} \
inline T operator-(T other) const {return T(wasm_f32x4_sub(data, other.data));} \
inline T operator*(T other) const {return T(wasm_f32x4_mul(data, other.data));} \
inline T operator/(T other) const {return T(wasm_f32x4_div(data, other.data));} \
inline T& operator+=(T other) {data = wasm_f32x4_add(data, other.data); return *this;} \
inline T& operator-=(T other) {data = wasm_f32x4_sub(data, other.data); return *this;} \
inline T& operator*=(T other) {data = wasm_f32x4_mul(data, other.data); return *this;} \
inline T& operator/=(T other) {data = wasm_f32x4_div(data, other.data); return *this;} \
inline T operator+(v128_t v) const {return T(wasm_f32x4_add(data, v));} \
inline T operator-(v128_t v) const {return T(wasm_f32x4_sub(data, v));} \
inline T operator*(v128_t v) const {return T(wasm_f32x4_mul(data, v));} \
inline T operator/(v128_t v) const {return T(wasm_f32x4_div(data, v));} \
inline T& operator+=(v128_t v) {data = wasm_f32x4_add(data, v); return *this;} \
inline T& operator-=(v128_t v) {data = wasm_f32x4_sub(data, v); return *this;} \
inline T& operator*=(v128_t v) {data = wasm_f32x4_mul(data, v); return *this;} \
inline T& operator/=(v128_t v) {data = wasm_f32x4_div(data, v); return *this;}

struct Vec3 {
    inline Vec3(float x, float y, float z): data(wasm_f32x4_make(x, y, z, 0.0f)) {}
    VECTOR_METHOD(Vec3)
    inline float sum() {return x() + y() + z();}
};
struct Vec4 {
    inline Vec4(float x, float y, float z, float w): data(wasm_f32x4_make(x, y, z, w)) {}
    inline float w() const {return wasm_f32x4_extract_lane(data, 3);}
    inline Vec4& w(float w) {data = wasm_f32x4_replace_lane(data, 3, w); return *this;}
    VECTOR_METHOD(Vec4)
    inline float sum() {return x() + y() + z() + w();}
};
struct Quat {
    inline Quat(float x, float y, float z, float w): data(wasm_f32x4_make(x, y, z, w)) {}
    inline float w() const {return wasm_f32x4_extract_lane(data, 3);}
    inline Quat& w(float w) {data = wasm_f32x4_replace_lane(data, 3, w); return *this;}
    VECTOR_METHOD(Quat)
    inline float sum() {return x() + y() + z() + w();}

    inline Quat operator^(Quat other) const {
        Quat vq1x = Quat(x()) * wasm_f32x4_const(1.0f, -1.0f, 1.0f, -1.0f);
        Quat vq1y = Quat(y()) * wasm_f32x4_const(1.0f, 1.0f, -1.0f, -1.0f);
        Quat vq1z = Quat(z()) * wasm_f32x4_const(-1.0f, 1.0f, 1.0f, -1.0f);
        Quat vq1w(w());
        return vq1x * wasm_i32x4_shuffle(other.data, other.data, 3, 2, 1, 0) +
               vq1y * wasm_i32x4_shuffle(other.data, other.data, 2, 3, 0, 1) +
               vq1z * wasm_i32x4_shuffle(other.data, other.data, 1, 0, 3, 2) +
               vq1w * other.data;
    }
    inline Quat operator^(Vec3 other) const {return *this ^ Quat((other * wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)).data);}
    inline Quat& operator^=(Quat other) {return *this = *this ^ other;}
    inline Quat& operator^=(Vec3 other) {return *this = *this ^ Quat((other * wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)).data);}
};
typedef std::array<Vec3, 8> RectVertex;


// 四元數旋轉向量
static inline Vec3 _quatRotateVec(Quat q, Vec3 v) {return Vec3(((q ^ v) ^ (q * wasm_f32x4_const(-1.0f, -1.0f, -1.0f, 1.0f))).data);}

static inline Vec3 _quatRotatePointAroundCenter(Vec3 p, Vec3 center, Quat q) {return _quatRotateVec(q, p - center) + center;}

// 計算 OBB 八個頂點
static inline RectVertex _getOBB(Vec3 center, Vec3 halfSize, Quat rotation) {
    v128_t constant[8] = {
        wasm_f32x4_const(-1.0f, -1.0f, -1.0f, 0.0f),
        wasm_f32x4_const(1.0f, -1.0f, -1.0f, 0.0f),
        wasm_f32x4_const(-1.0f, 1.0f, -1.0f, 0.0f),
        wasm_f32x4_const(1.0f, 1.0f, -1.0f, 0.0f),
        wasm_f32x4_const(-1.0f, -1.0f, 1.0f, 0.0f),
        wasm_f32x4_const(1.0f, -1.0f, 1.0f, 0.0f),
        wasm_f32x4_const(-1.0f, 1.0f, 1.0f, 0.0f),
        wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)
    };
    RectVertex result;
    for (int i = 0; i < 8; i++) {
        // 遍歷每個頂點符號組合: 0->-x, 1->+x
        Vec3 local = halfSize * constant[i];
        Vec3 rotated = _quatRotateVec(rotation, local);
        result[i] += rotated;
    }
    return result;
}

static inline Quat _eulerToQuat(Vec3 v) {
    v *= wasm_f32x4_const_splat(0.5f);
    float x = v.x(), y = v.y(), z = v.z();
    v128_t cy = wasm_f32x4_splat(cos(z)),
           sy = wasm_f32x4_splat(sin(z)),
           cp = wasm_f32x4_splat(cos(y)),
           sp = wasm_f32x4_splat(sin(y)),
           cr = wasm_f32x4_splat(cos(x)),
           sr = wasm_f32x4_splat(sin(x));
    Quat quat = Quat(wasm_i32x4_shuffle(cr, sr, 4, 1, 2, 3)) * Quat(wasm_i32x4_shuffle(cp, sp, 0, 5, 2, 3)) * Quat(wasm_i32x4_shuffle(cy, sy, 0, 1, 6, 3)) + 
                Quat(wasm_i32x4_shuffle(cr, sr, 0, 5, 6, 7)) * Quat(wasm_i32x4_shuffle(cp, sp, 4, 1, 6, 7)) * Quat(wasm_i32x4_shuffle(cy, sy, 4, 5, 2, 7)) * wasm_f32x4_const(-1.0f, 1.0f, -1.0f, 1.0f);
    // quat /= wasm_f32x4_splat(sqrt((quat * quat).sum()));
    return quat;
}
static inline Quat _eulerToQuat(float x, float y, float z) {
    x *= 0.5f;
    y *= 0.5f;
    z *= 0.5f;
    float cy = cos(z),
          sy = sin(z),
          cp = cos(y),
          sp = sin(y),
          cr = cos(x),
          sr = sin(x);
    Quat quat(
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy
    );
    // quat /= wasm_f32x4_splat(sqrt((quat * quat).sum()));
    return quat;
}

static v4sf r1;
static v128_t r2;

extern "C" {
    // 測試
    // float test(float x, float y, float z, float w, float h, float l, float r0, float r1, float r2, float r3) {
    //     // Quat rot(0, 0, 0.38268343f, 0.9238795f); // 繞Z旋轉 45度
    //     RectVertex vertices = _getOBB(Vec3(x, y, z), Vec3(w, h, l), Quat(r0, r1, r2, r3));
    //     float t = 0.0f;
    //     for (int i = 0; i < 8; i++) {
    //         t += vertices[i].sum();
    //     }
    //     return t;
    // }
    void* test2(float a, float b, float c, float d) {
        static float result[8];
        v128_t r1, r2;
        wasm_f32x4_sincos(wasm_f32x4_make(a, b, c, d), &r1, &r2);
        wasm_v128_store(result, r1);
        wasm_v128_store(result + 4, r2);
        return result;
    }
    void test3(float a, float b, float c, float d) {
        r1 = _mm_load_ps((float[]){a, b, c, d});
        r2 = (v128_t)r1;
        // static float result[8];
        // sincosf(a, result + 0, result + 4);
        // sincosf(b, result + 1, result + 5);
        // sincosf(c, result + 2, result + 6);
        // sincosf(d, result + 3, result + 7);
        // return result;
    }
    void* a() {
        return &r1;
    }
    void* b() {
        return &r2;
    }
}


/*

wasm2wat rectCollision3D.wasm -o rectCollision3D.wat
wasm-opt rectCollision3D.wasm -o rectCollision3D.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce
*/
