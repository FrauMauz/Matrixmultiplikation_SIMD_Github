#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

#ifdef FAST
#include <immintrin.h>
#endif

#ifndef NDEBUG
#define DIM 4   // number of rows and columns of the matrices
#define MAX 10  // only allow integers i with 0 <= i < 10 when debugging
#else
#define DIM 1024
#define MAX RAND_MAX
#endif

struct matrix {
    size_t nrows;
    size_t ncols;
    int *values;
};

enum init_strategy {
    UNSET,
    RANDOM,
    ZERO,
};

/**
 * Allocates an appropriate amount of memory and initializes the structure.
 */
static struct matrix matrix_create(size_t m, size_t n)
{
#ifdef FAST
    int *values = _aligned_malloc(m * n * sizeof(int), _Alignof(__m128i));
#else
    int *values = malloc(m * n * sizeof(int));
#endif
    return (struct matrix) { .nrows = m, .ncols = n, .values = values };
}

/**
 * Frees up resources to the system.
 */
static void matrix_destroy(struct matrix M)
{
#ifdef FAST
    _aligned_free(M.values);
#else
    free(M.values);
#endif
}

/**
 * Sets all entries to random values.
 */
static inline void matrix_fill_random(struct matrix M)
{
    for (size_t i = 0; i < M.nrows * M.ncols; i++) {
        M.values[i] = rand() % MAX;
    }
}

/**
 * Sets all entries to zero.
 */
static inline void matrix_fill_zero(struct matrix M)
{
    memset(M.values, 0, M.nrows * M.ncols * sizeof(int));
}

/**
 * Multiplies matrices A and B and stores the result in matrix C.
 *
 * All matrices are expected to be properly sized, initialized, and aligned to 128-bit boundaries.
 * That is, ncols(A) = nrows(B) and nrows(C) = nrows(A) and ncols(C) = ncols(B) and entries in C are zero.
 */
#define matrix_multiply(C, A, B) matrix_multiply_mxn(A.nrows, A.ncols, B.ncols, C.values, A.values, B.values)

#ifdef FAST
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

static inline void matrix_transpose_mxn(size_t m, size_t n, int *A)
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

static void matrix_multiply_mxn(size_t a_m, size_t a_n, size_t b_n, int *C, int *A, int *B)
{
    matrix_transpose_mxn(a_n, b_n, B);  // to optimise cache usage when traversing B
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
}
#else
static void matrix_multiply_mxn(size_t a_m, size_t a_n, size_t b_n, int *C, int *A, int *B)
{
    for (size_t i = 0; i < a_m; i++) {
        for (size_t j = 0; j < b_n; j++) {
            for (size_t k = 0; k < a_n; k++) {
                C[i * b_n + j] += A[i * a_n + k] * B[k * b_n + j];
            }
        }
    }
}
#endif

#ifndef NDEBUG
/**
 * Prints matrix M to stdout.
 */
static void matrix_print(struct matrix M)
{
    for (size_t i = 0; i < M.nrows * M.ncols; i += M.ncols) {
        for (size_t j = 0; j < M.ncols; j++) {
            printf("%d ", M.values[i + j]);
        }
        printf("\r\n");
    }
}
#endif

int main(int argc, char *argv[])
{
    struct matrix a = matrix_create(DIM, DIM);
    struct matrix b = matrix_create(DIM, DIM);
    struct matrix c = matrix_create(DIM, DIM);

    matrix_fill_random(a);
    matrix_fill_random(b);
    matrix_fill_zero(c);

    matrix_multiply(c, a, b);

#ifndef NDEBUG
    matrix_print(a);
    matrix_print(b);
    matrix_print(c);
#endif

    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(c);

    return EXIT_SUCCESS;
}
