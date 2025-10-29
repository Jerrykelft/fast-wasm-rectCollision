#include <tgmath.h>
#include <wasm_simd128.h>

#ifndef USE_FLOAT64
    #define FLOAT float
    #define f0_5 0.5f
    #define f10000 10000.0f
    #define f1e_5 1e-5f
#else
    #define FLOAT double
    #define f0_5 0.5
    #define f10000 10000.0
    #define f1e_5 1e-5
#endif
#define RECT_COLLISION_PARAM FLOAT x1, FLOAT y1, FLOAT w1, FLOAT h1, FLOAT r1, FLOAT x2, FLOAT y2, FLOAT w2, FLOAT h2, FLOAT r2

#define RECT_PARAM(__NAME__) Vector2 __NAME__[static restrict 4] // 矩形

#ifdef __INTELLISENSE__
// 這段只給 VSCode IntelliSense 看
v128_t __builtin_wasm_shuffle_i8x16(v128_t, v128_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
#endif

typedef struct {FLOAT x, y;} Vector2; // 向量
typedef Vector2 RectVertex[4]; // 矩形頂點
typedef struct {Vector2 min, max;} Bounding; // 包圍盒
typedef struct {v128_t low, high;} v256_t;

static inline v256_t wasm_f64x4_splat(double a) {return (v256_t){.low = wasm_f64x2_splat(a), .high = wasm_f64x2_splat(a)};}
static inline v256_t wasm_f64x4_make(double a, double b, double c, double d) {return (v256_t){.low = wasm_f64x2_make(a, b), .high = wasm_f64x2_make(c, d)};}
static inline v256_t wasm_f64x4_mul(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_mul(a.low, b.low), .high = wasm_f64x2_mul(a.high, b.high)};}
static inline v256_t wasm_f64x4_add(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_add(a.low, b.low), .high = wasm_f64x2_add(a.high, b.high)};}
static inline v256_t wasm_f64x4_min(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_min(a.low, b.low), .high = wasm_f64x2_min(a.high, b.high)};}
static inline v256_t wasm_f64x4_max(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_max(a.low, b.low), .high = wasm_f64x2_max(a.high, b.high)};}
static inline v256_t wasm_f64x4_lt(v256_t a, v256_t b) {return (v256_t){.low = wasm_f64x2_lt(a.low, b.low), .high = wasm_f64x2_lt(a.high, b.high)};}
static inline bool wasm_v256_any_true(v256_t a) {return wasm_v128_any_true(a.low) || wasm_v128_any_true(a.high);}
#define wasm_v64x4_shuffle(a, b, c0, c1, c2, c3) (v256_t){.low = wasm_v64x2_shuffle(a.low, b.high, c0, c1), .high = wasm_v64x2_shuffle(a.low, b.high, c2, c3)}

// 獲取無旋轉矩形頂點
static inline void _getAABB(RECT_PARAM(result), FLOAT x, FLOAT y, FLOAT w, FLOAT h) {
    FLOAT hw = w * f0_5, hh = h * f0_5;
    result[0].x = result[3].x = x - hw;
    result[1].y = result[0].y = y - hh;
    result[2].x = result[1].x = x + hw;
    result[3].y = result[2].y = y + hh;
}

// 獲取矩形頂點
static inline void _getOBB(RECT_PARAM(result), FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT r) {
    FLOAT hw = w * f0_5, hh = h * f0_5;
    FLOAT s = sin(r), c = cos(r);
    FLOAT hw_c = hw * c, hh_s = hh * s, hw_s = hw * s, hh_c = hh * c;
    result[0].x = x - hw_c + hh_s; result[0].y = y - hw_s - hh_c;
    result[1].x = x + hw_c + hh_s; result[1].y = y + hw_s - hh_c;
    result[2].x = x + hw_c - hh_s; result[2].y = y + hw_s + hh_c;
    result[3].x = x - hw_c - hh_s; result[3].y = y - hw_s + hh_c;
}

