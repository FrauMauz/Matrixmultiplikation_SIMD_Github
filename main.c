#include <stdlib.h>

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
#define DIM 256
#define MAX RAND_MAX
#endif

/*sizeof(A)=16*4=64 / sizeof(A[0])= 4 columns each 4 Byte = 16 */
#define matrix_nrows(A) (sizeof(A) / sizeof(A[0]))
/*sizeof(A[0])=first row 4 elements *4 Byte int size = 16 / sizeof(int) = 4 */
#define matrix_ncols(A) (sizeof(A[0]) / sizeof(int))

/**
 * Fills all entries of matrix A with values between 0 and MAX (exclusive).
 */
#define matrix_fill_random(A) matrix_fill_random_mxn(matrix_nrows(A), matrix_ncols(A), &A[0][0])

/**
 * Multiplies matrices A and B and stores the result in matrix C.
 *
 * All matrices are expected to be properly sized, initialized, and aligned to 128 boundaries.
 * That is, ncols(A) = nrows(B) and nrows(C) = nrows(A) and ncols(C) = ncols(B) and entries in C are zero.
 */
#define matrix_multiply(C, A, B) matrix_multiply_mxn(matrix_nrows(A), matrix_ncols(A), matrix_ncols(B), &C[0][0], &A[0][0], &B[0][0])

/**
 * Prints matrix A to stdout if not in release mode.
 */
#define matrix_print(A) matrix_print_mxn(matrix_nrows(A), matrix_ncols(A), &A[0][0])

static void matrix_fill_random_mxn(size_t m, size_t n, int *A)
{
    for (size_t i = 0; i < m * n; i++) {
        A[i] = rand() % MAX;
    }
}

#ifdef FAST
/**
 * Computes the dot product of two 4-dimensional vectors.
 */
static int vector_dot_product_4x1(int *a0, int *b0)
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

static void matrix_transpose_mxn(size_t m, size_t n, int *A)
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
            // process the first (a_n / 4) chunks of 4 items of the row
            for (size_t k = 0; k < a_n; k += 4) {
                C[i * b_n + j] += vector_dot_product_4x1(&A[i * a_n + k], &B[j * a_n + k]);
            }
            // process the remainder of the row (if any)
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

#ifdef NDEBUG
#define matrix_print_mxn(m, n, A) ((void)0)   // essentially a noop
#else
static void matrix_print_mxn(size_t m, size_t n, int *A)
{
    for (size_t i = 0; i < m * n; i += n) {
        for (size_t j = 0; j < n; j++) {
            printf("%d ", A[i + j]);
        }
        printf("\r\n");
    }
}
#endif

int main(int argc, char *argv[])
{
    /*
     * For sizeof(a) = sizeof(b) = sizeof(c) = DIM^2 * sizeof(int) and DIM = 256 = 2^8, the required space (in bytes)
     * amounts to 3 * (2^8)^2 * 4 = 3 * 2^18 = 786432, i.e. 0.75 MB, which is less than Windows' default stack size of 1 MB.
     * Since we only use a handful of additional integer variables, allocating the matrices on the stack is feasible.
     *
     * For matrices presumably exceeding the stack size, malloc memory on the heap instead!
     */

    _Alignas(__m128i) int a[DIM][DIM];
    _Alignas(__m128i) int b[DIM][DIM];
    _Alignas(__m128i) int c[DIM][DIM] = { 0 };

    matrix_fill_random(a);
    matrix_print(a);

    matrix_fill_random(b);
    matrix_print(b);

    matrix_multiply(c, a, b);
    matrix_print(c);

    return EXIT_SUCCESS;
}
