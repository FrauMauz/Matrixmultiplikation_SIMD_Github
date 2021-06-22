#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include <green/matrix.h>

#include "malloc.h"

#ifndef NDEBUG
#define MAX 10  // only allow integers i with 0 <= i < 10 when debugging
#else
#define MAX RAND_MAX
#endif // NDEBUG

//for creating random matrices without console parameters
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
    /*
     * Note that this parser is complete but *not* sound
     * That is, it will accept all valid input but not reject all invalid input.
     */

    size_t limit = 1024;
    size_t pos = 0; // pos < limit

    size_t nrows = 0;
    size_t ncols = 0;
    int *values = matrix_malloc(limit * sizeof(int));

    size_t ncols_last = 0;

    FILE *file = fopen(fpath, "r"); // open in read mode 
    char buf[4096]; // buffer for reading file
    size_t nchars = 0; // number of bytes read into buf

    int sign = 0; //-1 negative number, 0 no number, 1 positive number
    int value = 0; //

    //at least 1 Byte was read, read 4KB - one page in RAM
    while ((nchars = fread(buf, 1, sizeof(buf), file)) > 0) {
        //1 Byte / 1 char processed
        for (size_t i = 0; i < nchars; i++) {
            char c = buf[i]; 
            if (isdigit(c)) {
                if (sign == 0) { // begin parsing non-negative integer
                    sign = 1;
                    value = c - '0';
                    if (pos == limit) {
                        limit *= 2;
                        values = matrix_realloc(values, limit * sizeof(int));
                    }
                } else { // continue parsing integer
                    value = value * 10 + c - '0';
                }
            } else if (c == '-') { // begin parsing negative integer
                sign = -1;
            } else if (sign != 0 || c == '\n') { // finish parsing integer
                if (sign != 0) {
                    values[pos++] = sign * value;
                    value = sign = 0;
                    ncols++;
                }
                if (c == '\n') {
                    nrows++;
                    ncols_last = ncols;
                    ncols = 0;
                }
            }
        }
    }

    fclose(file);

    // handle final value (if any)
    if (sign != 0) {
        values[pos] = sign * value;
        nrows++;
        ncols++;
    } else {
        ncols = ncols_last;
    }

    return (matrix_t) { .nrows = nrows, .ncols = ncols, .values = values };
}


//if malloc is used, also free needs to be defined
void matrix_destroy(matrix_t m)
{
    matrix_free(m.values);
}