// 點繞圓旋轉
static inline void _rotateAround(FLOAT px[static restrict 1], FLOAT py[static restrict 1], FLOAT cx, FLOAT cy, FLOAT r) {
    FLOAT s = sin(r), c = cos(r);
    FLOAT x = *px - cx, y = *py - cy; // 平移到原點

    // 旋轉並平移回原位
    *px = x * c - y * s + cx;
    *py = x * s + y * c + cy;
}

// 計算矩形邊界
static inline Bounding _rectBounding(RECT_PARAM(rect)) {
    return (Bounding){
        .min = (Vector2){
            .x = fmin(fmin(rect[0].x, rect[1].x), fmin(rect[2].x, rect[3].x)),
            .y = fmin(fmin(rect[0].y, rect[1].y), fmin(rect[2].y, rect[3].y))
        },
        .max = (Vector2){
            .x = fmax(fmax(rect[0].x, rect[1].x), fmax(rect[2].x, rect[3].x)),
            .y = fmax(fmax(rect[0].y, rect[1].y), fmax(rect[2].y, rect[3].y))
        }
    };
}

// 包圍盒檢測
static inline bool _boundingCollision(RECT_PARAM(rect1), RECT_PARAM(rect2)) {
    Bounding a = _rectBounding(rect1);
    Bounding b = _rectBounding(rect2);
    return !(a.max.x < b.min.x || a.min.x > b.max.x || a.max.y < b.min.y || a.min.y > b.max.y);
}

static inline bool _AABBvsAABB(RECT_PARAM(rect1), RECT_PARAM(rect2)) {
    return !(rect1[1].x < rect2[0].x || rect1[0].x > rect2[1].x || rect1[0].y < rect2[2].y || rect1[2].y > rect2[0].y);
}

