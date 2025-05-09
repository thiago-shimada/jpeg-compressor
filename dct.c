#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dct.h"
#include "color_convert.h"
#include "bitmap.h"
#include "heap_manager.h"

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

double **idct_2d(double **block, 
               double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]) {
    
    double** result = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    
    double temp[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    double temp_res[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    
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

double **level_shift(unsigned char **block) {
    double **shifted_block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            shifted_block[i][j] = (double)block[i][j] - 128.0;
        }
    }

    return shifted_block;
}

unsigned char **unlevel_shift(double **block) {
    unsigned char **unshifted_block = init_uchar_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            unshifted_block[i][j] = (unsigned char)(block[i][j] + 128.0);
        }
    }

    return unshifted_block;
}