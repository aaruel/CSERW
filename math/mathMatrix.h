//
//  mathMatrix.h
//  CSERW
//

#ifndef __CSERW__mathMatrix__
#define __CSERW__mathMatrix__

#include <xmmintrin.h>
#include <immintrin.h>

/**
 * 512 bit (4*4*32bit) SIMD style Matrix
 *
 * Each 128bit row is accessible through 32bit indexing or SIMD
 */
typedef struct {
    union {
        __m128 simd;
        float iter[4];
    } r0;
    union {
        __m128 simd;
        float iter[4];
    } r1;
    union {
        __m128 simd;
        float iter[4];
    } r2;
    union {
        __m128 simd;
        float iter[4];
    } r3;
}__internal_mat4x4;

/**
 * Main matrix allowing for linear indexing, row indexing, and SIMD
 */
typedef struct {
    union{
        float iter[16];
        __internal_mat4x4 row;
    }main;
} mat4x4;

/**
 * Vector allowing for indexing and SIMD
 */
typedef struct {
    union {
        float a[4];
        __m128 v;
    } v1;
} vec4;

/**
 * @discussion Matrix Scalar Product
 * @param m address of matrix
 * @param in scalar input
 * @return input matrix modified with the scalar product
 */
void mat_scalar(mat4x4 *m, float in);

/**
 * @discussion Matrix Vector Product
 * @param m address of matrix
 * @param v address of vector
 * @return input vector modified with mv product
 */
void mat_mvproduct(mat4x4 *m, vec4 *v);

/**
 * @discussion Matrix Multiplication
 * @param m0 address of matrix 0
 * @param m1 address of matrix 1
 * @param r address of return matrix
 * @return input matrix r
 */
void mat_multiply(mat4x4 *m0, mat4x4 *m1, mat4x4 *r);

/**
 * @discussion Vector Multiply Singular
 * @param a vector input
 * @param b float multiplicant
 * @return multiplied vector structure
 */
#define vec_mult1(a, b) ((vec4){.v1.v = _mm_mul_ps(a.v1.v, _mm_set1_ps(b))})

/**
 * @discussion Vector Multiply
 * @param a vector input
 * @param b vector multiplicant
 * @return multiplied vector structure
 */
#define vec_mult(a, b) ((vec4){.v1.v = _mm_mul_ps(a.v1.v, b.v1.v)})

/**
 * @discussion Vector Subtract
 * @param a vector input
 * @param b vector subtractor
 * @return multiplied vector structure
 */
#define vec_subtract(a, b) ((vec4){.v1.v = _mm_sub_ps(a.v1.v, b.v1.v)})

/**
 * @discussion Vector Add
 * @param a vector input
 * @param b vector adder
 * @return multiplied vector structure
 */
#define vec_add(a, b) ((vec4){.v1.v = _mm_add_ps(a.v1.v, b.v1.v)})

/**
 * @discussion Optimized Version of GLM LookAt
 * @param cameraPosition where the camera is (only first 3 values in vector considered)
 * @param cameraTarget vector where camera is pointed (only first 3 values in vector considered)
 * @param cameraUp vector pointed above cameraPosition (only first 3 values in vector considered)
 * @return processed matrix
 */
mat4x4 mat_lookat(vec4 cameraPosition, vec4 cameraTarget, vec4 cameraUp);

/**
 * @discussion Vector Cross Product
 * @param a vector 1 (only first 3 values in vector considered)
 * @param b vector 2 (only first 3 values in vector considered)
 * @return crossed vector
 */
vec4 vec_cross(vec4 *a, vec4 *b);

/**
 * @discussion Optimized Version of GLM Perspective
 * @param FoV Field of View
 * @param aspectRatio Aspect Ratio
 * @param nearPlane camera face
 * @param farPlane field plane
 * @return perspective matrix
 */
mat4x4 mat_perspective(float FoV, float aspectRatio, float nearPlane, float farPlane);

/**
 * @discussion Degrees to Radian
 * @param a degrees input
 * @return radian value
 */
float deg_to_rads(float a);

#endif /* defined(__CSERW__mathMatrix__) */
