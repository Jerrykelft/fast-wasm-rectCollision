#include <cmath>
#include <array>
#include "simd.c"

#define inline __attribute__((__always_inline__)) inline

extern "C" {
    void sincosf(float x, float *s, float *c);
    void sincos(double x, double *s, double *c);
}
static inline void sincos(float x, float *s, float *c) {sincosf(x, s, c);}

// v128_t wasm_i32x4_shuffle_fake(v128_t, v128_t, int, int, int, int);
// #define wasm_i32x4_shuffle(a, b, c, d, e, f) wasm_i32x4_shuffle_fake(a, b, c, d, e, f)

#define FLOAT_TEMPLATE \
template <typename FLOAT> \
requires std::floating_point<FLOAT>


#define VECTOR_METHOD(T) \
static inline v128_t min(v128_t a, v128_t b) {return wasm_f32x4_min(a, b);} \
static inline v128_t min(T a, T b) {return min(a.data.vector, b.data.vector);} \
static inline v128_t max(v128_t a, v128_t b) {return wasm_f32x4_max(a, b);} \
static inline v128_t max(T a, T b) {return max(a.data.vector, b.data.vector);} \
static inline v128_t pmin(v128_t a, v128_t b) {return wasm_f32x4_pmin(a, b);} \
static inline v128_t pmin(T a, T b) {return pmin(a.data.vector, b.data.vector);} \
static inline v128_t pmax(v128_t a, v128_t b) {return wasm_f32x4_pmax(a, b);} \
static inline v128_t pmax(T a, T b) {return pmax(a.data.vector, b.data.vector);} \
union { \
    v128_t vector; \
    struct {float x, y, z, w;} scalar; \
    std::array<float, 4> arr; \
} data; \
inline T() {} \
inline T(v128_t v): data(v) {} \
inline T(const T& v): data(v.data.vector) {} \
inline T(const void *ptr): data(wasm_v128_load(ptr)) {} \
inline T(float splat): data(wasm_f32x4_splat(splat)) {} \
inline T& operator=(T other) {data.vector = other.data.vector; return *this;} \
inline T& operator=(v128_t v) {data.vector = v; return *this;} \
inline T operator-() const {return T(wasm_f32x4_neg(data.vector));} \
inline T operator+(T other) const {return T(wasm_f32x4_add(data.vector, other.data.vector));} \
inline T operator-(T other) const {return T(wasm_f32x4_sub(data.vector, other.data.vector));} \
inline T operator*(T other) const {return T(wasm_f32x4_mul(data.vector, other.data.vector));} \
inline T operator/(T other) const {return T(wasm_f32x4_div(data.vector, other.data.vector));} \
inline T& operator+=(T other) {data.vector = wasm_f32x4_add(data.vector, other.data.vector); return *this;} \
inline T& operator-=(T other) {data.vector = wasm_f32x4_sub(data.vector, other.data.vector); return *this;} \
inline T& operator*=(T other) {data.vector = wasm_f32x4_mul(data.vector, other.data.vector); return *this;} \
inline T& operator/=(T other) {data.vector = wasm_f32x4_div(data.vector, other.data.vector); return *this;} \
inline T operator+(v128_t v) const {return T(wasm_f32x4_add(data.vector, v));} \
inline T operator-(v128_t v) const {return T(wasm_f32x4_sub(data.vector, v));} \
inline T operator*(v128_t v) const {return T(wasm_f32x4_mul(data.vector, v));} \
inline T operator/(v128_t v) const {return T(wasm_f32x4_div(data.vector, v));} \
inline T& operator+=(v128_t v) {data.vector = wasm_f32x4_add(data.vector, v); return *this;} \
inline T& operator-=(v128_t v) {data.vector = wasm_f32x4_sub(data.vector, v); return *this;} \
inline T& operator*=(v128_t v) {data.vector = wasm_f32x4_mul(data.vector, v); return *this;} \
inline T& operator/=(v128_t v) {data.vector = wasm_f32x4_div(data.vector, v); return *this;} \
inline T operator+(float v) const {return T(wasm_f32x4_add(data.vector, wasm_f32x4_splat(v)));} \
inline T operator-(float v) const {return T(wasm_f32x4_sub(data.vector, wasm_f32x4_splat(v)));} \
inline T operator*(float v) const {return T(wasm_f32x4_mul(data.vector, wasm_f32x4_splat(v)));} \
inline T operator/(float v) const {return T(wasm_f32x4_div(data.vector, wasm_f32x4_splat(v)));} \
inline T& operator+=(float v) {data.vector = wasm_f32x4_add(data.vector, wasm_f32x4_splat(v)); return *this;} \
inline T& operator-=(float v) {data.vector = wasm_f32x4_sub(data.vector, wasm_f32x4_splat(v)); return *this;} \
inline T& operator*=(float v) {data.vector = wasm_f32x4_mul(data.vector, wasm_f32x4_splat(v)); return *this;} \
inline T& operator/=(float v) {data.vector = wasm_f32x4_div(data.vector, wasm_f32x4_splat(v)); return *this;} \
inline Vec4<int> operator==(T other) {return Vec4<int>(wasm_f32x4_eq(data.vector, other.data.vector));} \
inline Vec4<int> operator!=(T other) {return Vec4<int>(wasm_f32x4_ne(data.vector, other.data.vector));} \
inline Vec4<int> operator<=(T other) {return Vec4<int>(wasm_f32x4_le(data.vector, other.data.vector));} \
inline Vec4<int> operator>=(T other) {return Vec4<int>(wasm_f32x4_ge(data.vector, other.data.vector));} \
inline Vec4<int> operator<(T other) {return Vec4<int>(wasm_f32x4_lt(data.vector, other.data.vector));} \
inline Vec4<int> operator>(T other) {return Vec4<int>(wasm_f32x4_gt(data.vector, other.data.vector));} \
inline Vec4<int> operator==(v128_t v) {return Vec4<int>(wasm_f32x4_eq(data.vector, v));} \
inline Vec4<int> operator!=(v128_t v) {return Vec4<int>(wasm_f32x4_ne(data.vector, v));} \
inline Vec4<int> operator<=(v128_t v) {return Vec4<int>(wasm_f32x4_le(data.vector, v));} \
inline Vec4<int> operator>=(v128_t v) {return Vec4<int>(wasm_f32x4_ge(data.vector, v));} \
inline Vec4<int> operator<(v128_t v) {return Vec4<int>(wasm_f32x4_lt(data.vector, v));} \
inline Vec4<int> operator>(v128_t v) {return Vec4<int>(wasm_f32x4_gt(data.vector, v));} \
inline T abs() {return T(wasm_f32x4_abs(data.vector));} \
inline T sqrt() {return T(wasm_f32x4_sqrt(data.vector));} \
inline T ceil() {return T(wasm_f32x4_ceil(data.vector));} \
inline T floor() {return T(wasm_f32x4_floor(data.vector));} \
inline T nearest() {return T(wasm_f32x4_nearest(data.vector));} \
inline T trunc() {return T(wasm_f32x4_trunc(data.vector));} \
inline void sincos(v128_t *s, v128_t *c) {wasm_f32x4_sincos(data.vector, s, c);} \
inline T sin() {return T(wasm_f32x4_sin(data.vector));} \
inline T cos() {return T(wasm_f32x4_cos(data.vector));}

