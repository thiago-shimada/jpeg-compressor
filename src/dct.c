#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dct.h"
#include "color_convert.h"
#include "bitmap.h"
#include "heap_manager.h"

/**
 * @brief Computes the cosine matrix for DCT transformation
 *
 * This function generates the cosine coefficients needed for DCT (Discrete Cosine Transform)
 * calculations. For the first row (i=0), it uses a normalization factor, and for all other rows,
 * it calculates the cosine values according to the DCT formula.
 *
 * @param matrix 8x8 matrix to store the computed cosine values
 */
void compute_cosine_matrix(double matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]) {
    for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
            if(i == 0) {
                matrix[i][j] = sqrt(1.0 / DCT_BLOCK_SIZE);
            } else {
                matrix[i][j] = cos(((2*j+1)*i*M_PI)/16.0)/2.0;
            }
        }
    }
}

/**
 * @brief Performs 2D Discrete Cosine Transform on an 8x8 block
 *
 * This function applies the 2D DCT transformation to a given 8x8 block using
 * the precomputed cosine matrix. It transforms the spatial domain values into
 * frequency domain coefficients through matrix multiplication with the cosine matrix.
 *
 * @param block Input 8x8 block of pixel values
 * @param cosine_matrix Precomputed cosine coefficients
 * @return Pointer to a new 8x8 matrix containing DCT coefficients
 */
double** dct_2d(double** block, 
               double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]) {
    
    double** result = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    
    double temp[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                temp[i][j] += cosine_matrix[i][k] * block[k][j];
            }
        }
    }
    
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                result[i][j] += temp[i][k] * cosine_matrix[j][k];
            }
        }
    }
    
    return result;
}

/**
 * @brief Performs 2D Inverse Discrete Cosine Transform on an 8x8 block
 *
 * This function applies the 2D Inverse DCT to convert frequency domain coefficients
 * back to spatial domain pixel values. It uses the transpose of the cosine matrix
 * to reverse the DCT transformation.
 *
 * @param block Input 8x8 block of DCT coefficients
 * @param cosine_matrix Precomputed cosine coefficients
 * @return Pointer to a new 8x8 matrix containing pixel values in spatial domain
 */
double **idct_2d(double **block, 
               double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]) {
    
    double** result = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    
    double temp[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                temp[i][j] += cosine_matrix[k][i] * block[k][j];
            }
        }
    }

    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                result[i][j] += temp[i][k] * cosine_matrix[k][j];
            }
        }
    }
        
    return result;
}

/**
 * @brief Applies level shifting to pixel values before DCT
 *
 * This function shifts the pixel values from [0,255] range to [-128,127] range
 * by subtracting 128 from each pixel. This centering around zero improves
 * the DCT compression efficiency.
 *
 * @param block Input 8x8 block of unsigned char pixel values
 * @return Pointer to a new 8x8 matrix with level-shifted double values
 */
double **level_shift(unsigned char **block) {
    double **shifted_block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            shifted_block[i][j] = (double)block[i][j] - 128.0;
        }
    }

    return shifted_block;
}

/**
 * @brief Reverses level shifting after IDCT
 *
 * This function shifts the pixel values from [-128,127] range back to the [0,255] range
 * by adding 128 to each pixel. This reverses the level shift operation performed before DCT.
 *
 * @param block Input 8x8 block of level-shifted double values
 * @return Pointer to a new 8x8 matrix with unsigned char pixel values
 */
unsigned char **unlevel_shift(double **block) {
    unsigned char **unshifted_block = init_uchar_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            unshifted_block[i][j] = (unsigned char)(block[i][j] + 128.0);
        }
    }

    return unshifted_block;
}