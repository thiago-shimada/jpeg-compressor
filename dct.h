#ifndef _DCT_H
#define _DCT_H

#define DCT_BLOCK_SIZE 8
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void compute_cosine_matrix();
double **dct_2d(unsigned char **block, double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]);
unsigned char **idct_2d(double **block, double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]);
#endif