FLOAT_TEMPLATE struct Vec3;
FLOAT_TEMPLATE struct Quat;
FLOAT_TEMPLATE struct Mat3x3;
FLOAT_TEMPLATE struct RectData;

template <>
struct Vec3<float> {
    VECTOR_METHOD(Vec3)
    inline Vec3(float x, float y, float z): data(wasm_f32x4_make(x, y, z, 0.0f)) {}
    inline float sum() {return data.scalar.x + data.scalar.y + data.scalar.z;}

    inline float dot(Vec3 other) const {return (*this * other).sum();}
    float length() const {return ::sqrt((*this * *this).sum());}
    float length_sq() const {return (*this * *this).sum();}
};
template <>
struct Quat<float> {
    VECTOR_METHOD(Quat)
    inline Quat(float x, float y, float z, float w): data(wasm_f32x4_make(x, y, z, w)) {}
    inline float sum() {return data.scalar.x + data.scalar.y + data.scalar.z + data.scalar.w;}

    inline Quat qmul(Quat other) const {
        Quat vq1x = Quat(data.scalar.x) * wasm_f32x4_const(1.0f, -1.0f, 1.0f, -1.0f);
        Quat vq1y = Quat(data.scalar.y) * wasm_f32x4_const(1.0f, 1.0f, -1.0f, -1.0f);
        Quat vq1z = Quat(data.scalar.z) * wasm_f32x4_const(-1.0f, 1.0f, 1.0f, -1.0f);
        Quat vq1w(data.scalar.w);
        return vq1x * wasm_i32x4_shuffle(other.data.vector, other.data.vector, 3, 2, 1, 0) +
               vq1y * wasm_i32x4_shuffle(other.data.vector, other.data.vector, 2, 3, 0, 1) +
               vq1z * wasm_i32x4_shuffle(other.data.vector, other.data.vector, 1, 0, 3, 2) +
               vq1w * other.data.vector;
    }
    inline Quat operator^(Quat other) const {return this->qmul(other);}
    inline Quat operator^(Vec3<float> other) const {return this->qmul(Quat((other * wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)).data.vector));}
    inline Quat& operator^=(Quat other) {return *this = this->qmul(other);}
    inline Quat& operator^=(Vec3<float> other) {return *this = this->qmul(Quat((other * wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)).data.vector));}
};
template <>
struct Mat3x3<float>: std::array<Vec3<float>, 3> {
    inline Vec3<float> dot(Vec3<float> v) const {return Vec3<float>(((*this)[0] * v).sum(), ((*this)[1] * v).sum(), ((*this)[2] * v).sum());}

