#ifndef NDEBUG
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <green/matrix.h>

void matrix_print(matrix_t m, const char *name)
{
    char indent[32] = "";

    // build indentation
    if (name != NULL && strlen(name) < 30) {
        const int depth = strlen(name) + 2;
        memset(indent, ' ', depth);
        indent[depth] = 0; // terminating '\0' character
        printf("%s: ", name);
    }

    // print matrix
    for (size_t i = 0; i < m.nrows * m.ncols; i += m.ncols) {
        for (size_t j = 0; j < m.ncols; j++) {
            printf("%d  ", m.values[i + j]);
        }
        printf("\r\n%s", indent);
    }
}
#endif // NDEBUG
