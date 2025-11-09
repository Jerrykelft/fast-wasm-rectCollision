#include <cmath>
#include <array>
#include <concepts>

#define USE_DEFAULT_VEC4
#include "simd.c"

extern "C" {
    void sincosf(float x, float *s, float *c);
    void sincos(double x, double *s, double *c);
}

#define RECT_COLLISION_PARAM double x1, double y1, FLOAT w1, FLOAT h1, FLOAT r1, double x2, double y2, FLOAT w2, FLOAT h2, FLOAT r2

#define FLOAT_TEMPLATE \
template <typename FLOAT> \
requires std::floating_point<FLOAT>

FLOAT_TEMPLATE struct Mat4x4;

template <>
struct Mat4x4<float>: std::array<Vec4<float>, 4> {
    inline Mat4x4() {}
    inline Mat4x4(Vec4<float> a, Vec4<float> b, Vec4<float> c, Vec4<float> d) {
        (*this)[0].data = a.data;
        (*this)[1].data = b.data;
        (*this)[2].data = c.data;
        (*this)[3].data = d.data;
    }
    inline Mat4x4<float>& transpose() {
        // 交換相鄰行內的低兩個元素
        v128_t t0 = wasm_i32x4_shuffle((*this)[0].data.vector, (*this)[1].data.vector, 0, 4, 1, 5);
        v128_t t1 = wasm_i32x4_shuffle((*this)[0].data.vector, (*this)[1].data.vector, 2, 6, 3, 7);
        v128_t t2 = wasm_i32x4_shuffle((*this)[2].data.vector, (*this)[3].data.vector, 0, 4, 1, 5);
        v128_t t3 = wasm_i32x4_shuffle((*this)[2].data.vector, (*this)[3].data.vector, 2, 6, 3, 7);

        // 交叉合併上下兩組
        (*this)[0] = wasm_i32x4_shuffle(t0, t2, 0, 1, 4, 5);
        (*this)[1] = wasm_i32x4_shuffle(t0, t2, 2, 3, 6, 7);
        (*this)[2] = wasm_i32x4_shuffle(t1, t3, 0, 1, 4, 5);
        (*this)[3] = wasm_i32x4_shuffle(t1, t3, 2, 3, 6, 7);
        return *this;
    }
};

template <>
struct Mat4x4<double>: std::array<Vec4<double>, 4> {
    inline Mat4x4() {}
    inline Mat4x4(Vec4<double> a, Vec4<double> b, Vec4<double> c, Vec4<double> d) {
        (*this)[0].data = a.data;
        (*this)[1].data = b.data;
        (*this)[2].data = c.data;
        (*this)[3].data = d.data;
    }
    inline Mat4x4<double>& transpose() {
        // 交換相鄰行內的低兩個元素
        v256_t t0 = wasm_i64x4_shuffle((*this)[0].data.vector, (*this)[1].data.vector, 0, 4, 1, 5);
        v256_t t1 = wasm_i64x4_shuffle((*this)[0].data.vector, (*this)[1].data.vector, 2, 6, 3, 7);
        v256_t t2 = wasm_i64x4_shuffle((*this)[2].data.vector, (*this)[3].data.vector, 0, 4, 1, 5);
        v256_t t3 = wasm_i64x4_shuffle((*this)[2].data.vector, (*this)[3].data.vector, 2, 6, 3, 7);

        // 交叉合併上下兩組
        (*this)[0] = wasm_i64x4_shuffle(t0, t2, 0, 1, 4, 5);
        (*this)[1] = wasm_i64x4_shuffle(t0, t2, 2, 3, 6, 7);
        (*this)[2] = wasm_i64x4_shuffle(t1, t3, 0, 1, 4, 5);
        (*this)[3] = wasm_i64x4_shuffle(t1, t3, 2, 3, 6, 7);
        return *this;
    }
};

FLOAT_TEMPLATE struct Vec2x4;

// 向量
FLOAT_TEMPLATE struct Vec2 {
    FLOAT x, y;
    inline Vec2() {}
    inline Vec2(FLOAT _x, FLOAT _y): x(_x), y(_y) {}
    inline Vec2 operator+(Vec2 other) const {return {x + other.x, y + other.y};}
    inline Vec2 operator-(Vec2 other) const {return {x - other.x, y - other.y};}
    inline FLOAT dot(Vec2 other) const {return x * other.x + y * other.y;}
    inline Vec4<FLOAT> dot(Vec2x4<FLOAT>other) const {
        return Vec4<FLOAT>(x) * other.data.vector.vx + Vec4<FLOAT>(y) * other.data.vector.vy;
    }
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
};

