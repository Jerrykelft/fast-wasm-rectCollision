#include <tgmath.h>
#include <wasm_simd128.h>

#define true 1
#define false 0
#define RECT_PARAM(__NAME__) Vector2 __NAME__[static restrict 4] // 矩形

typedef struct {float x, y;} Vector2; // 向量
typedef Vector2 RectVertex[4]; // 矩形頂點
typedef struct {Vector2 min, max;} Bounding; // 包圍盒

#ifdef __INTELLISENSE__
// 這段只給 VSCode IntelliSense 看
v128_t __builtin_wasm_shuffle_i8x16(v128_t, v128_t, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
#endif

// 獲取無旋轉矩形頂點
static inline void _getAABB(RECT_PARAM(result), float x, float y, float w, float h) {
    float hw = w * 0.5f, hh = h * 0.5f;
    result[0].x = result[3].x = x - hw;
    result[1].y = result[0].y = y - hh;
    result[2].x = result[1].x = x + hw;
    result[3].y = result[2].y = y + hh;
}

// 獲取矩形頂點
static inline void _getOBB(RECT_PARAM(result), float x, float y, float w, float h, float r) {
    float hw = w * 0.5f, hh = h * 0.5f;
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

// AABB vs OBB SAT 判定，AABB 為 rect1(未旋轉)，OBB 為 rect2(已旋轉)
static inline bool _satAABBvsOBB(RECT_PARAM(rect1), RECT_PARAM(rect2)) {
    if (!_boundingCollision(rect1, rect2)) return false;

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
}

// 碰撞偵測(基於變換數據)
bool rectCollision(
    float x1, float y1, float w1, float h1, float r1,
    float x2, float y2, float w2, float h2, float r2
) {
    if (fabs(r1) > 1e-5f) _pointRotateAround(&x2, &y2, x1, y1, -r1);

    RectVertex rect1, rect2;
    _getAABB(rect1, x1, y1, w1, h1);
    float newr2 = r2 - r1;
    int intRad = (int)round(newr2 * 10000.0f);
    if (intRad % 15708 == 0) {
        int isSwapped = intRad % 31416 != 0;
        _getAABB(rect2, x2, y2, isSwapped ? h2 : w2, isSwapped ? w2 : h2);
        return _boundingCollision(rect1, rect2);
    }
    _getOBB(rect2, x2, y2, w2, h2, newr2);
    return _satAABBvsOBB(rect1, rect2);
}

/*
emsdk_env
emcc -o rectCollision-f32-simd-tmp.wasm rectCollision-f32-simd.c --no-entry -O3 -msimd128 -flto -ffast-math -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS="['_rectCollision']" -s INITIAL_MEMORY=131072
wasm2wat rectCollision-f32-simd-tmp.wasm -o rectCollision-f32-simd-tmp.wat

wat2wasm rectCollision-f32-simd-tmp.wat -o rectCollision-f32-simd-opt-tmp.wasm
wasm-opt rectCollision-f32-simd-opt-tmp.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce -o rectCollision-f32-simd.wasm
wasm2wat rectCollision-f32-simd.wasm -o rectCollision-f32-simd.wat
*/