    inline Vec3<float> row(int i) const {return Vec3<float>((*this)[0].data.arr[i], (*this)[1].data.arr[i], (*this)[2].data.arr[i]);}
    inline Mat3x3 transpose() const {return {row(0), row(1), row(2)};}

    inline Mat3x3 operator*(Mat3x3 other) const {
        Mat3x3 result;
        for (int j = 0; j < 3; ++j) {
            result[j] = Vec3<float>(row(0).dot(other[j]), row(1).dot(other[j]), row(2).dot(other[j]));
        }
        return result;
    }
};
template <>
struct RectData<float> {
    Vec3<float> pos;
    Vec3<float> hsize;
    Mat3x3<float> rotate;
};

static inline void _rotateAround(Vec3<float> *p, Vec3<float> center, Mat3x3<float> r) {
    *p = r.dot(*p - center) + center;
}

static inline bool _testAxis(RectData<float> rect1, RectData<float> rect2, Vec3<float> L, Vec3<float> D) {
    if (L.length_sq() < 1e-6f) return true; 
    float rA = (rect1.hsize * rect1.rotate.dot(L)).abs().sum();
    float rB = (rect2.hsize * rect2.rotate.dot(L)).abs().sum();
    float proj = abs(D.dot(L));
    return proj <= (rA + rB);
}
static inline bool _collision(RectData<float> rect1, RectData<float> rect2) {
    Vec3<float> D = rect2.pos - rect1.pos;

    RectData<float> fixRect1 = {
        Vec3<float>(wasm_f32x4_const_splat(0.0f)),
        rect1.hsize,
        (Mat3x3<float>){
            Vec3<float>(wasm_f32x4_const(1.0f, 0.0f, 0.0f,  0.0f)),
            Vec3<float>(wasm_f32x4_const(0.0f, 1.0f, 0.0f,  0.0f)),
            Vec3<float>(wasm_f32x4_const(0.0f, 0.0f, 1.0f,  0.0f))
        }
    };
    _rotateAround(&D, fixRect1.pos, rect1.rotate.transpose() * rect2.rotate);

    for (int i = 0; i < 3; ++i) {
        if (!_testAxis(fixRect1, rect2, rect1.rotate[i], D)) return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!_testAxis(fixRect1, rect2, rect2.rotate[i], D)) return false;
    }
    // for (int i = 0; i < 3; ++i) {
    //     for (int j = 0; j < 3; ++j) {
    //         if (!_testAxis(rect1, rect2, rect1.rotate[i].cross(rect2.rotate[j]), D)) return false;
    //     }
    // }
    return true;
}

