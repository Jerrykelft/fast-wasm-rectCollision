#ifndef __SIMD_H
#define __SIMD_H

#include <wasm_simd128.h>

#ifdef __INTELLISENSE__
// 這段只給 VSCode IntelliSense 看
v128_t __builtin_wasm_shuffle_i8x16(v128_t, v128_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
#endif

typedef struct {v128_t low, high;} v256_t;

static inline v256_t wasm_v256_load(const void *ptr) {return (v256_t){.low = wasm_v128_load(ptr), .high = wasm_v128_load((uint8_t*)ptr + sizeof(v128_t))};}
static inline void wasm_v256_store(void *ptr, v256_t v) {wasm_v128_store(ptr, v.low); wasm_v128_store((uint8_t*)ptr + sizeof(v128_t), v.high);}
static inline bool wasm_v256_any_true(v256_t a) {return wasm_v128_any_true(a.low) || wasm_v128_any_true(a.high);}
static inline v256_t wasm_v256_not(v256_t a) {return (v256_t){.low = wasm_v128_not(a.low), .high = wasm_v128_not(a.high)};}
static inline v256_t wasm_v256_and(v256_t a, v256_t b) {return (v256_t){.low = wasm_v128_and(a.low, b.low), .high = wasm_v128_and(a.high, b.high)};}
static inline v256_t wasm_v256_andnot(v256_t a, v256_t b) {return (v256_t){.low = wasm_v128_andnot(a.low, b.low), .high = wasm_v128_andnot(a.high, b.high)};}
static inline v256_t wasm_v256_or(v256_t a, v256_t b) {return (v256_t){.low = wasm_v128_or(a.low, b.low), .high = wasm_v128_or(a.high, b.high)};}
static inline v256_t wasm_v256_xor(v256_t a, v256_t b) {return (v256_t){.low = wasm_v128_xor(a.low, b.low), .high = wasm_v128_xor(a.high, b.high)};}
#define wasm_i64x4_shuffle(a, b, c0, c1, c2, c3) (v256_t){.low = wasm_i64x2_shuffle(a.low, b.high, c0, c1), .high = wasm_i64x2_shuffle(a.low, b.high, c2, c3)}

static inline v256_t wasm_f64x4_splat(double a) {return (v256_t){.low = wasm_f64x2_splat(a), .high = wasm_f64x2_splat(a)};}
static inline v256_t wasm_f64x4_make(double a, double b, double c, double d) {return (v256_t){.low = wasm_f64x2_make(a, b), .high = wasm_f64x2_make(c, d)};}
#define wasm_f64x4_const_splat(a) (v256_t){.low = wasm_f64x2_const_splat(a), .high = wasm_f64x2_const_splat(a)}
#define wasm_f64x4_const(a, b, c, d) (v256_t){.low = wasm_f64x2_const(a, b), .high = wasm_f64x2_const(c, d)}
#define wasm_f64x4_replace_lane(v, i, val) i < 2 ? (v256_t){.low = wasm_f64x2_replace_lane(v.low, i, val), .high = v.high} : (v256_t){.low = v.low, .high = wasm_f64x2_replace_lane(v.high, i, val)}
#define wasm_f64x4_extract_lane(v, i) i < 2 ? wasm_f64x2_extract_lane(v.low, i) : wasm_f64x2_extract_lane(v.high, i)

static inline v256_t wasm_i64x4_splat(long long a) {return (v256_t){.low = wasm_i64x2_splat(a), .high = wasm_i64x2_splat(a)};}
static inline v256_t wasm_i64x4_make(long long a, long long b, long long c, long long d) {return (v256_t){.low = wasm_i64x2_make(a, b), .high = wasm_i64x2_make(c, d)};}
#define wasm_i64x4_const_splat(a) (v256_t){.low = wasm_i64x2_const_splat(a), .high = wasm_i64x2_const_splat(a)}
#define wasm_i64x4_const(a, b, c, d) (v256_t){.low = wasm_i64x2_const(a, b), .high = wasm_i64x2_const(c, d)}
#define wasm_i64x4_replace_lane(v, i, val) i < 2 ? (v256_t){.low = wasm_i64x2_replace_lane(v.low, i, val), .high = v.high} : (v256_t){.low = v.low, .high = wasm_i64x2_replace_lane(v.high, i, val)}
#define wasm_i64x4_extract_lane(v, i) i < 2 ? wasm_i64x2_extract_lane(v.low, i) : wasm_i64x2_extract_lane(v.high, i)

static inline v256_t wasm_u64x4_splat(unsigned long long a) {return (v256_t){.low = wasm_u64x2_splat(a), .high = wasm_u64x2_splat(a)};}
static inline v256_t wasm_u64x4_make(unsigned long long a, unsigned long long b, unsigned long long c, unsigned long long d) {return (v256_t){.low = wasm_u64x2_make(a, b), .high = wasm_u64x2_make(c, d)};}
#define wasm_u64x4_const_splat(a) (v256_t){.low = wasm_u64x2_const_splat(a), .high = wasm_u64x2_const_splat(a)}
#define wasm_u64x4_const(a, b, c, d) (v256_t){.low = wasm_u64x2_const(a, b), .high = wasm_u64x2_const(c, d)}
#define wasm_u64x4_replace_lane(v, i, val) i < 2 ? (v256_t){.low = wasm_u64x2_replace_lane(v.low, i, val), .high = v.high} : (v256_t){.low = v.low, .high = wasm_u64x2_replace_lane(v.high, i, val)}
#define wasm_u64x4_extract_lane(v, i) i < 2 ? wasm_u64x2_extract_lane(v.low, i) : wasm_u64x2_extract_lane(v.high, i)

static inline v256_t wasm_f64x4_add(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_add(a.low, b.low), .high = wasm_f64x2_add(a.high, b.high)};}
static inline v256_t wasm_f64x4_sub(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_sub(a.low, b.low), .high = wasm_f64x2_sub(a.high, b.high)};}
static inline v256_t wasm_f64x4_mul(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_mul(a.low, b.low), .high = wasm_f64x2_mul(a.high, b.high)};}
static inline v256_t wasm_f64x4_div(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_div(a.low, b.low), .high = wasm_f64x2_div(a.high, b.high)};}
static inline v256_t wasm_f64x4_eq(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_eq(a.low, b.low), .high = wasm_f64x2_eq(a.high, b.high)};}
static inline v256_t wasm_f64x4_ne(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_ne(a.low, b.low), .high = wasm_f64x2_ne(a.high, b.high)};}
static inline v256_t wasm_f64x4_le(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_le(a.low, b.low), .high = wasm_f64x2_le(a.high, b.high)};}
static inline v256_t wasm_f64x4_ge(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_ge(a.low, b.low), .high = wasm_f64x2_ge(a.high, b.high)};}
static inline v256_t wasm_f64x4_lt(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_lt(a.low, b.low), .high = wasm_f64x2_lt(a.high, b.high)};}
static inline v256_t wasm_f64x4_gt(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_gt(a.low, b.low), .high = wasm_f64x2_gt(a.high, b.high)};}

