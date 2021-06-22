#ifndef NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <green/matrix.h>

#ifndef _WIN32
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif // _WIN32

/* Gets minimum number of digits required to display any value in matrix m */
static inline int least_ndigits(matrix_t m)
{
    int n = 0;
    for (size_t i = 0; i < m.nrows * m.ncols; i++) {
        if (m.values[i] > n) {
            n = m.values[i];
        }
    }
    return log10(max(n, 1)) + 1;
}

void matrix_print(matrix_t m, const char *label)
{
    int indent = 0;
    int ndigits = least_ndigits(m);

    // build indentation
    if (label != NULL && strlen(label) > 0) {
        indent = strlen(label) + 2;
        printf("%s: ", label);
    }

    // print matrix
    for (size_t i = 0; i < m.nrows * m.ncols; i += m.ncols) {
        if (i > 0) {
            printf("%*s", indent, "");
        }
        for (size_t j = 0; j < m.ncols; j++) {
            int v = m.values[i + j];
            printf(" %-*d ", ndigits, v);
        }
        printf("\r\n");
    }
}
#endif // NDEBUG
