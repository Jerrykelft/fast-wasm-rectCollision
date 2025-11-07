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

// 向量
FLOAT_TEMPLATE struct Vec2 {
    FLOAT x, y;
    inline Vec2() {}
    inline Vec2(FLOAT _x, FLOAT _y): x(_x), y(_y) {}
};
FLOAT_TEMPLATE struct RectVertexSIMD {
    std::array<FLOAT, 4> x, y;
    inline RectVertexSIMD() {}
    inline RectVertexSIMD(std::array<FLOAT, 4> _x, std::array<FLOAT, 4> _y): x(_x), y(_y) {}
    inline RectVertexSIMD(v128_t _x, v128_t _y) {
        wasm_v128_store(&x, _x);
        wasm_v128_store(&y, _y);
    }
    inline RectVertexSIMD(v256_t _x, v256_t _y) {
        wasm_v256_store(&x, _x);
        wasm_v256_store(&y, _y);
    }
    inline RectVertexSIMD(Vec4<float> _x, Vec4<float> _y) {
        wasm_v128_store(&x, _x.data);
        wasm_v128_store(&y, _y.data);
    }
    inline RectVertexSIMD(Vec4<double> _x, Vec4<double> _y) {
        wasm_v256_store(&x, _x.data);
        wasm_v256_store(&y, _y.data);
    }
};
// 包圍盒
FLOAT_TEMPLATE struct Bounding {
    Vec2<FLOAT> min, max;
    inline Bounding() {}
    inline Bounding(Vec2<FLOAT> _min, Vec2<FLOAT> _max): min(_min), max(_max) {}
};
FLOAT_TEMPLATE using RectVertex = std::array<Vec2<FLOAT>, 4>; // 矩形頂點

// 獲取無旋轉矩形頂點
FLOAT_TEMPLATE static inline RectVertexSIMD<FLOAT> _getAABB(FLOAT x, FLOAT y, FLOAT w, FLOAT h) {
    RectVertexSIMD<FLOAT> result;
    FLOAT hw = w * static_cast<FLOAT>(0.5), hh = h * static_cast<FLOAT>(0.5);
    result.x[0] = result.x[3] = x - hw;
    result.y[1] = result.y[0] = y - hh;
    result.x[2] = result.x[1] = x + hw;
    result.y[3] = result.y[2] = y + hh;
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
            fmin(fmin(rect.x[0], rect.x[1]), fmin(rect.x[2], rect.x[3])),
            fmin(fmin(rect.y[0], rect.y[1]), fmin(rect.y[2], rect.y[3]))
        ),
        Vec2<FLOAT>(
            fmax(fmax(rect.x[0], rect.x[1]), fmax(rect.x[2], rect.x[3])),
            fmax(fmax(rect.y[0], rect.y[1]), fmax(rect.y[2], rect.y[3]))
        )
    );
}

// 包圍盒檢測
FLOAT_TEMPLATE static inline bool _boundingCollision(RectVertexSIMD<FLOAT> rect1, RectVertexSIMD<FLOAT> rect2) {
    Bounding<FLOAT> a = _rectBounding(rect1);
    Bounding<FLOAT> b = _rectBounding(rect2);
    return !(a.max.x < b.min.x || a.min.x > b.max.x || a.max.y < b.min.y || a.min.y > b.max.y);
}

FLOAT_TEMPLATE static inline bool _AABBvsAABB(RectVertexSIMD<FLOAT> rect1, RectVertexSIMD<FLOAT> rect2) {
    return !(rect1.x[1] < rect2.x[0] || rect1.x[0] > rect2.x[1] || rect1.y[0] < rect2.y[2] || rect1.y[2] > rect2.y[0]);
}

// AABB vs OBB SAT 判定，AABB 為 rect1(未旋轉)，OBB 為 rect2(已旋轉)
FLOAT_TEMPLATE static inline bool _satAABBvsOBB(RectVertexSIMD<FLOAT> rect1, RectVertexSIMD<FLOAT> rect2) {
    if (!_boundingCollision<FLOAT>(rect1, rect2)) return false;

    // 使用 OBB 的邊產生分離軸
    Vec2<FLOAT> axes[2] = {
        Vec2<FLOAT>(-(rect2.y[1] - rect2.y[0]), rect2.x[1] - rect2.x[0]),
        Vec2<FLOAT>(-(rect2.y[2] - rect2.y[1]), rect2.x[2] - rect2.x[1])
    };

    Vec4<FLOAT> vaxis0_x(axes[0].x);
    Vec4<FLOAT> vaxis0_y(axes[0].y);
    Vec4<FLOAT> vaxis1_x(axes[1].x);
    Vec4<FLOAT> vaxis1_y(axes[1].y);

    Vec4<FLOAT> vrect1_x((const void*)&rect1.x);
    Vec4<FLOAT> vrect1_y((const void*)&rect1.y);
    Vec4<FLOAT> vrect2_x((const void*)&rect2.x);
    Vec4<FLOAT> vrect2_y((const void*)&rect2.y);

    Mat4x4<FLOAT> projs(
        vaxis0_x * vrect1_x + vaxis0_y * vrect1_y,
        vaxis0_x * vrect2_x + vaxis0_y * vrect2_y,
        vaxis1_x * vrect1_x + vaxis1_y * vrect1_y,
        vaxis1_x * vrect2_x + vaxis1_y * vrect2_y
    );
    projs.transpose();

    Vec4<FLOAT> vmin = Vec4<FLOAT>::min(Vec4<FLOAT>::min(projs[0], projs[1]), Vec4<FLOAT>::min(projs[2], projs[3]));
    Vec4<FLOAT> vmax = Vec4<FLOAT>::max(Vec4<FLOAT>::max(projs[0], projs[1]), Vec4<FLOAT>::max(projs[2], projs[3]));

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
FLOAT_TEMPLATE static inline bool rectCollision(
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
        return rectCollision<float>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
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
        return rectCollision<double>(x1, y1, w1, h1, r1, x2, y2, w2, h2, r2);
    }
}

/*
wat2wasm rectCollision2D.wat -o rectCollision2D.wasm
wasm-opt rectCollision2D.wasm -o rectCollision2D.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce
wasm2wat rectCollision2D.wasm -o rectCollision2D.wat
*/
