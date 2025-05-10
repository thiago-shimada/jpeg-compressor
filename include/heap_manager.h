#ifndef _HEAP_MANAGER_H
#define _HEAP_MANAGER_H

int **init_int_matrix(int rows, int cols);
void free_int_matrix(int **matrix, int rows);
double **init_double_matrix(int rows, int cols);
void free_double_matrix(double **matrix, int rows);
unsigned char **init_uchar_matrix(int rows, int cols);
void free_uchar_matrix(unsigned char **matrix, int rows);
double *init_double_array(int size);
double ****init_matrix_of_double_matrices(int rows, int cols);
void free_matrix_of_double_matrices(double ****matrix, int rows);

#endif