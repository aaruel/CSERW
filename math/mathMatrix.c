//
//  mathMatrix.c
//  CSERW
//

#include "mathMatrix.h"
#include <math.h>
#define PI 3.1415926535

/*
 Trying to make this as SIMD as possible!
 */

const mat4x4 matinit   = (mat4x4)  {1,0,0,0,
                                    0,1,0,0,
                                    0,0,1,0,
                                    0,0,0,1};

void mat_scalar(mat4x4 *m, float in) {
    __m128 am = _mm_load1_ps(&in);
    m->main.row.r0.simd = _mm_mul_ps(m->main.row.r0.simd, am);
    m->main.row.r1.simd = _mm_mul_ps(m->main.row.r1.simd, am);
    m->main.row.r2.simd = _mm_mul_ps(m->main.row.r2.simd, am);
    m->main.row.r3.simd = _mm_mul_ps(m->main.row.r3.simd, am);
}

void mat_mvproduct(mat4x4 *m, vec4 *v) {
    __m128 a, b;
    m->main.row.r0.simd = _mm_mul_ps(m->main.row.r0.simd, v->v1.v);
    m->main.row.r1.simd = _mm_mul_ps(m->main.row.r1.simd, v->v1.v);
    m->main.row.r2.simd = _mm_mul_ps(m->main.row.r2.simd, v->v1.v);
    m->main.row.r3.simd = _mm_mul_ps(m->main.row.r3.simd, v->v1.v);
    a = _mm_hadd_ps(m->main.row.r0.simd, m->main.row.r1.simd);
    b = _mm_hadd_ps(m->main.row.r2.simd, m->main.row.r3.simd);
    v->v1.v = _mm_hadd_ps(a, b);
}

void mat_transformation(mat4x4 *m, vec4 *v) {
    // much more efficient than typical mvproduct
    __m128 am = _mm_setr_ps(m->main.row.r0.iter[0],
                            m->main.row.r1.iter[1],
                            m->main.row.r2.iter[2],
                            m->main.row.r3.iter[3]);
    v->v1.v = _mm_mul_ps(am, v->v1.v);
}

void mat_multiply(mat4x4 *m1, mat4x4 *m0, mat4x4 *r) {
#define acc(column, row) (row*4)+column
    
///// SIMD multiplication
    
//    for(int i = 0; i < 4; ++i) {
//        __m128 brod1 = _mm_set1_ps(m0->main.iter[acc(0, i)]);
//        __m128 brod2 = _mm_set1_ps(m0->main.iter[acc(1, i)]);
//        __m128 brod3 = _mm_set1_ps(m0->main.iter[acc(2, i)]);
//        __m128 brod4 = _mm_set1_ps(m0->main.iter[acc(3, i)]);
//        __m128 row = _mm_add_ps(_mm_add_ps(
//                                    _mm_mul_ps(brod1, m1->main.row.r0.simd),
//                                    _mm_mul_ps(brod2, m1->main.row.r1.simd)),
//                                _mm_add_ps(
//                                    _mm_mul_ps(brod3, m1->main.row.r2.simd),
//                                    _mm_mul_ps(brod4, m1->main.row.r3.simd)));
//        _mm_store_ps(&r->main.iter[4*i], row);
//    }
    
///// Vanilla multiplication (naive/slowest)
    
//    for(int i = 0; i < 4; ++i) {
//        for(int j = 0; j < 4; ++j) {
//            r->main.iter[acc(i, j)] =   (m0->main.iter[acc(i, 0)]*m1->main.iter[acc(0, j)])+
//                                        (m0->main.iter[acc(i, 1)]*m1->main.iter[acc(1, j)])+
//                                        (m0->main.iter[acc(i, 2)]*m1->main.iter[acc(2, j)])+
//                                        (m0->main.iter[acc(i, 3)]*m1->main.iter[acc(3, j)]);
//        }
//    }
    
///// SIMDv2 multiplication (unrolled/fastest)
    
    const __m128 a = m1->main.row.r0.simd;
    const __m128 b = m1->main.row.r1.simd;
    const __m128 c = m1->main.row.r2.simd;
    const __m128 d = m1->main.row.r3.simd;
    
    __m128 t1, t2;
    
    t1 = _mm_set1_ps(m0->main.iter[0]);
    t2 = _mm_mul_ps(a, t1);
    t1 = _mm_set1_ps(m0->main.iter[1]);
    t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[2]);
    t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[3]);
    t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);
    
    _mm_store_ps(&r->main.iter[0], t2);
    
    t1 = _mm_set1_ps(m0->main.iter[4]);
    t2 = _mm_mul_ps(a, t1);
    t1 = _mm_set1_ps(m0->main.iter[5]);
    t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[6]);
    t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[7]);
    t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);
    
    _mm_store_ps(&r->main.iter[4], t2);
    
    t1 = _mm_set1_ps(m0->main.iter[8]);
    t2 = _mm_mul_ps(a, t1);
    t1 = _mm_set1_ps(m0->main.iter[9]);
    t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[10]);
    t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[11]);
    t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);
    
    _mm_store_ps(&r->main.iter[8], t2);
    
    t1 = _mm_set1_ps(m0->main.iter[12]);
    t2 = _mm_mul_ps(a, t1);
    t1 = _mm_set1_ps(m0->main.iter[13]);
    t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[14]);
    t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
    t1 = _mm_set1_ps(m0->main.iter[15]);
    t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);
    
    _mm_store_ps(&r->main.iter[12], t2);
}

