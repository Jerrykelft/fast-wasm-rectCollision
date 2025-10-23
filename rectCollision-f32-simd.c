#include <tgmath.h>
#include <wasm_simd128.h>

#define true 1
#define false 0
#define rect_param(__NAME__) Vector2 __NAME__[static restrict 4] // 矩形
#define f32x4_get(a, i) wasm_f32x4_extract_lane(a, i) // 取出 v128 的第 i 個 float

typedef struct {float x, y;} Vector2; // 向量
typedef Vector2 RectVertex[4]; // 矩形頂點
typedef struct {float min, max;} Project; // 投影

#ifdef __INTELLISENSE__
// 這段只給 VSCode IntelliSense 看
v128_t __builtin_wasm_shuffle_i8x16(v128_t, v128_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
#endif

// 獲取無旋轉矩形
static inline void _getRightRect(rect_param(result), float x, float y, float w, float h) {
    float hw = w / 2.0f, hh = h / 2.0f;
    result[0].x = result[3].x = x - hw;
    result[1].y = result[0].y = y - hh;
    result[2].x = result[1].x = x + hw;
    result[3].y = result[2].y = y + hh;
}

// 獲取頂點
static inline void _getRect(rect_param(result), float x, float y, float w, float h, float r) {
    float hw = w / 2.0f, hh = h / 2.0f;
    float c = cos(r), s = sin(r);
    float hw_c = hw * c, hh_s = hh * s, hw_s = hw * s, hh_c = hh * c;
    result[0].x = x - hw_c + hh_s; result[0].y = y - hw_s - hh_c;
    result[1].x = x + hw_c + hh_s; result[1].y = y + hw_s - hh_c;
    result[2].x = x + hw_c - hh_s; result[2].y = y + hw_s + hh_c;
    result[3].x = x - hw_c - hh_s; result[3].y = y - hw_s + hh_c;
}

// 點繞圓旋轉
static inline void _pointRotateAround(float px[static restrict 1], float py[static restrict 1], float cx, float cy, float r) {
    float s = sin(r), c = cos(r);
    float x = *px - cx, y = *py - cy; // 平移到原點

    // 旋轉並平移回原位
    *px = x * c - y * s + cx;
    *py = x * s + y * c + cy;
}

// 將多邊形投影到軸上
static inline Project _projectPolygon(Vector2 axis, rect_param(rect)) {
    float min = axis.x * rect[0].x + axis.y * rect[0].y;
    float max = min;
    for (int i = 1; i < 4; i++) {
        float p = axis.x * rect[i].x + axis.y * rect[i].y;
        if (p < min) min = p;
        if (p > max) max = p;
    }
    return (Project){min, max};
}

// 計算矩形邊界
static inline void _rectBounds(rect_param(rect), Vector2 min[static restrict 1], Vector2 max[static restrict 1]) {
    min->x = fmin(fmin(rect[0].x, rect[1].x), fmin(rect[2].x, rect[3].x));
    min->y = fmin(fmin(rect[0].y, rect[1].y), fmin(rect[2].y, rect[3].y));
    max->x = fmax(fmax(rect[0].x, rect[1].x), fmax(rect[2].x, rect[3].x));
    max->y = fmax(fmax(rect[0].y, rect[1].y), fmax(rect[2].y, rect[3].y));
}

// 包圍盒檢測
static inline bool _boundingCollision(rect_param(rect1), rect_param(rect2)) {
    Vector2 min1, max1, min2, max2;
    _rectBounds(rect1, &min1, &max1);
    _rectBounds(rect2, &min2, &max2);
    return !(max1.x < min2.x || min1.x > max2.x || max1.y < min2.y || min1.y > max2.y);
}

// AABB vs OBB SAT 判定，AABB 為 rect1(未旋轉)，OBB 為 rect2(已旋轉)
static inline bool _satAABBvsOBB(rect_param(rect1), rect_param(rect2)) {
    if (!_boundingCollision(rect1, rect2)) return 0;

    // 使用 OBB 的邊產生分離軸
    Vector2 axes[2];
    for (int i = 0; i < 2; i++) {
        int next = (i + 1) % 4;
        axes[i].x = -(rect2[next].y - rect2[i].y);
        axes[i].y = (rect2[next].x - rect2[i].x);
    }

    v128_t vaxis0_x = wasm_f32x4_splat(axes[0].x);
    v128_t vaxis0_y = wasm_f32x4_splat(axes[0].y);
    v128_t vaxis1_x = wasm_f32x4_splat(axes[1].x);
    v128_t vaxis1_y = wasm_f32x4_splat(axes[1].y);

    v128_t vrect1_x = wasm_f32x4_make(rect1[0].x, rect1[1].x, rect1[2].x, rect1[3].x);
    v128_t vrect1_y = wasm_f32x4_make(rect1[0].y, rect1[1].y, rect1[2].y, rect1[3].y);
    v128_t vrect2_x = wasm_f32x4_make(rect2[0].x, rect2[1].x, rect2[2].x, rect2[3].x);
    v128_t vrect2_y = wasm_f32x4_make(rect2[0].y, rect2[1].y, rect2[2].y, rect2[3].y);

    v128_t projs[4] = {
        wasm_f32x4_add(wasm_f32x4_mul(vaxis0_x, vrect1_x), wasm_f32x4_mul(vaxis0_y, vrect1_y)),
        wasm_f32x4_add(wasm_f32x4_mul(vaxis0_x, vrect2_x), wasm_f32x4_mul(vaxis0_y, vrect2_y)),
        wasm_f32x4_add(wasm_f32x4_mul(vaxis1_x, vrect1_x), wasm_f32x4_mul(vaxis1_y, vrect1_y)),
        wasm_f32x4_add(wasm_f32x4_mul(vaxis1_x, vrect2_x), wasm_f32x4_mul(vaxis1_y, vrect2_y))
    };
    v128_t a = wasm_f32x4_make(f32x4_get(projs[0], 0), f32x4_get(projs[1], 0), f32x4_get(projs[2], 0), f32x4_get(projs[3], 0));
    v128_t b = wasm_f32x4_make(f32x4_get(projs[0], 1), f32x4_get(projs[1], 1), f32x4_get(projs[2], 1), f32x4_get(projs[3], 1));
    v128_t c = wasm_f32x4_make(f32x4_get(projs[0], 2), f32x4_get(projs[1], 2), f32x4_get(projs[2], 2), f32x4_get(projs[3], 2));
    v128_t d = wasm_f32x4_make(f32x4_get(projs[0], 3), f32x4_get(projs[1], 3), f32x4_get(projs[2], 3), f32x4_get(projs[3], 3));
    v128_t vmin = wasm_f32x4_min(wasm_f32x4_min(a, b), wasm_f32x4_min(c, d));
    v128_t vmax = wasm_f32x4_max(wasm_f32x4_max(a, b), wasm_f32x4_max(c, d));

    v128_t cmp = wasm_f32x4_lt(vmax, wasm_v32x4_shuffle(vmin, vmin, 1, 0, 3, 2));
    return !wasm_v128_any_true(cmp);
}

// 碰撞偵測(基於變換數據)
bool rectCollision(
    float x1, float y1, float w1, float h1, float r1,
    float x2, float y2, float w2, float h2, float r2
) {
    if (fabs(r1) > 1e-5f) _pointRotateAround(&x2, &y2, x1, y1, -r1);

    RectVertex rect1, rect2;
    _getRightRect(rect1, x1, y1, w1, h1);
    float newr2 = r2 - r1;
    int intRad = (int)round(newr2 * 10000.0f);
    if (intRad % 15708 == 0) {
        int isSwapped = intRad % 31416 != 0;
        _getRightRect(rect2, x2, y2, isSwapped ? h2 : w2, isSwapped ? w2 : h2);
        return _boundingCollision(rect1, rect2);
    }
    _getRect(rect2, x2, y2, w2, h2, newr2);
    return _satAABBvsOBB(rect1, rect2);
}
