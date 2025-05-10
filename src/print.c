#include <stdio.h>
#include <stdlib.h>
#include "print.h"

/**
 * @brief prints a 2D matrix of double values
 *
 * This function iterates through the rows and columns of a 2D matrix
 * and prints each element formatted to three decimal places.
 * Each row is printed on a new line.
 *
 * @param matrix Pointer to the 2D matrix to be printed
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 */
void print_double_matrix(double **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.3lf ", matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief prints a 2D matrix of int values
 *
 * This function iterates through the rows and columns of a 2D matrix
 * and prints each element.
 * Each row is printed on a new line.
 *
 * @param matrix Pointer to the 2D matrix to be printed
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 */
void print_int_matrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief prints a 2D matrix of unsigned char values
 *
 * This function iterates through the rows and columns of a 2D matrix
 * and prints each element formatted to int.
 * Each row is printed on a new line.
 *
 * @param matrix Pointer to the 2D matrix to be printed
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 */
void print_uchar_matrix(unsigned char **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", (int) matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief prints a 1D array of double values
 *
 * This function iterates through the elements of a 1D array
 * and prints each element formatted to three decimal places.
 *
 * @param array Pointer to the 1D array to be printed
 * @param size Number of elements in the array
 */
void print_double_array(double *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%.3lf ", array[i]);
    }
    printf("\n");
}