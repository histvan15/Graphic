#include "matrix.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265f

void init_zero_matrix(float matrix[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = 0.0f;
        }
    }
}

void init_identity_matrix(float matrix[3][3]) {
    init_zero_matrix(matrix);
    for (int i = 0; i < 3; i++) {
        matrix[i][i] = 1.0f;
    }
}

void print_matrix(const float matrix[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void add_matrices(const float a[3][3], const float b[3][3], float c[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

void scalar_multiply(float matrix[3][3], float scalar) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] *= scalar;
        }
    }
}

void multiply_matrices(const float a[3][3], const float b[3][3], float res[3][3]) {
    float tmp[3][3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                tmp[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            res[i][j] = tmp[i][j];
        }
    }
}

void transform_point(const float matrix[3][3], float point[3]) {
    float res[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            res[i] += matrix[i][j] * point[j];
        }
    }
    for (int i = 0; i < 3; i++) {
        point[i] = res[i];
    }
}

void scale(float matrix[3][3], float sx, float sy) {
    float s_mat[3][3];
    init_identity_matrix(s_mat);
    s_mat[0][0] = sx;
    s_mat[1][1] = sy;
    multiply_matrices(s_mat, matrix, matrix);
}

void shift(float matrix[3][3], float tx, float ty) {
    float t_mat[3][3];
    init_identity_matrix(t_mat);
    t_mat[0][2] = tx;
    t_mat[1][2] = ty;
    multiply_matrices(t_mat, matrix, matrix);
}

void rotate(float matrix[3][3], float angle) {
    float r_mat[3][3];
    init_identity_matrix(r_mat);
    float rad = angle * (PI / 180.0f);
    r_mat[0][0] = cosf(rad);
    r_mat[0][1] = -sinf(rad);
    r_mat[1][0] = sinf(rad);
    r_mat[1][1] = cosf(rad);
    multiply_matrices(r_mat, matrix, matrix);
}