static inline void _sincos(float x, float *s, float *c) {sincosf(x, s, c);}
static inline void _sincos(double x, double *s, double *c) {sincos(x, s, c);}

FLOAT_TEMPLATE static inline bool check_collision_obb2d(
    double x1, double y1, FLOAT w1, FLOAT h1, FLOAT r1,
    double x2, double y2, FLOAT w2, FLOAT h2, FLOAT r2
) {
    // wasm_f64x2_sincos(wasm_f64x2_make(r1, r2));
    FLOAT s1 = sin(r1), c1 = cos(r1),
          s2 = sin(r2), c2 = cos(r2);
    FLOAT x = x2 - x1, y = y2 - y1;
    Vec2<FLOAT> axis[4] = {{c1, s1}, {-s1, c1}, {c2, s2}, {-s2, c2}},
                D = {x, y};
    Vec2x4<FLOAT> vaxis = {std::array<FLOAT, 4>{c1, -s1, c2, -s2}, std::array<FLOAT, 4>{s1, c1, s2, c2}};

    FLOAT hw1 = w1 * 0.5f,
          hh1 = h1 * 0.5f,
          hw2 = w2 * 0.5f,
          hh2 = h2 * 0.5f;

    // 使用 broadcast 進行四軸 dot
    Vec4<FLOAT> limit = (Vec4<FLOAT>(hw1) * (axis[0].dot(vaxis))).abs() +
                        (Vec4<FLOAT>(hh1) * (axis[1].dot(vaxis))).abs() +
                        (Vec4<FLOAT>(hw2) * (axis[2].dot(vaxis))).abs() +
                        (Vec4<FLOAT>(hh2) * (axis[3].dot(vaxis))).abs();
    return !(D.dot(vaxis).abs() > limit).any_true();
}

// FLOAT_TEMPLATE static inline bool check_collision_obb2d(
//     double x1, double y1, FLOAT w1, FLOAT h1, FLOAT r1,
//     double x2, double y2, FLOAT w2, FLOAT h2, FLOAT r2
// ) {
//     FLOAT s1 = sin(r1),
//           c1 = cos(r1),
//           s2 = sin(r2),
//           c2 = cos(r2);
//     Vec2<FLOAT> axis[4] = {
//         {c1, s1},
//         {-s1, c1},
//         {c2, s2},
//         {-s2, c2}
//     };
//     Vec2<FLOAT> D(x2 - x1, y2 - y1);
//     for (int i = 0; i < 4; i++) {
//         FLOAT r = abs(w1 * static_cast<FLOAT>(0.5) * axis[i].dot(axis[0])) +
//                   abs(h1 * static_cast<FLOAT>(0.5) * axis[i].dot(axis[1])) +
//                   abs(w2 * static_cast<FLOAT>(0.5) * axis[i].dot(axis[2])) +
//                   abs(h2 * static_cast<FLOAT>(0.5) * axis[i].dot(axis[3]));
//         if (abs(D.dot(axis[i])) > r) return false; 
//     }
//     return true;
// }

// 獲取無旋轉矩形頂點
FLOAT_TEMPLATE static inline RectVertexSIMD<FLOAT> _getAABB(FLOAT x, FLOAT y, FLOAT w, FLOAT h) {
    RectVertexSIMD<FLOAT> result;
    FLOAT hw = w * static_cast<FLOAT>(0.5), hh = h * static_cast<FLOAT>(0.5);
    result.data.scalar.x[0] = result.data.scalar.x[3] = x - hw;
    result.data.scalar.y[1] = result.data.scalar.y[0] = y - hh;
    result.data.scalar.x[2] = result.data.scalar.x[1] = x + hw;
    result.data.scalar.y[3] = result.data.scalar.y[2] = y + hh;
    return result;
}

