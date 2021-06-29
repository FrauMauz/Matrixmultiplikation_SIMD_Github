#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef struct {
    size_t nrows;
    size_t ncols;
    int *values;
} matrix_t;

/* Allocates an appropriate amount of memory and initializes the structure with random values. */
matrix_t matrix_create_random(size_t m, size_t n);

/* Reads a matrix from a text file into main memory. */
matrix_t matrix_load_file(const char *fpath);

/* Returns resources to the system. */
void matrix_destroy(matrix_t m);

/* Multiplies matrices a and b and returns their product. */
matrix_t matrix_multiply(matrix_t a, matrix_t b);

#ifndef NDEBUG
/* Prints matrix m to stdout. A label may be provided if desired. */
void matrix_print(matrix_t m, const char *label);
#endif // NDEBUG

#endif // MATRIX_H
