#ifndef MEMORY_H
#define MEMORY_H

#ifdef FAST

#include <immintrin.h>

#define matrix_malloc(sz) _aligned_malloc(sz, _Alignof(__m128i)) //_Alignof(_m128)=16 needed for simd operations
#define matrix_realloc(ptr, sz) _aligned_realloc(ptr, sz, _Alignof(__m128i)) 
#define matrix_free(ptr) _aligned_free(ptr)

#else

#include <stdlib.h>

#define matrix_malloc(sz) malloc(sz) //memory allocation (size), malloc returns pointer - adress of return needs to be a multple of 8
#define matrix_realloc(ptr, sz) realloc(ptr, sz) //if more space is needed, realloc returns pointer - adress of return needs to be a multple of 8
#define matrix_free(ptr) free(ptr) //make matrix free 

#endif // FAST

#endif // MEMORY_H