// 獲取矩形頂點
FLOAT_TEMPLATE static inline RectVertexSIMD<FLOAT> _getOBB(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT r) {
    FLOAT hw = w * static_cast<FLOAT>(0.5), hh = h * static_cast<FLOAT>(0.5);
    FLOAT s = sin(r), c = cos(r);
    FLOAT hw_c = hw * c, hh_s = hh * s, hw_s = hw * s, hh_c = hh * c;
    return RectVertexSIMD<FLOAT>(
        std::array<FLOAT, 4>{x - hw_c + hh_s, x + hw_c + hh_s, x + hw_c - hh_s, x - hw_c - hh_s},
        std::array<FLOAT, 4>{y - hw_s - hh_c, y + hw_s - hh_c, y + hw_s + hh_c, y - hw_s + hh_c}
    );
    // return RectVertexSIMD<FLOAT>(
    //     Vec4<FLOAT>(x) + Vec4<FLOAT>(-hw_c, hw_c, hw_c, -hw_c) + Vec4<FLOAT>(hh_s, hh_s, -hh_s, -hh_s),
    //     Vec4<FLOAT>(y) + Vec4<FLOAT>(-hw_s, hw_s, hw_s, -hw_s) + Vec4<FLOAT>(-hh_c, -hh_c, hh_c, hh_c)
    // );
}

// 點繞圓旋轉
FLOAT_TEMPLATE static inline void _rotateAround(FLOAT *px, FLOAT *py, FLOAT cx, FLOAT cy, FLOAT r) {
    FLOAT s = sin(r), c = cos(r);
    FLOAT x = *px - cx, y = *py - cy; // 平移到原點

    // 旋轉並平移回原位
    *px = x * c - y * s + cx;
    *py = x * s + y * c + cy;
}

// 計算矩形邊界
FLOAT_TEMPLATE static inline Bounding<FLOAT> _rectBounding(RectVertexSIMD<FLOAT> rect) {
    return Bounding<FLOAT>(
        Vec2<FLOAT>(
            fmin(fmin(rect.data.scalar.x[0], rect.data.scalar.x[1]), fmin(rect.data.scalar.x[2], rect.data.scalar.x[3])),
            fmin(fmin(rect.data.scalar.y[0], rect.data.scalar.y[1]), fmin(rect.data.scalar.y[2], rect.data.scalar.y[3]))
        ),
        Vec2<FLOAT>(
            fmax(fmax(rect.data.scalar.x[0], rect.data.scalar.x[1]), fmax(rect.data.scalar.x[2], rect.data.scalar.x[3])),
            fmax(fmax(rect.data.scalar.y[0], rect.data.scalar.y[1]), fmax(rect.data.scalar.y[2], rect.data.scalar.y[3]))
        )
    );
}

// 包圍盒檢測
FLOAT_TEMPLATE static inline bool _boundingCollision(RectVertexSIMD<FLOAT> rect1, RectVertexSIMD<FLOAT> rect2) {
    Bounding<FLOAT> a = _rectBounding(rect1);
    Bounding<FLOAT> b = _rectBounding(rect2);
    return !(a.max.x < b.min.x || a.min.x > b.max.x || a.max.y < b.min.y || a.min.y > b.max.y);
}
// 精度下降嚴重
FLOAT_TEMPLATE static inline bool _AABBvsAABB(RectVertexSIMD<FLOAT> rect1, RectVertexSIMD<FLOAT> rect2) {
    return !(rect1.data.scalar.x[1] < rect2.data.scalar.x[0] || rect1.data.scalar.x[0] > rect2.data.scalar.x[1] || rect1.data.scalar.y[0] < rect2.data.scalar.y[2] || rect1.data.scalar.y[2] > rect2.data.scalar.y[0]);
}

// AABB vs OBB SAT 判定，AABB 為 rect1(未旋轉)，OBB 為 rect2(已旋轉)
FLOAT_TEMPLATE static inline bool _satAABBvsOBB(RectVertexSIMD<FLOAT> rect1, RectVertexSIMD<FLOAT> rect2) {
    if (!_boundingCollision<FLOAT>(rect1, rect2)) return false;

    // 使用 OBB 的邊產生分離軸
    Vec2<FLOAT> axes[2] = {
        Vec2<FLOAT>(-(rect2.data.scalar.y[1] - rect2.data.scalar.y[0]), rect2.data.scalar.x[1] - rect2.data.scalar.x[0]),
        Vec2<FLOAT>(-(rect2.data.scalar.y[2] - rect2.data.scalar.y[1]), rect2.data.scalar.x[2] - rect2.data.scalar.x[1])
    };

    Vec4<FLOAT> vaxis0_x(axes[0].x);
    Vec4<FLOAT> vaxis0_y(axes[0].y);
    Vec4<FLOAT> vaxis1_x(axes[1].x);
    Vec4<FLOAT> vaxis1_y(axes[1].y);

    Mat4x4<FLOAT> projs(
        vaxis0_x * rect1.data.vector.vx + vaxis0_y * rect1.data.vector.vy,
        vaxis0_x * rect2.data.vector.vx + vaxis0_y * rect2.data.vector.vy,
        vaxis1_x * rect1.data.vector.vx + vaxis1_y * rect1.data.vector.vy,
        vaxis1_x * rect2.data.vector.vx + vaxis1_y * rect2.data.vector.vy
    );
    projs.transpose();

    Vec4<FLOAT> vmin = Vec4<FLOAT>::pmin(Vec4<FLOAT>::pmin(projs[0], projs[1]), Vec4<FLOAT>::pmin(projs[2], projs[3]));
    Vec4<FLOAT> vmax = Vec4<FLOAT>::pmax(Vec4<FLOAT>::pmax(projs[0], projs[1]), Vec4<FLOAT>::pmax(projs[2], projs[3]));

    if constexpr (std::is_same_v<FLOAT, float>) {
        v128_t cmp = wasm_f32x4_lt(vmax.data, wasm_i32x4_shuffle(vmin.data, vmin.data, 1, 0, 3, 2));
        return !wasm_v128_any_true(cmp);
    } else {
        v256_t cmp = wasm_f64x4_lt(vmax.data, wasm_i64x4_shuffle(vmin.data, vmin.data, 1, 0, 3, 2));
        return !wasm_v256_any_true(cmp);
    }
}

