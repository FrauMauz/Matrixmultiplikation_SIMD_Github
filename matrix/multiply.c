#include <stdlib.h>
#include <string.h>

#include <green/matrix.h>

#include "memory.h"

#ifdef FAST
#include <immintrin.h>

static inline int vector_dot_product_4x1(int *a0, int *b0)
{
    /*
     * The dot product of two 4-dimensional vectors a = (a0, a1, a2, a3) and b = (b0, b1, b2, b3)
     * is defined as a * b := a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3.
     *
     * This function operates as follows:
     *   (1) load vectors a and b into SIMD registers
     *   (2) perform the 4 pairwise multiplications
     *   (3) perform two horizontal additions to sum the 4 products
     *   (4) convert the lower 32 bits of the SIMD register into a signed integer
     */

    __m128i a = _mm_load_si128((__m128i *)a0);  // load four 32-bit signed integers
    __m128i b = _mm_load_si128((__m128i *)b0);
    __m128i products = _mm_mullo_epi32(a, b);   // pairwise multiplication of 32-bit signed integers
    __m128i sum = _mm_hadd_epi32(products, products);  // horizontal addition
    sum = _mm_hadd_epi32(sum, sum); // creates 4 times all sums multiplied
    return _mm_cvtsi128_si32(sum);  // convert lower 32 bits to a 32-bit signed imteger
}

static inline void matrix_transpose(size_t m, size_t n, int *A)
{
    for (size_t i = 0; i < m; i++) {
        for (size_t j = i + 1; j < n; j++) {
            // swap A[i, j] and A[j, i]
            int tmp = A[i * n + j];
            A[i * n + j] = A[j * n + i];
            A[j * n + i] = tmp;
        }
    }
}

matrix_t matrix_multiply(matrix_t a, matrix_t b)
{
    size_t a_m = a.nrows;
    size_t a_n = a.ncols;
    size_t b_n = b.ncols;

    int *A = a.values;
    int *B = matrix_malloc(b.nrows * b_n * sizeof(int));
    int *C = matrix_malloc(a_m * b_n * sizeof(int));

    memcpy(B, b.values, b.nrows * b_n * sizeof(int)); // preserve original matrix since matrix_transpose works in-place
    matrix_transpose(b.nrows, b_n, B); // to optimise cache usage when traversing matrix b

    memset(C, 0, a_m * b_n * sizeof(int));

    for (size_t i = 0; i < a_m; i++) {
        for (size_t j = 0; j < b_n; j++) {
            // process the row in 16-int chunks
            for (size_t k = 0; k + 15 < a_n; k += 16) {
                size_t offset = i * a_n + k;
                C[i * b_n + j] += vector_dot_product_4x1(&A[offset], &B[offset]);
                C[i * b_n + j] += vector_dot_product_4x1(&A[offset + 4], &B[offset + 4]);
                C[i * b_n + j] += vector_dot_product_4x1(&A[offset + 8], &B[offset + 8]);
                C[i * b_n + j] += vector_dot_product_4x1(&A[offset + 12], &B[offset + 12]);
            }
            // process the remainder (if any) in 4-int chunks
            for (size_t k = a_n & -16; k + 3 < a_n; k += 4) {
                C[i * b_n + j] += vector_dot_product_4x1(&A[i * a_n + k], &B[j * a_n + k]);
            }
            // process the remainder (if any) in 1-int chunks
            for (size_t k = a_n & -4; k < a_n; k++) {
                C[i * b_n + j] += A[i * a_n + k] * B[j * a_n + k];
            }
        }
    }

    matrix_free(B);

    return (matrix_t) { .nrows = a_m, .ncols = b_n, .values = C };
}
#else
matrix_t matrix_multiply(matrix_t a, matrix_t b)
{
    size_t a_m = a.nrows;
    size_t a_n = a.ncols;
    size_t b_n = b.ncols;

    int *A = a.values;
    int *B = b.values;
    int *C = matrix_malloc(a_m * b_n * sizeof(int));

    memset(C, 0, a_m * b_n * sizeof(int));

    for (size_t i = 0; i < a_m; i++) {
        for (size_t j = 0; j < b_n; j++) {
            for (size_t k = 0; k < a_n; k++) {
                C[i * b_n + j] += A[i * a_n + k] * B[k * b_n + j];
            }
        }
    }

    return (matrix_t) { .nrows = a_m, .ncols = b_n, .values = C };
}
#endif // FAST