static inline v256_t wasm_f64x4_min(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_min(a.low, b.low), .high = wasm_f64x2_min(a.high, b.high)};}
static inline v256_t wasm_f64x4_max(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_max(a.low, b.low), .high = wasm_f64x2_max(a.high, b.high)};}
static inline v256_t wasm_f64x4_pmin(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_pmin(a.low, b.low), .high = wasm_f64x2_pmin(a.high, b.high)};}
static inline v256_t wasm_f64x4_pmax(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_pmax(a.low, b.low), .high = wasm_f64x2_pmax(a.high, b.high)};}
static inline v256_t wasm_f64x4_abs(v256_t x) {return (v256_t){.low = wasm_f64x2_abs(x.low), .high = wasm_f64x2_abs(x.high)};}
static inline v256_t wasm_f64x4_sqrt(v256_t x) {return (v256_t){.low = wasm_f64x2_sqrt(x.low), .high = wasm_f64x2_sqrt(x.high)};}
static inline v256_t wasm_f64x4_neg(v256_t x) {return (v256_t){.low = wasm_f64x2_neg(x.low), .high = wasm_f64x2_neg(x.high)};}
static inline v256_t wasm_f64x4_ceil(v256_t x) {return (v256_t){.low = wasm_f64x2_ceil(x.low), .high = wasm_f64x2_ceil(x.high)};}
static inline v256_t wasm_f64x4_floor(v256_t x) {return (v256_t){.low = wasm_f64x2_floor(x.low), .high = wasm_f64x2_floor(x.high)};}
static inline v256_t wasm_f64x4_nearest(v256_t x) {return (v256_t){.low = wasm_f64x2_nearest(x.low), .high = wasm_f64x2_nearest(x.high)};}
static inline v256_t wasm_f64x4_trunc(v256_t x) {return (v256_t){.low = wasm_f64x2_trunc(x.low), .high = wasm_f64x2_trunc(x.high)};}


static inline v256_t wasm_i64x4_add(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_add(a.low, b.low), .high = wasm_i64x2_add(a.high, b.high)};}
static inline v256_t wasm_i64x4_sub(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_sub(a.low, b.low), .high = wasm_i64x2_sub(a.high, b.high)};}
static inline v256_t wasm_i64x4_mul(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_mul(a.low, b.low), .high = wasm_i64x2_mul(a.high, b.high)};}
static inline v256_t wasm_i64x4_eq(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_eq(a.low, b.low), .high = wasm_i64x2_eq(a.high, b.high)};}
static inline v256_t wasm_i64x4_ne(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_ne(a.low, b.low), .high = wasm_i64x2_ne(a.high, b.high)};}
static inline v256_t wasm_i64x4_le(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_le(a.low, b.low), .high = wasm_i64x2_le(a.high, b.high)};}
static inline v256_t wasm_i64x4_ge(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_ge(a.low, b.low), .high = wasm_i64x2_ge(a.high, b.high)};}
static inline v256_t wasm_i64x4_lt(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_lt(a.low, b.low), .high = wasm_i64x2_lt(a.high, b.high)};}
static inline v256_t wasm_i64x4_gt(v256_t a, v256_t b) {return (v256_t){.low = wasm_i64x2_gt(a.low, b.low), .high = wasm_i64x2_gt(a.high, b.high)};}

