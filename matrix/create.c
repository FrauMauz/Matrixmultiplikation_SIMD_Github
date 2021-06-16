#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <green/matrix.h>

#include "memory.h"

#ifndef NDEBUG
#define MAX 10  // only allow integers i with 0 <= i < 10 when debugging
#else
#define MAX RAND_MAX
#endif // NDEBUG

matrix_t matrix_create_random(size_t m, size_t n)
{
    int *values = matrix_malloc(m * n * sizeof(int));
    for (size_t i = 0; i < m * n; i++) {
        values[i] = rand() % MAX;
    }
    return (matrix_t) { .nrows = m, .ncols = n, .values = values };
}

matrix_t matrix_load_file(const char *fpath)
{
    size_t nrows = 0;
    size_t limit = 1024;
    int *values = matrix_malloc(limit * sizeof(int));
    size_t pos = 0;

    FILE *file = fopen(fpath, "r");
    char buffer[4096]; // buffer for reading file
    size_t nchars = 0; // number of bytes read

    int state = 0; // 1 iff currently reading a number, otherwise 0
    int value = 0; // currently (partially) read integer

    while ((nchars = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        for (size_t i = 0; i < nchars; i++) {
            char c = buffer[i];
            if (isdigit(c)) {
                if (state == 0) {
                    if (pos == limit) {
                        limit *= 2;
                        values = matrix_realloc(values, limit * sizeof(int));
                    }
                    state = 1;
                }
                value = value * 10 + c - '0';
                continue;
            } else if (state == 1) {
                values[pos++] = value;
                value = state = 0;
            }
            if (c == '\n') {
                nrows++;
            }
        }
    }

    fclose(file);

    return (matrix_t) { .nrows = nrows, .ncols = pos / nrows, .values = values };
}

void matrix_destroy(matrix_t m)
{
    matrix_free(m.values);
}