// struct Bounding {
//     Vec3 min, max;
//     inline Bounding() {}
//     inline Bounding(Vec3 _min, Vec3 _max): min(_min), max(_max) {}
// };
// // 四元數旋轉向量
// static inline Vec3<float> _quatRotateVec(Quat<float> q, Vec3<float> v) {
//     return Vec3<float>(((q ^ v) ^ (q * wasm_f32x4_const(-1.0f, -1.0f, -1.0f, 1.0f))).data.vector);
// }
// static inline Vec3<float> _quatRotateAround(Vec3<float> p, Vec3<float> center, Quat<float> q) {
//     return _quatRotateVec(q, p - center) + center;
// }
// static inline void _quatRotateAround(Vec3<float> *p, Vec3<float> center, Quat<float> q) {
//     *p = _quatRotateVec(q, *p - center) + center;
// }

// static inline RectVertex _getAABB(Vec3 center, Vec3 size) {
//     size *= wasm_f32x4_const_splat(0.5f);
//     static v128_t constant[8] = {
//         wasm_f32x4_const(-1.0f, -1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, -1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(-1.0f, 1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, 1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(-1.0f, -1.0f, 1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, -1.0f, 1.0f, 0.0f),
//         wasm_f32x4_const(-1.0f, 1.0f, 1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)
//     };
//     RectVertex result;
//     for (int i = 0; i < 8; i++) {
//         result[i] = center + size * constant[i];
//         // 將Vec3閒置的w向量賦值為z防止干擾_rectBounding判斷
//         result[i].data.vector = wasm_i32x4_shuffle(result[i].data.vector, result[i].data.vector, 0, 1, 2, 2);
//     }
//     return result;
// }

// static inline RectVertex _getOBB(Vec3 center, Vec3 size, Quat rotation) {
//     static v128_t constant[8] = {
//         wasm_f32x4_const(-1.0f, -1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, -1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(-1.0f, 1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, 1.0f, -1.0f, 0.0f),
//         wasm_f32x4_const(-1.0f, -1.0f, 1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, -1.0f, 1.0f, 0.0f),
//         wasm_f32x4_const(-1.0f, 1.0f, 1.0f, 0.0f),
//         wasm_f32x4_const(1.0f, 1.0f, 1.0f, 0.0f)
//     };
//     RectVertex result;
//     size *= wasm_f32x4_const_splat(0.5f);
//     for (int i = 0; i < 8; ++i) {
//         // 遍歷每個頂點符號組合: 0->-x, 1->+x
//         Vec3 local = size * constant[i];
//         Vec3 rotated = _quatRotateVec(rotation, local);
//         result[i] += rotated;
//         // 將Vec3閒置的w向量賦值為z防止干擾_rectBounding判斷
//         result[i].data.vector = wasm_i32x4_shuffle(result[i].data.vector, result[i].data.vector, 0, 1, 2, 2);
//     }
//     return result;
// }

