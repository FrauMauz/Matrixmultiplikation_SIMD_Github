#include <stdlib.h>
#include <string.h>

#include <green/matrix.h>

#include "malloc.h"

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

    //_mm_load_si128((__m128i *) : Load 128-bits of integer data from memory into dst. mem_addr must be aligned on a 16-byte boundary or a general-protection exception may be generated
    __m128i a = _mm_load_si128((__m128i *)a0);
    __m128i b = _mm_load_si128((__m128i *)b0);

    //__m128i _mm_mullo_epi32 (__m128i a, __m128i b) : Multiply the packed 32-bit integers in a and b, producing intermediate 64-bit integers, and store the low 32 bits of the intermediate integers in dst
    __m128i products = _mm_mullo_epi32(a, b);   // pairwise multiplication of 32-bit signed integers

    //__m128i _mm_hadd_epi32 (__m128i a, __m128i b) : Horizontally add adjacent pairs of 32-bit integers in a and b, and pack the signed 32-bit results in dst
    __m128i sum = _mm_hadd_epi32(products, products);  // horizontal addition
    sum = _mm_hadd_epi32(sum, sum); // creates 4 times all sums multiplied

    //_mm_cvtsi128_si32 (__m128i a) Copy the lower 32-bit integer in a to dst
    return _mm_cvtsi128_si32(sum);  // convert lower 32 bits to a 32-bit signed integer
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
                //unroll
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



/*
* TESTED on Intel's CPU i5 11.Gen Tigerlake, 2,4 GHz
* 22.06.2021, 19 C°
*
* TEST FAST n= 1024, RELEASE x64
*
Profiled Entity / Type                   Power (mW)       Energy (mJ)
                                         11445.789        17713.501

Function / Call Stack                    CPU Time         Clockticks       Instructions Retired     CPI Rate
[Loop at line 63 in matrix_multiply]     0.453s           1,874,400,000    6,727,200,000            0.279


* TEST NDEBUG n= 1024, RELEASE x64
Profiled Entity / Type                   Power (mW)       Energy (mJ)
                                         10820.214        55503.845

Function / Call Stack                    CPU Time         Clockticks        Instructions Retired    CPI Rate
[Loop at line 100 in matrix_multiply]    4.962s           20,613,600,000    30,062,400,000          0.686    28.3%

*/





/*
    CPU                Intel(R) Processor code named Kabylake ULX
    Frequency:         1,8 GHz
    Local CPU count:   8

    22.06.2021, 22 C°

    -------------------------------------
    Sequentielle Implementierung
    -------------------------------------

    TESTEN mit x64 Release

Function / Call Stack	                    CPU Time	Clockticks	    Instructions Retired	CPI Rate	Average CPU Frequency
[Loop at line 99 in matrix_multiply]	    17.192s	    50,550,400,000	30,155,200,000	        1.676	    2.9 GHz
[Loop at line 20 in matrix_create_random]	0.014s	    52,800,000	    180,800,000	            0.292	    3.7 GHz
[Loop at line 98 in matrix_multiply]	    0.004s	    6,400,000	    4,800,000	            1.333	    1.4 GHz

    --------------------------------------
    SIMD Implementierung mit Unroll
    --------------------------------------

    TESTEN mit x64 Release FAST

Function / Call Stack	                    CPU Time	C   lockticks	    Instructions Retired	CPI Rate	Average CPU Frequency
vector_dot_product_4x1	                  1.669s	    5,624,000,000	  9,488,000,000	        0.593	    3.4 GHz
[Loop at line 62 in matrix_multiply]	    0.526s	    1,835,200,000	  3,155,200,000	        0.582	    3.5 GHz
[Loop at line 20 in matrix_create_random]	0.015s	    57,600,000	    195,200,000	          0.295	    3.8 GHz
[Loop at line 35 in matrix_transpose]	    0.008s	    27,200,000	    16,000,000	          1.700	    3.4 GHz
[Loop at line 60 in matrix_multiply]	    0.003s	    3,200,000	      8,000,000	            0.400	    1.2 GHz
[Loop at line 70 in matrix_multiply]	    0.001s	    0	              3,200,000	            0.000	    0.0 MHz
[Loop at line 74 in matrix_multiply]	    0s	        0	              0	                    0.000	    0.0 MHz


    --------------------------------------
    SIMD Implementierung ohne Unroll
    --------------------------------------

    TESTEN mit x64 Release FAST

    Function / Call Stack	                    CPU Time	  Clockticks	    Instructions Retired	CPI Rate	Average CPU Frequency
    vector_dot_product_4x1	                  1.689s	    5,720,000,000	  10,185,600,000	      0.562     3.4 GHz
    [Loop at line 87 in matrix_multiply]	    0.736s	    2,505,600,000	  4,870,400,000	        0.514	    3.4 GHz
    [Loop at line 19 in matrix_create_random]	0.018s	    36,800,000	    161,600,000	          0.228	    2.1 GHz
    [Loop at line 42 in matrix_transpose]	    0.006s	    22,400,000	    14,400,000	          1.556	    3.6 GHz
    [Loop at line 67 in matrix_multiply]	    0.001s	    3,200,000	      4,800,000	            0.667	    3.6 GHz
*/
