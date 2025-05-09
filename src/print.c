#include <stdio.h>
#include <stdlib.h>
#include "print.h"

void print_double_matrix(double **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.3lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_int_matrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_uchar_matrix(unsigned char **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", (int) matrix[i][j]);
        }
        printf("\n");
    }
}