// static inline Bounding _rectBounding(RectVertex rect) {
//     return Bounding(
//         Vec3::pmin(Vec3::pmin(Vec3::pmin(rect[0], rect[1]), Vec3::pmin(rect[2], rect[3])), Vec3::pmin(Vec3::pmin(rect[4], rect[5]), Vec3::pmin(rect[6], rect[7]))),
//         Vec3::pmax(Vec3::pmax(Vec3::pmax(rect[0], rect[1]), Vec3::pmax(rect[2], rect[3])), Vec3::pmax(Vec3::pmax(rect[4], rect[5]), Vec3::pmax(rect[6], rect[7])))
//     );
// }

// struct Vec3Scalar {float x, y, z;};
// typedef std::array<Vec3Scalar, 8> RectVertexScalar;
// struct BoundingScalar {
//     Vec3Scalar min, max;
//     inline BoundingScalar() {}
//     inline BoundingScalar(Vec3Scalar _min, Vec3Scalar _max): min(_min), max(_max) {}
// };

// static inline BoundingScalar _rectBounding(RectVertexScalar rect) {
//     return BoundingScalar(
//         Vec3Scalar{
//             fmin(fmin(fmin(rect[0].x, rect[1].x), fmin(rect[2].x, rect[3].x)), fmin(fmin(rect[4].x, rect[5].x), fmin(rect[6].x, rect[7].x))),
//             fmin(fmin(fmin(rect[0].y, rect[1].y), fmin(rect[2].y, rect[3].y)), fmin(fmin(rect[4].y, rect[5].y), fmin(rect[6].y, rect[7].y))),
//             fmin(fmin(fmin(rect[0].z, rect[1].z), fmin(rect[2].z, rect[3].z)), fmin(fmin(rect[4].z, rect[5].z), fmin(rect[6].z, rect[7].z))),
//         },
//         Vec3Scalar{
//             fmax(fmax(fmax(rect[0].x, rect[1].x), fmax(rect[2].x, rect[3].x)), fmax(fmax(rect[4].x, rect[5].x), fmax(rect[6].x, rect[7].x))),
//             fmax(fmax(fmax(rect[0].y, rect[1].y), fmax(rect[2].y, rect[3].y)), fmax(fmax(rect[4].y, rect[5].y), fmax(rect[6].y, rect[7].y))),
//             fmax(fmax(fmax(rect[0].z, rect[1].z), fmax(rect[2].z, rect[3].z)), fmax(fmax(rect[4].z, rect[5].z), fmax(rect[6].z, rect[7].z))),
//         }
//     );
// }

// static inline bool _boundingCollision(RectVertex rect1, RectVertex rect2) {
//     Bounding a = _rectBounding(rect1);
//     Bounding b = _rectBounding(rect2);
//     return wasm_i32x4_all_true(wasm_v128_and(a.min <= b.max, a.max >= b.min));
//     // return a.min.data.scalar.x <= b.max.data.scalar.x && a.max.data.scalar.x >= b.min.data.scalar.x &&
//     //        a.min.data.scalar.y <= b.max.data.scalar.y && a.max.data.scalar.y >= b.min.data.scalar.y &&
//     //        a.min.data.scalar.z <= b.max.data.scalar.z && a.max.data.scalar.z >= b.min.data.scalar.z;
// }

static inline Quat<float> _eulerToQuat(Vec3<float> v) {
    v128_t vs, vc;
    wasm_f32x4_sincos((v * wasm_f32x4_const_splat(0.5f)).data.vector, &vs, &vc);
    float cy = wasm_f32x4_extract_lane(vc, 2),
          sy = wasm_f32x4_extract_lane(vs, 2),
          cp = wasm_f32x4_extract_lane(vc, 1),
          sp = wasm_f32x4_extract_lane(vs, 1),
          cr = wasm_f32x4_extract_lane(vc, 0),
          sr = wasm_f32x4_extract_lane(vs, 0);
    /*
    sr * cp * cy - cr * sp * sy
    cr * sp * cy + sr * cp * sy
    cr * cp * sy - sr * sp * cy
    cr * cp * cy + sr * sp * sy
    */
    Quat<float> quat = Quat<float>(sr, cr, cr, cr) * Quat<float>(cp, sp, cp, cp) * Quat<float>(cy, cy, sy, cy) +
                       Quat<float>(cr, sr, sr, sr) * Quat<float>(sp, cp, sp, sp) * Quat<float>(sy, sy, cy, sy) * wasm_f32x4_const(-1.0f, 1.0f, -1.0f, 1.0f);
    quat /= wasm_f32x4_splat(sqrt((quat * quat).sum()));
    return quat;
}

