#include <stdlib.h>
#include <stdio.h>
#include "heap_manager.h"

/**
 * @brief Initializes a 2D array of integers
 *
 * This function allocates memory for a 2D array of integers with the specified
 * number of rows and columns. Each row is dynamically allocated.
 *
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @return Pointer to the allocated 2D integer array
 */
int **init_int_matrix(int rows, int cols){
    int **matrix = (int **)malloc(rows * sizeof(int *));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *)malloc(cols * sizeof(int));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            exit(EXIT_FAILURE);
        }
    }

    return matrix;
}

/**
 * @brief Frees the memory allocated for a 2D integer array
 *
 * This function frees the memory previously allocated and sets the pointer to NULL.
 *
 * @param matrix Pointer to the 2D integer array to be freed
 * @param rows Number of rows in the matrix
 */
void free_int_matrix(int **matrix, int rows){
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;
}

/**
 * @brief Initializes a 2D array of double values
 *
 * This function allocates memory for a 2D array of double values with the specified
 * number of rows and columns. Each row is dynamically allocated.
 *
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @return Pointer to the allocated 2D double array
 */
double **init_double_matrix(int rows, int cols) {
    double **matrix = (double **)malloc(rows * sizeof(double *));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double *)malloc(cols * sizeof(double));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            exit(EXIT_FAILURE);
        }
    }

    return matrix;
}

/**
 * @brief Frees the memory allocated for a 2D double array
 *
 * This function frees the memory previously allocated and sets the pointer to NULL.
 *
 * @param matrix Pointer to the 2D double array to be freed
 * @param rows Number of rows in the matrix
 */
void free_double_matrix(double **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;
}

/**
 * @brief Initializes a 2D array of unsigned char values
 *
 * This function allocates memory for a 2D array of unsigned char values with the specified
 * number of rows and columns. Each row is dynamically allocated.
 *
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @return Pointer to the allocated 2D unsigned char array
 */
unsigned char **init_uchar_matrix(int rows, int cols) {
    unsigned char **matrix = (unsigned char **)malloc(rows * sizeof(unsigned char *));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (unsigned char *)malloc(cols * sizeof(unsigned char));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            exit(EXIT_FAILURE);
        }
    }

    return matrix;
}

/**
 * @brief Frees the memory allocated for a 2D unsigned char array
 *
 * This function frees the memory previously allocated and sets the pointer to NULL.
 *
 * @param matrix Pointer to the 2D unsigned char array to be freed
 * @param rows Number of rows in the matrix
 */
void free_uchar_matrix(unsigned char **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;
}

/**
 * @brief Initializes a 1D array of double values
 *
 * This function allocates memory for a 1D array of double values with the specified
 * size.
 *
 * @param size Size of the array
 * @return Pointer to the allocated 1D double array
 */
double *init_double_array(int size) {
    double *array = (double *)malloc(size * sizeof(double));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return array;
}

/**
 * @brief Frees the memory allocated for a matrix of double matrices
 * 
 * This function frees the memory previously allocated for a matrix of double matrices.
 * It does not allocate memory for each double matrix inside the matrix.
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 */
double ****init_matrix_of_double_matrices(int rows, int cols) {
    double ****matrix = (double ****)malloc(rows * sizeof(double ***));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double ***)malloc(cols * sizeof(double **));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            exit(EXIT_FAILURE);
        }
    }

    return matrix;
}

/**
 * @brief Frees the memory allocated for a matrix of double matrices
 *
 * This function frees the memory previously allocated for a matrix of double matrices.
 * It does not free the individual double matrices inside the matrix.
 *
 * @param matrix Pointer to the matrix of double matrices to be freed
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 */
void free_matrix_of_double_matrices(double ****matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;
}