/// Vector Operations

float fastinvsqrt(float x) {
    return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ps1(x) ) );
}

float vec_dot(vec4 *a, vec4 *b) {
    vec4 r;
    r.v1.v = _mm_mul_ps(a->v1.v, b->v1.v);
    return r.v1.a[0] + r.v1.a[1] + r.v1.a[2] + r.v1.a[3];
}

// only uses first 3 elements for obvious reasons
vec4 vec_cross(vec4 *a, vec4 *b) {
    vec4 r;
    r.v1.v =
    _mm_sub_ps( _mm_mul_ps(_mm_set_ps(0.f, a->v1.a[0], a->v1.a[2], a->v1.a[1]),
                           _mm_set_ps(0.f, b->v1.a[1], b->v1.a[0], b->v1.a[2])),
                _mm_mul_ps(_mm_set_ps(0.f, a->v1.a[1], a->v1.a[0], a->v1.a[2]),
                           _mm_set_ps(0.f, b->v1.a[0], b->v1.a[2], b->v1.a[1])));
    return r;
}

vec4 vec_normalize(vec4 val) {
    int eq = (val.v1.a[0]==0 && val.v1.a[1]==0 &&
              val.v1.a[2]==0 && val.v1.a[3]==0)? 1 : 0;
    return (eq) ? (vec4){0,0,0,0} : vec_mult1(val, fastinvsqrt(vec_dot(&val, &val)));
}

mat4x4 mat_lookat(vec4 cameraPosition, vec4 cameraTarget, vec4 cameraUp) {
    mat4x4 matrix = matinit;
    vec4 f = vec_normalize(vec_subtract(cameraTarget, cameraPosition));
    vec4 s = vec_normalize(vec_cross(&f, &cameraUp));
    vec4 u = vec_cross(&s, &f);
    matrix.main.row.r0.simd = _mm_set_ps(0.f, -f.v1.a[0], u.v1.a[0], s.v1.a[0]);
    matrix.main.row.r1.simd = _mm_set_ps(0.f, -f.v1.a[1], u.v1.a[1], s.v1.a[1]);
    matrix.main.row.r2.simd = _mm_set_ps(0.f, -f.v1.a[2], u.v1.a[2], s.v1.a[2]);
    matrix.main.row.r3.simd = _mm_set_ps(1.f, vec_dot(&f, &cameraPosition),
                                         -vec_dot(&u, &cameraPosition),
                                         -vec_dot(&s, &cameraPosition));
    
    return matrix;
}

float deg_to_rads(float a) {
    return (a*PI)/180;
}

mat4x4 mat_perspective(float FoV, float aspectRatio, float nearPlane, float farPlane) {
    float num = (float)tan((double)FoV*0.5f);
    return (mat4x4){(float)1/(aspectRatio*num), 0.f, 0.f, 0.f,
            0.f, (float)1/num, 0.f, 0.f,
            0.f, 0.f, -(farPlane+nearPlane)/(farPlane-nearPlane), -1.f,
            0.f, 0.f, -(2.f*farPlane*nearPlane)/(farPlane-nearPlane), 0.f};
}