// static inline bool _satAABBvsOBB(RectVertex rect1, RectVertex rect2) {
//     if (!_boundingCollision(rect1, rect2)) return false;
// }

// static inline bool _rectCollision(RectData rect1, RectData rect2) {
//     Quat nq1 = rect1.quat * wasm_f32x4_const(-1.0f, -1.0f, -1.0f, 1.0f);
//     _quatRotateAround(&rect2.pos, rect1.pos, nq1);
//     RectVertex rv1 = _getAABB(rect1.pos, rect1.size);

//     Quat newq2 = nq1 ^ rect2.quat;
//     return _satAABBvsOBB(rv1, _getOBB(rect2.pos, rect2.size, newq2));
// }

Mat3x3<float> mat = {
    Vec3<float>(1.0f, 0.0f, 0.0f),
    Vec3<float>(0.0f, 1.0f, 0.0f),
    Vec3<float>(0.0f, 0.0f, 1.0f)
};

extern "C" {
    // bool test1(float a) {
    //     return check_obb_collision(
    //         (RectData<float>){(Vec3<float>){a, a+1.0f, a+2.0f}, (Vec3<float>){a, a+3.0f, a+4.0f}, (Mat3x3<float>){(Vec3<float>){a, a+3.0f, a+4.0f}, (Vec3<float>){a, a+3.0f, a+4.0f}, (Vec3<float>){a, a+3.0f, a+4.0f}}},
    //         (RectData<float>){(Vec3<float>){a, a+11.0f, a+21.0f}, (Vec3<float>){a, a+31.0f, a+41.0f}, (Mat3x3<float>){(Vec3<float>){a, a+31.0f, a+41.0f}, (Vec3<float>){a, a+31.0f, a+41.0f}, (Vec3<float>){a, a+31.0f, a+114.514f}}}
    //     );
    // }
    void* get() {
        return &mat;
    }
    void make(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
        mat = {
            Vec3<float>(a, b, c),
            Vec3<float>(d, e, f),
            Vec3<float>(g, h, i)
        };
    }
    void t1() {
        // mat = mat.transpose1();
    }
    void t2() {
        // mat = mat.transpose2();
    }
    // void* test1(float a, float b, float c, float d) {
    //     static float result[8];
    //     v128_t r1, r2;
    //     wasm_f32x4_sincos(wasm_f32x4_make(a, b, c, d), &r1, &r2);
    //     wasm_v128_store(result, r1);
    //     wasm_v128_store(result + 4, r2);
    //     return result;
    // }
    // void* test2(float a, float b, float c, float d) {
    //     static float result[8];
    //     sincosf(a, result + 0, result + 4);
    //     sincosf(b, result + 1, result + 5);
    //     sincosf(c, result + 2, result + 6);
    //     sincosf(d, result + 3, result + 7);
    //     return result;
    // }
    // void* test1(float a, float b, float c) {
    //     static std::array<float, 4> result;
    //     result = _eulerToQuat(a, b, c);
    //     return &result;
    // }
    // void* test2(float a, float b, float c) {
    //     static Quat result;
    //     result = _eulerToQuat(Vec3(a, b, c));
    //     return &result;
    // }
}


/*
wasm2wat rectCollision3D.wasm -o rectCollision3D.wat
wasm-opt rectCollision3D.wasm -o rectCollision3D.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce
*/