static inline v256_t wasm_i64x4_abs(v256_t a) {return (v256_t){.low = wasm_i64x2_abs(a.low), .high = wasm_i64x2_abs(a.high)};}
static inline v256_t wasm_i64x4_neg(v256_t a) {return (v256_t){.low = wasm_i64x2_neg(a.low), .high = wasm_i64x2_neg(a.high)};}
static inline bool wasm_i64x4_all_true(v256_t a) {return wasm_i64x2_all_true(a.low) && wasm_i64x2_all_true(a.high);}
static inline uint32_t wasm_i64x4_bitmask(v256_t a) {return wasm_i64x2_bitmask(a.low) | (wasm_i64x2_bitmask(a.high) << 2);}

static inline v256_t wasm_i64x4_shl(v256_t a, unsigned int v) {return (v256_t){.low = wasm_i64x2_shl(a.low, v), .high = wasm_i64x2_shl(a.high, v)};}
static inline v256_t wasm_i64x4_shr(v256_t a, unsigned int v) {return (v256_t){.low = wasm_i64x2_shr(a.low, v), .high = wasm_i64x2_shr(a.high, v)};}

#ifdef __cplusplus

#include <array>

extern "C" {
    void wasm_f32x4_sincos(v128_t x, v128_t *s, v128_t *c);
    v128_t wasm_f32x4_sin(v128_t x);
    v128_t wasm_f32x4_cos(v128_t x);
    void wasm_f64x4_sincos(v256_t x, v256_t *s, v256_t *c);
    void wasm_f64x2_sincos(v128_t x, v128_t *s, v128_t *c);
}

template <typename T>
struct Vec4;

template <>
struct Vec4<int> {
    static inline v128_t min(v128_t a, v128_t b) {return wasm_i32x4_min(a, b);}
    static inline v128_t min(Vec4 a, Vec4 b) {return min(a.data.vector, b.data.vector);}
    static inline v128_t max(v128_t a, v128_t b) {return wasm_i32x4_max(a, b);}
    static inline v128_t max(Vec4 a, Vec4 b) {return max(a.data.vector, b.data.vector);}
    union {
        v128_t vector;
        struct {int x, y, z, w;} scalar;
    } data;

    inline Vec4() {}
    inline Vec4(v128_t v): data(v) {}
    inline Vec4(const Vec4& v): data(v.data.vector) {}
    inline Vec4(const void *ptr): data(wasm_v128_load(ptr)) {}
    inline Vec4(int splat): data(wasm_i32x4_splat(splat)) {}
    inline Vec4(int x, int y, int z, int w): data(wasm_i32x4_make(x, y, z, w)) {}
    inline int sum() {return data.scalar.x + data.scalar.y + data.scalar.z + data.scalar.w;}

