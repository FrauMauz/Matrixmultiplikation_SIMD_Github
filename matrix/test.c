#include <stdlib.h>
#include <stdio.h>

#include <green/matrix.h>

#ifndef NDEBUG
#define DIM 4   // number of rows and columns of the matrices
#else
#define DIM 1024
#endif // NDEBUG

int main(int argc, char *argv[])
{
    int status;
    matrix_t a, b, c;

    if (argc < 2) {
        /* no matrices given */
        a = matrix_create_random(DIM, DIM);
        b = matrix_create_random(DIM, DIM);
    } else if (argc < 3) {
        /* one matrix given */
        a = matrix_load_file(argv[1]);
        b = matrix_load_file(argv[1]);
    } else {
        /* two matrices given */
        a = matrix_load_file(argv[1]);
        b = matrix_load_file(argv[2]);
    }

    if (a.ncols != b.nrows) {
        fprintf(stderr, "Cannot perform matrix multiplication: Incompatible dimensions.\r\n");
        status = EXIT_FAILURE;
        goto error;
    }

    c = matrix_multiply(a, b);
    status = EXIT_SUCCESS;

#ifndef NDEBUG
    matrix_print(a, "A");
    printf("\r\n");
    matrix_print(b, "B");
    printf("\r\n");
    matrix_print(c, "C");
    printf("\r\n");
#endif // NDEBUG

    matrix_destroy(c);
error:
    matrix_destroy(a);
    matrix_destroy(b);

    return status;
}