// AABB vs OBB SAT 判定，AABB 為 rect1(未旋轉)，OBB 為 rect2(已旋轉)
static inline bool _satAABBvsOBB(RECT_PARAM(rect1), RECT_PARAM(rect2)) {
    if (!_boundingCollision(rect1, rect2)) return false;

    // 使用 OBB 的邊產生分離軸
    Vector2 axes[2] = {
        {.x = -(rect2[1].y - rect2[0].y), .y = rect2[1].x - rect2[0].x},
        {.x = -(rect2[2].y - rect2[1].y), .y = rect2[2].x - rect2[1].x}
    };

#ifndef USE_FLOAT64
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
    // 交換相鄰行內的低兩個元素
    v128_t t0 = wasm_v32x4_shuffle(projs[0], projs[1], 0, 4, 1, 5);
    v128_t t1 = wasm_v32x4_shuffle(projs[0], projs[1], 2, 6, 3, 7);
    v128_t t2 = wasm_v32x4_shuffle(projs[2], projs[3], 0, 4, 1, 5);
    v128_t t3 = wasm_v32x4_shuffle(projs[2], projs[3], 2, 6, 3, 7);

    // 交叉合併上下兩組
    v128_t va = wasm_v32x4_shuffle(t0, t2, 0, 1, 4, 5);
    v128_t vb = wasm_v32x4_shuffle(t0, t2, 2, 3, 6, 7);
    v128_t vc = wasm_v32x4_shuffle(t1, t3, 0, 1, 4, 5);
    v128_t vd = wasm_v32x4_shuffle(t1, t3, 2, 3, 6, 7);
    v128_t vmin = wasm_f32x4_min(wasm_f32x4_min(va, vb), wasm_f32x4_min(vc, vd));
    v128_t vmax = wasm_f32x4_max(wasm_f32x4_max(va, vb), wasm_f32x4_max(vc, vd));

    v128_t cmp = wasm_f32x4_lt(vmax, wasm_v32x4_shuffle(vmin, vmin, 1, 0, 3, 2));
    return !wasm_v128_any_true(cmp);
#else
    v256_t vaxis0_x = wasm_f64x4_splat(axes[0].x);
    v256_t vaxis0_y = wasm_f64x4_splat(axes[0].y);
    v256_t vaxis1_x = wasm_f64x4_splat(axes[1].x);
    v256_t vaxis1_y = wasm_f64x4_splat(axes[1].y);

    v256_t vrect1_x = wasm_f64x4_make(rect1[0].x, rect1[1].x, rect1[2].x, rect1[3].x);
    v256_t vrect1_y = wasm_f64x4_make(rect1[0].y, rect1[1].y, rect1[2].y, rect1[3].y);
    v256_t vrect2_x = wasm_f64x4_make(rect2[0].x, rect2[1].x, rect2[2].x, rect2[3].x);
    v256_t vrect2_y = wasm_f64x4_make(rect2[0].y, rect2[1].y, rect2[2].y, rect2[3].y);

    v256_t projs[4] = {
        wasm_f64x4_add(wasm_f64x4_mul(vaxis0_x, vrect1_x), wasm_f64x4_mul(vaxis0_y, vrect1_y)),
        wasm_f64x4_add(wasm_f64x4_mul(vaxis0_x, vrect2_x), wasm_f64x4_mul(vaxis0_y, vrect2_y)),
        wasm_f64x4_add(wasm_f64x4_mul(vaxis1_x, vrect1_x), wasm_f64x4_mul(vaxis1_y, vrect1_y)),
        wasm_f64x4_add(wasm_f64x4_mul(vaxis1_x, vrect2_x), wasm_f64x4_mul(vaxis1_y, vrect2_y))
    };
    // 交換相鄰行內的低兩個元素
    v256_t t0 = wasm_v64x4_shuffle(projs[0], projs[1], 0, 4, 1, 5);
    v256_t t1 = wasm_v64x4_shuffle(projs[0], projs[1], 2, 6, 3, 7);
    v256_t t2 = wasm_v64x4_shuffle(projs[2], projs[3], 0, 4, 1, 5);
    v256_t t3 = wasm_v64x4_shuffle(projs[2], projs[3], 2, 6, 3, 7);

    // 交叉合併上下兩組
    v256_t va = wasm_v64x4_shuffle(t0, t2, 0, 1, 4, 5);
    v256_t vb = wasm_v64x4_shuffle(t0, t2, 2, 3, 6, 7);
    v256_t vc = wasm_v64x4_shuffle(t1, t3, 0, 1, 4, 5);
    v256_t vd = wasm_v64x4_shuffle(t1, t3, 2, 3, 6, 7);
    v256_t vmin = wasm_f64x4_min(wasm_f64x4_min(va, vb), wasm_f64x4_min(vc, vd));
    v256_t vmax = wasm_f64x4_max(wasm_f64x4_max(va, vb), wasm_f64x4_max(vc, vd));

    v256_t cmp = wasm_f64x4_lt(vmax, wasm_v64x4_shuffle(vmin, vmin, 1, 0, 3, 2));
    return !wasm_v256_any_true(cmp);
#endif
}

// 碰撞偵測(基於變換數據)
#if defined(USE_FLOAT64) && defined(__FAST_MATH__)
bool rectCollisionF64fast(RECT_COLLISION_PARAM) // f64-fast-math
#elif defined(USE_FLOAT64)
bool rectCollisionF64(RECT_COLLISION_PARAM) // f64
#elif defined(__FAST_MATH__)
bool rectCollisionF32fast(RECT_COLLISION_PARAM) // f32-fast-math
#else
bool rectCollisionF32(RECT_COLLISION_PARAM) // f32
#endif
{
    if (fabs(r1) > f1e_5) _rotateAround(&x2, &y2, x1, y1, -r1);

    RectVertex rect1, rect2;
    _getAABB(rect1, x1, y1, w1, h1);
    FLOAT newr2 = r2 - r1;
    int intRad = (int)round(newr2 * f10000);
    if (intRad % 15708 == 0) {
        int isSwapped = intRad % 31416 != 0;
        _getAABB(rect2, x2, y2, isSwapped ? h2 : w2, isSwapped ? w2 : h2);
        return _AABBvsAABB(rect1, rect2);
    }
    _getOBB(rect2, x2, y2, w2, h2, newr2);
    return _satAABBvsOBB(rect1, rect2);
}

/*
wat2wasm rectCollision.wat -o rectCollision.wasm
wasm-opt rectCollision.wasm -o rectCollision.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce
wasm2wat rectCollision.wasm -o rectCollision.wat
*/