    inline Vec4& operator=(Vec4 other) {data.vector = other.data.vector; return *this;}
    inline Vec4& operator=(v128_t v) {data.vector = v; return *this;}
    inline Vec4 operator-() const {return Vec4(wasm_i32x4_neg(data.vector));}
    inline Vec4 operator+(Vec4 other) const {return Vec4(wasm_i32x4_add(data.vector, other.data.vector));}
    inline Vec4 operator-(Vec4 other) const {return Vec4(wasm_i32x4_sub(data.vector, other.data.vector));}
    inline Vec4 operator*(Vec4 other) const {return Vec4(wasm_i32x4_mul(data.vector, other.data.vector));}
    inline Vec4 operator/(Vec4 other) const {
        return Vec4(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
    }
    inline Vec4& operator+=(Vec4 other) {data.vector = wasm_i32x4_add(data.vector, other.data.vector); return *this;}
    inline Vec4& operator-=(Vec4 other) {data.vector = wasm_i32x4_sub(data.vector, other.data.vector); return *this;}
    inline Vec4& operator*=(Vec4 other) {data.vector = wasm_i32x4_mul(data.vector, other.data.vector); return *this;}
    inline Vec4& operator/=(Vec4 other) {
        data.vector = wasm_i32x4_make(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
        return *this;
    }
    inline Vec4 operator+(v128_t v) const {return Vec4(wasm_i32x4_add(data.vector, v));}
    inline Vec4 operator-(v128_t v) const {return Vec4(wasm_i32x4_sub(data.vector, v));}
    inline Vec4 operator*(v128_t v) const {return Vec4(wasm_i32x4_mul(data.vector, v));}
    inline Vec4 operator/(v128_t v) const {
        Vec4 other(v);
        return Vec4(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
    }
    inline Vec4& operator+=(v128_t v) {data.vector = wasm_i32x4_add(data.vector, v); return *this;}
    inline Vec4& operator-=(v128_t v) {data.vector = wasm_i32x4_sub(data.vector, v); return *this;}
    inline Vec4& operator*=(v128_t v) {data.vector = wasm_i32x4_mul(data.vector, v); return *this;}
    inline Vec4& operator/=(v128_t v) {
        Vec4 other(v);
        data.vector = wasm_i32x4_make(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
        return *this;
    }
    inline Vec4 operator+(int v) const {return Vec4(wasm_i32x4_add(data.vector, wasm_i32x4_splat(v)));}
    inline Vec4 operator-(int v) const {return Vec4(wasm_i32x4_sub(data.vector, wasm_i32x4_splat(v)));}
    inline Vec4 operator*(int v) const {return Vec4(wasm_i32x4_mul(data.vector, wasm_i32x4_splat(v)));}
    inline Vec4 operator/(int v) const {
        return Vec4(
            data.scalar.x / v,
            data.scalar.y / v,
            data.scalar.z / v,
            data.scalar.w / v
        );
    }
    inline Vec4& operator+=(int v) {data.vector = wasm_i32x4_add(data.vector, wasm_i32x4_splat(v)); return *this;}
    inline Vec4& operator-=(int v) {data.vector = wasm_i32x4_sub(data.vector, wasm_i32x4_splat(v)); return *this;}
    inline Vec4& operator*=(int v) {data.vector = wasm_i32x4_mul(data.vector, wasm_i32x4_splat(v)); return *this;}
    inline Vec4& operator/=(int v) {
        data.vector = wasm_i32x4_make(
            data.scalar.x / v,
            data.scalar.y / v,
            data.scalar.z / v,
            data.scalar.w / v
        );
        return *this;
    }
    inline Vec4 operator==(Vec4 other) {return Vec4(wasm_i32x4_eq(data.vector, other.data.vector));}
    inline Vec4 operator!=(Vec4 other) {return Vec4(wasm_i32x4_ne(data.vector, other.data.vector));}
    inline Vec4 operator<=(Vec4 other) {return Vec4(wasm_i32x4_le(data.vector, other.data.vector));}
    inline Vec4 operator>=(Vec4 other) {return Vec4(wasm_i32x4_ge(data.vector, other.data.vector));}
    inline Vec4 operator<(Vec4 other) {return Vec4(wasm_i32x4_lt(data.vector, other.data.vector));}
    inline Vec4 operator>(Vec4 other) {return Vec4(wasm_i32x4_gt(data.vector, other.data.vector));}
    inline Vec4 operator==(v128_t v) {return Vec4(wasm_i32x4_eq(data.vector, v));}
    inline Vec4 operator!=(v128_t v) {return Vec4(wasm_i32x4_ne(data.vector, v));}
    inline Vec4 operator<=(v128_t v) {return Vec4(wasm_i32x4_le(data.vector, v));}
    inline Vec4 operator>=(v128_t v) {return Vec4(wasm_i32x4_ge(data.vector, v));}
    inline Vec4 operator<(v128_t v) {return Vec4(wasm_i32x4_lt(data.vector, v));}
    inline Vec4 operator>(v128_t v) {return Vec4(wasm_i32x4_gt(data.vector, v));}
    inline Vec4 abs() {return Vec4(wasm_i32x4_abs(data.vector));}


    inline Vec4 bitmask() {return Vec4(wasm_i32x4_bitmask(data.vector));}
    inline bool all_true() {return wasm_i32x4_all_true(data.vector);}
    inline bool any_true() {return wasm_v128_any_true(data.vector);}
    inline Vec4 shl(unsigned int v) {return Vec4(wasm_i32x4_shl(data.vector, v));}
    inline Vec4 shr(unsigned int v) {return Vec4(wasm_i32x4_shr(data.vector, v));}
};

template <>
struct Vec4<float> {
    static inline v128_t min(v128_t a, v128_t b) {return wasm_f32x4_min(a, b);}
    static inline v128_t min(Vec4 a, Vec4 b) {return min(a.data.vector, b.data.vector);}
    static inline v128_t max(v128_t a, v128_t b) {return wasm_f32x4_max(a, b);}
    static inline v128_t max(Vec4 a, Vec4 b) {return max(a.data.vector, b.data.vector);}
    static inline v128_t pmin(v128_t a, v128_t b) {return wasm_f32x4_pmin(a, b);}
    static inline v128_t pmin(Vec4 a, Vec4 b) {return pmin(a.data.vector, b.data.vector);}
    static inline v128_t pmax(v128_t a, v128_t b) {return wasm_f32x4_pmax(a, b);}
    static inline v128_t pmax(Vec4 a, Vec4 b) {return pmax(a.data.vector, b.data.vector);}
    union {
        v128_t vector;
        struct {float x, y, z, w;} scalar;
        std::array<float, 4> arr;
    } data;
    inline Vec4() {}
    inline Vec4(v128_t v): data(v) {}
    inline Vec4(const Vec4& v): data(v.data.vector) {}
    inline Vec4(const void *ptr): data(wasm_v128_load(ptr)) {}
    inline Vec4(float splat): data(wasm_f32x4_splat(splat)) {}
    inline Vec4& operator=(Vec4 other) {data.vector = other.data.vector; return *this;}
    inline Vec4& operator=(v128_t v) {data.vector = v; return *this;}
    inline Vec4 operator-() const {return Vec4(wasm_f32x4_neg(data.vector));}
    inline Vec4 operator+(Vec4 other) const {return Vec4(wasm_f32x4_add(data.vector, other.data.vector));}
    inline Vec4 operator-(Vec4 other) const {return Vec4(wasm_f32x4_sub(data.vector, other.data.vector));}
    inline Vec4 operator*(Vec4 other) const {return Vec4(wasm_f32x4_mul(data.vector, other.data.vector));}
    inline Vec4 operator/(Vec4 other) const {return Vec4(wasm_f32x4_div(data.vector, other.data.vector));}
    inline Vec4& operator+=(Vec4 other) {data.vector = wasm_f32x4_add(data.vector, other.data.vector); return *this;}
    inline Vec4& operator-=(Vec4 other) {data.vector = wasm_f32x4_sub(data.vector, other.data.vector); return *this;}
    inline Vec4& operator*=(Vec4 other) {data.vector = wasm_f32x4_mul(data.vector, other.data.vector); return *this;}
    inline Vec4& operator/=(Vec4 other) {data.vector = wasm_f32x4_div(data.vector, other.data.vector); return *this;}
    inline Vec4 operator+(v128_t v) const {return Vec4(wasm_f32x4_add(data.vector, v));}
    inline Vec4 operator-(v128_t v) const {return Vec4(wasm_f32x4_sub(data.vector, v));}
    inline Vec4 operator*(v128_t v) const {return Vec4(wasm_f32x4_mul(data.vector, v));}
    inline Vec4 operator/(v128_t v) const {return Vec4(wasm_f32x4_div(data.vector, v));}
    inline Vec4& operator+=(v128_t v) {data.vector = wasm_f32x4_add(data.vector, v); return *this;}
    inline Vec4& operator-=(v128_t v) {data.vector = wasm_f32x4_sub(data.vector, v); return *this;}
    inline Vec4& operator*=(v128_t v) {data.vector = wasm_f32x4_mul(data.vector, v); return *this;}
    inline Vec4& operator/=(v128_t v) {data.vector = wasm_f32x4_div(data.vector, v); return *this;}
    inline Vec4 operator+(float v) const {return Vec4(wasm_f32x4_add(data.vector, wasm_f32x4_splat(v)));}
    inline Vec4 operator-(float v) const {return Vec4(wasm_f32x4_sub(data.vector, wasm_f32x4_splat(v)));}
    inline Vec4 operator*(float v) const {return Vec4(wasm_f32x4_mul(data.vector, wasm_f32x4_splat(v)));}
    inline Vec4 operator/(float v) const {return Vec4(wasm_f32x4_div(data.vector, wasm_f32x4_splat(v)));}
    inline Vec4& operator+=(float v) {data.vector = wasm_f32x4_add(data.vector, wasm_f32x4_splat(v)); return *this;}
    inline Vec4& operator-=(float v) {data.vector = wasm_f32x4_sub(data.vector, wasm_f32x4_splat(v)); return *this;}
    inline Vec4& operator*=(float v) {data.vector = wasm_f32x4_mul(data.vector, wasm_f32x4_splat(v)); return *this;}
    inline Vec4& operator/=(float v) {data.vector = wasm_f32x4_div(data.vector, wasm_f32x4_splat(v)); return *this;}
    inline Vec4<int> operator==(Vec4 other) {return Vec4<int>(wasm_f32x4_eq(data.vector, other.data.vector));}
    inline Vec4<int> operator!=(Vec4 other) {return Vec4<int>(wasm_f32x4_ne(data.vector, other.data.vector));}
    inline Vec4<int> operator<=(Vec4 other) {return Vec4<int>(wasm_f32x4_le(data.vector, other.data.vector));}
    inline Vec4<int> operator>=(Vec4 other) {return Vec4<int>(wasm_f32x4_ge(data.vector, other.data.vector));}
    inline Vec4<int> operator<(Vec4 other) {return Vec4<int>(wasm_f32x4_lt(data.vector, other.data.vector));}
    inline Vec4<int> operator>(Vec4 other) {return Vec4<int>(wasm_f32x4_gt(data.vector, other.data.vector));}
    inline Vec4<int> operator==(v128_t v) {return Vec4<int>(wasm_f32x4_eq(data.vector, v));}
    inline Vec4<int> operator!=(v128_t v) {return Vec4<int>(wasm_f32x4_ne(data.vector, v));}
    inline Vec4<int> operator<=(v128_t v) {return Vec4<int>(wasm_f32x4_le(data.vector, v));}
    inline Vec4<int> operator>=(v128_t v) {return Vec4<int>(wasm_f32x4_ge(data.vector, v));}
    inline Vec4<int> operator<(v128_t v) {return Vec4<int>(wasm_f32x4_lt(data.vector, v));}
    inline Vec4<int> operator>(v128_t v) {return Vec4<int>(wasm_f32x4_gt(data.vector, v));}
    inline Vec4 abs() {return Vec4(wasm_f32x4_abs(data.vector));}
    inline Vec4 sqrt() {return Vec4(wasm_f32x4_sqrt(data.vector));}
    inline Vec4 ceil() {return Vec4(wasm_f32x4_ceil(data.vector));}
    inline Vec4 floor() {return Vec4(wasm_f32x4_floor(data.vector));}
    inline Vec4 nearest() {return Vec4(wasm_f32x4_nearest(data.vector));}
    inline Vec4 trunc() {return Vec4(wasm_f32x4_trunc(data.vector));}
    inline void sincos(v128_t *s, v128_t *c) {wasm_f32x4_sincos(data.vector, s, c);}
    inline Vec4 sin() {return Vec4(wasm_f32x4_sin(data.vector));}
    inline Vec4 cos() {return Vec4(wasm_f32x4_cos(data.vector));}

    inline Vec4(float x, float y, float z, float w): data(wasm_f32x4_make(x, y, z, w)) {}
    inline float sum() {return data.scalar.x + data.scalar.y + data.scalar.z + data.scalar.w;}
};

template <>
struct Vec4<long long> {
    union {
        v256_t vector;
        struct {long long x, y, z, w;} scalar;
    } data;

    inline Vec4() {}
    inline Vec4(v256_t v): data(v) {}
    inline Vec4(const Vec4& v): data(v.data.vector) {}
    inline Vec4(const void *ptr): data(wasm_v256_load(ptr)) {}
    inline Vec4(long long splat): data(wasm_i64x4_splat(splat)) {}
    inline Vec4(long long x, long long y, long long z, long long w): data(wasm_i64x4_make(x, y, z, w)) {}
    inline int sum() {return data.scalar.x + data.scalar.y + data.scalar.z + data.scalar.w;}

    inline Vec4& operator=(Vec4 other) {data.vector = other.data.vector; return *this;}
    inline Vec4& operator=(v256_t v) {data.vector = v; return *this;}
    inline Vec4 operator-() const {return Vec4(wasm_i64x4_neg(data.vector));}
    inline Vec4 operator+(Vec4 other) const {return Vec4(wasm_i64x4_add(data.vector, other.data.vector));}
    inline Vec4 operator-(Vec4 other) const {return Vec4(wasm_i64x4_sub(data.vector, other.data.vector));}
    inline Vec4 operator*(Vec4 other) const {return Vec4(wasm_i64x4_mul(data.vector, other.data.vector));}
    inline Vec4 operator/(Vec4 other) const {
        return Vec4(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
    }
    inline Vec4& operator+=(Vec4 other) {data.vector = wasm_i64x4_add(data.vector, other.data.vector); return *this;}
    inline Vec4& operator-=(Vec4 other) {data.vector = wasm_i64x4_sub(data.vector, other.data.vector); return *this;}
    inline Vec4& operator*=(Vec4 other) {data.vector = wasm_i64x4_mul(data.vector, other.data.vector); return *this;}
    inline Vec4& operator/=(Vec4 other) {
        data.vector = wasm_i64x4_make(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
        return *this;
    }
    inline Vec4 operator+(v256_t v) const {return Vec4(wasm_i64x4_add(data.vector, v));}
    inline Vec4 operator-(v256_t v) const {return Vec4(wasm_i64x4_sub(data.vector, v));}
    inline Vec4 operator*(v256_t v) const {return Vec4(wasm_i64x4_mul(data.vector, v));}
    inline Vec4 operator/(v256_t v) const {
        Vec4 other(v);
        return Vec4(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
    }
    inline Vec4& operator+=(v256_t v) {data.vector = wasm_i64x4_add(data.vector, v); return *this;}
    inline Vec4& operator-=(v256_t v) {data.vector = wasm_i64x4_sub(data.vector, v); return *this;}
    inline Vec4& operator*=(v256_t v) {data.vector = wasm_i64x4_mul(data.vector, v); return *this;}
    inline Vec4& operator/=(v256_t v) {
        Vec4 other(v);
        data.vector = wasm_i64x4_make(
            data.scalar.x / other.data.scalar.x,
            data.scalar.y / other.data.scalar.y,
            data.scalar.z / other.data.scalar.z,
            data.scalar.w / other.data.scalar.w
        );
        return *this;
    }
    inline Vec4 operator+(long long v) const {return Vec4(wasm_i64x4_add(data.vector, wasm_i64x4_splat(v)));}
    inline Vec4 operator-(long long v) const {return Vec4(wasm_i64x4_sub(data.vector, wasm_i64x4_splat(v)));}
    inline Vec4 operator*(long long v) const {return Vec4(wasm_i64x4_mul(data.vector, wasm_i64x4_splat(v)));}
    inline Vec4 operator/(long long v) const {
        return Vec4(
            data.scalar.x / v,
            data.scalar.y / v,
            data.scalar.z / v,
            data.scalar.w / v
        );
    }
    inline Vec4& operator+=(long long v) {data.vector = wasm_i64x4_add(data.vector, wasm_i64x4_splat(v)); return *this;}
    inline Vec4& operator-=(long long v) {data.vector = wasm_i64x4_sub(data.vector, wasm_i64x4_splat(v)); return *this;}
    inline Vec4& operator*=(long long v) {data.vector = wasm_i64x4_mul(data.vector, wasm_i64x4_splat(v)); return *this;}
    inline Vec4& operator/=(long long v) {
        data.vector = wasm_i64x4_make(
            data.scalar.x / v,
            data.scalar.y / v,
            data.scalar.z / v,
            data.scalar.w / v
        );
        return *this;
    }
    inline Vec4 operator==(Vec4 other) {return Vec4(wasm_i64x4_eq(data.vector, other.data.vector));}
    inline Vec4 operator!=(Vec4 other) {return Vec4(wasm_i64x4_ne(data.vector, other.data.vector));}
    inline Vec4 operator<=(Vec4 other) {return Vec4(wasm_i64x4_le(data.vector, other.data.vector));}
    inline Vec4 operator>=(Vec4 other) {return Vec4(wasm_i64x4_ge(data.vector, other.data.vector));}
    inline Vec4 operator<(Vec4 other) {return Vec4(wasm_i64x4_lt(data.vector, other.data.vector));}
    inline Vec4 operator>(Vec4 other) {return Vec4(wasm_i64x4_gt(data.vector, other.data.vector));}
    inline Vec4 operator==(v256_t v) {return Vec4(wasm_i64x4_eq(data.vector, v));}
    inline Vec4 operator!=(v256_t v) {return Vec4(wasm_i64x4_ne(data.vector, v));}
    inline Vec4 operator<=(v256_t v) {return Vec4(wasm_i64x4_le(data.vector, v));}
    inline Vec4 operator>=(v256_t v) {return Vec4(wasm_i64x4_ge(data.vector, v));}
    inline Vec4 operator<(v256_t v) {return Vec4(wasm_i64x4_lt(data.vector, v));}
    inline Vec4 operator>(v256_t v) {return Vec4(wasm_i64x4_gt(data.vector, v));}
    inline Vec4 abs() {return Vec4(wasm_i64x4_abs(data.vector));}


    inline Vec4 bitmask() {return Vec4(wasm_i64x4_bitmask(data.vector));}
    inline bool all_true() {return wasm_i64x4_all_true(data.vector);}
    inline bool any_true() {return wasm_v256_any_true(data.vector);}
    inline Vec4 shl(unsigned int v) {return Vec4(wasm_i64x4_shl(data.vector, v));}
    inline Vec4 shr(unsigned int v) {return Vec4(wasm_i64x4_shr(data.vector, v));}
};

template <>
struct Vec4<double> {
    static inline v256_t min(v256_t a, v256_t b) {return wasm_f64x4_min(a, b);}
    static inline v256_t min(Vec4 a, Vec4 b) {return min(a.data.vector, b.data.vector);}
    static inline v256_t max(v256_t a, v256_t b) {return wasm_f64x4_max(a, b);}
    static inline v256_t max(Vec4 a, Vec4 b) {return max(a.data.vector, b.data.vector);}
    static inline v256_t pmin(v256_t a, v256_t b) {return wasm_f64x4_pmin(a, b);}
    static inline v256_t pmin(Vec4 a, Vec4 b) {return pmin(a.data.vector, b.data.vector);}
    static inline v256_t pmax(v256_t a, v256_t b) {return wasm_f64x4_pmax(a, b);}
    static inline v256_t pmax(Vec4 a, Vec4 b) {return pmax(a.data.vector, b.data.vector);}
    union {
        v256_t vector;
        struct {double x, y, z, w;} scalar;
    } data;

    inline Vec4() {}
    inline Vec4(v256_t v): data(v) {}
    inline Vec4(const void *ptr): data(wasm_v256_load(ptr)) {}
    inline Vec4(double splat): data(wasm_f64x4_splat(splat)) {}
    inline Vec4(double x, double y, double z, double w): data(wasm_f64x4_make(x, y, z, w)) {}
    inline double sum() {return data.scalar.x + data.scalar.y + data.scalar.z + data.scalar.w;}

    inline Vec4& operator=(Vec4 other) {data.vector = other.data.vector; return *this;}
    inline Vec4& operator=(v256_t v) {data.vector = v; return *this;}
    inline Vec4 operator-() const {return Vec4(wasm_f64x4_neg(data.vector));}
    inline Vec4 operator+(Vec4 other) const {return Vec4(wasm_f64x4_add(data.vector, other.data.vector));}
    inline Vec4 operator-(Vec4 other) const {return Vec4(wasm_f64x4_sub(data.vector, other.data.vector));}
    inline Vec4 operator*(Vec4 other) const {return Vec4(wasm_f64x4_mul(data.vector, other.data.vector));}
    inline Vec4 operator/(Vec4 other) const {return Vec4(wasm_f64x4_div(data.vector, other.data.vector));}
    inline Vec4& operator+=(Vec4 other) {data.vector = wasm_f64x4_add(data.vector, other.data.vector); return *this;}
    inline Vec4& operator-=(Vec4 other) {data.vector = wasm_f64x4_sub(data.vector, other.data.vector); return *this;}
    inline Vec4& operator*=(Vec4 other) {data.vector = wasm_f64x4_mul(data.vector, other.data.vector); return *this;}
    inline Vec4& operator/=(Vec4 other) {data.vector = wasm_f64x4_div(data.vector, other.data.vector); return *this;}
    inline Vec4 operator+(v256_t v) const {return Vec4(wasm_f64x4_add(data.vector, v));}
    inline Vec4 operator-(v256_t v) const {return Vec4(wasm_f64x4_sub(data.vector, v));}
    inline Vec4 operator*(v256_t v) const {return Vec4(wasm_f64x4_mul(data.vector, v));}
    inline Vec4 operator/(v256_t v) const {return Vec4(wasm_f64x4_div(data.vector, v));}
    inline Vec4& operator+=(v256_t v) {data.vector = wasm_f64x4_add(data.vector, v); return *this;}
    inline Vec4& operator-=(v256_t v) {data.vector = wasm_f64x4_sub(data.vector, v); return *this;}
    inline Vec4& operator*=(v256_t v) {data.vector = wasm_f64x4_mul(data.vector, v); return *this;}
    inline Vec4& operator/=(v256_t v) {data.vector = wasm_f64x4_div(data.vector, v); return *this;}
    inline Vec4 operator+(double v) const {return Vec4(wasm_f64x4_add(data.vector, wasm_f64x4_splat(v)));}
    inline Vec4 operator-(double v) const {return Vec4(wasm_f64x4_sub(data.vector, wasm_f64x4_splat(v)));}
    inline Vec4 operator*(double v) const {return Vec4(wasm_f64x4_mul(data.vector, wasm_f64x4_splat(v)));}
    inline Vec4 operator/(double v) const {return Vec4(wasm_f64x4_div(data.vector, wasm_f64x4_splat(v)));}
    inline Vec4& operator+=(double v) {data.vector = wasm_f64x4_add(data.vector, wasm_f64x4_splat(v)); return *this;}
    inline Vec4& operator-=(double v) {data.vector = wasm_f64x4_sub(data.vector, wasm_f64x4_splat(v)); return *this;}
    inline Vec4& operator*=(double v) {data.vector = wasm_f64x4_mul(data.vector, wasm_f64x4_splat(v)); return *this;}
    inline Vec4& operator/=(double v) {data.vector = wasm_f64x4_div(data.vector, wasm_f64x4_splat(v)); return *this;}
    inline Vec4<long long> operator==(Vec4 other) {return Vec4<long long>(wasm_f64x4_eq(data.vector, other.data.vector));}
    inline Vec4<long long> operator!=(Vec4 other) {return Vec4<long long>(wasm_f64x4_ne(data.vector, other.data.vector));}
    inline Vec4<long long> operator<=(Vec4 other) {return Vec4<long long>(wasm_f64x4_le(data.vector, other.data.vector));}
    inline Vec4<long long> operator>=(Vec4 other) {return Vec4<long long>(wasm_f64x4_ge(data.vector, other.data.vector));}
    inline Vec4<long long> operator<(Vec4 other) {return Vec4<long long>(wasm_f64x4_lt(data.vector, other.data.vector));}
    inline Vec4<long long> operator>(Vec4 other) {return Vec4<long long>(wasm_f64x4_gt(data.vector, other.data.vector));}
    inline Vec4<long long> operator==(v256_t v) {return Vec4<long long>(wasm_f64x4_eq(data.vector, v));}
    inline Vec4<long long> operator!=(v256_t v) {return Vec4<long long>(wasm_f64x4_ne(data.vector, v));}
    inline Vec4<long long> operator<=(v256_t v) {return Vec4<long long>(wasm_f64x4_le(data.vector, v));}
    inline Vec4<long long> operator>=(v256_t v) {return Vec4<long long>(wasm_f64x4_ge(data.vector, v));}
    inline Vec4<long long> operator<(v256_t v) {return Vec4<long long>(wasm_f64x4_lt(data.vector, v));}
    inline Vec4<long long> operator>(v256_t v) {return Vec4<long long>(wasm_f64x4_gt(data.vector, v));}
    inline Vec4 abs() {return Vec4(wasm_f64x4_abs(data.vector));}
    inline Vec4 sqrt() {return Vec4(wasm_f64x4_sqrt(data.vector));}
    inline Vec4 ceil() {return Vec4(wasm_f64x4_ceil(data.vector));}
    inline Vec4 floor() {return Vec4(wasm_f64x4_floor(data.vector));}
    inline Vec4 nearest() {return Vec4(wasm_f64x4_nearest(data.vector));}
    inline Vec4 trunc() {return Vec4(wasm_f64x4_trunc(data.vector));}
    inline void sincos(v256_t *s, v256_t *c) {wasm_f64x4_sincos(data.vector, s, c);}
};


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
void wasm_f64x2_sincos(v128_t x, v128_t *s, v128_t *c) {
    sincos_pd((v2df)x, (v2df*)s, (v2df*)c);
}

#endif // __cplusplus


#endif // __SIMD_H