// 投影
// FLOAT_TEMPLATE struct Project {
//     FLOAT min, max;
//     inline Project() {}
//     inline Project(FLOAT _min, FLOAT _max): min(_min), max(_max) {}
// };
// FLOAT_TEMPLATE static inline Project<FLOAT> _projectPolygon(Vec2<FLOAT> axis, RectVertex<FLOAT> rect) {
//     FLOAT min = axis.x * rect[0].x + axis.y * rect[0].y;
//     FLOAT max = min;
//     for (int i = 1; i < 4; ++i) {
//         FLOAT p = axis.x * rect[i].x + axis.y * rect[i].y;
//         if (p < min) min = p;
//         if (p > max) max = p;
//     }
//     return Project<FLOAT>(min, max);
// }
// FLOAT_TEMPLATE static inline bool _satAABBvsOBB_OLD(RectVertex<FLOAT> rect1, RectVertex<FLOAT> rect2) {
//     if (!_boundingCollision(rect1, rect2)) return false;
//     Vec2<FLOAT> axes[2];
//     for (int i = 0; i < 2; ++i) {
//         int next = (i + 1) % 4;
//         axes[i].x = -(rect2[next].y - rect2[i].y);
//         axes[i].y = (rect2[next].x - rect2[i].x);
//     }
//     for (int i = 0; i < 2; ++i) {
//         Project<FLOAT> proj1 = _projectPolygon(axes[i], rect1);
//         Project<FLOAT> proj2 = _projectPolygon(axes[i], rect2);
//         if (proj1.max < proj2.min || proj2.max < proj1.min) return false;
//     }
//     return true;
// }

// 碰撞偵測
FLOAT_TEMPLATE static inline bool _rectCollision(
    double x1, double y1, FLOAT w1, FLOAT h1, FLOAT r1,
    double x2, double y2, FLOAT w2, FLOAT h2, FLOAT r2
) {
    FLOAT x = x2 - x1;
    FLOAT y = y2 - y1;
    if (abs(r1) > static_cast<FLOAT>(1e-5)) _rotateAround<FLOAT>(&x, &y, static_cast<FLOAT>(0.0), static_cast<FLOAT>(0.0), -r1);

    RectVertexSIMD<FLOAT> rect1 = _getAABB<FLOAT>(static_cast<FLOAT>(0.0), static_cast<FLOAT>(0.0), w1, h1);
    FLOAT newr2 = r2 - r1;
    int intRad = (int)round(newr2 * static_cast<FLOAT>(10000.0));
    if (intRad % 15708 == 0) {
        int isSwapped = intRad % 31416 != 0;
        return _AABBvsAABB<FLOAT>(rect1, _getAABB<FLOAT>(x, y, isSwapped ? h2 : w2, isSwapped ? w2 : h2));
    }
    return _satAABBvsOBB<FLOAT>(rect1, _getOBB<FLOAT>(x, y, w2, h2, newr2));
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
        return check_collision_obb2d<float>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
        // return _rectCollision<float>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
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
        return check_collision_obb2d<double>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
        // return _rectCollision<double>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
    }
}

/*
wat2wasm rectCollision2D.wat -o rectCollision2D.wasm
wasm-opt rectCollision2D.wasm -o rectCollision2D.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce
wasm2wat rectCollision2D.wasm -o rectCollision2D.wat
*/
