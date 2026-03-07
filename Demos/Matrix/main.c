#include "matrix.h"
#include <stdio.h>

int main() {
    float m1[3][3], m2[3][3], res[3][3];
    float pont[3] = {1.0f, 1.0f, 1.0f};

    init_identity_matrix(m1);
    print_matrix(m1);

    scalar_multiply(m1, 2.5f);
    print_matrix(m1);

    init_identity_matrix(m2);
    m2[0][2] = 5.0f;
    multiply_matrices(m1, m2, res);
    print_matrix(res);

    init_identity_matrix(res);
    scale(res, 2.0f, 3.0f);
    shift(res, 10.0f, -5.0f);
    rotate(res, 45.0f);
    print_matrix(res);

    transform_point(res, pont);
    printf("%f %f %f\n", pont[0], pont[1], pont[2]);

    return 0;
}