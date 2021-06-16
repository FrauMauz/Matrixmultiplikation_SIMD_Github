#ifndef MEMORY_H
#define MEMORY_H

#ifdef FAST

#include <immintrin.h>

#define matrix_malloc(sz) _aligned_malloc(sz, _Alignof(__m128i))
#define matrix_realloc(ptr, sz) _aligned_realloc(ptr, sz, _Alignof(__m128i))
#define matrix_free(ptr) _aligned_free(ptr)

#else

#include <stdlib.h>

#define matrix_malloc(sz) malloc(sz)
#define matrix_realloc(ptr, sz) realloc(ptr, sz)
#define matrix_free(ptr) free(ptr)

#endif // FAST

#endif // MEMORY_H
