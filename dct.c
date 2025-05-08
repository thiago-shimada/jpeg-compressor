#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dct.h"
#include "color_convert.h"
#include "bitmap.h"

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

double** dct_2d(unsigned char** block, 
               double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]) {
    
    // Allocate memory for result
    double** result = (double**)malloc(DCT_BLOCK_SIZE * sizeof(double*));
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        result[i] = (double*)malloc(DCT_BLOCK_SIZE * sizeof(double));
    }
    
    // Temporary matrix for intermediate result
    double temp[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    
    // First multiplication: temp = cosine_matrix * block
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                temp[i][j] += cosine_matrix[i][k] * (double) block[k][j];
            }
        }
    }
    
    // Second multiplication: result = temp * cosine_matrix.T
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                result[i][j] += temp[i][k] * cosine_matrix[j][k]; // Note: j,k instead of k,j for transpose
            }
        }
    }
    
    return result;
}

unsigned char **idct_2d(double **block, 
               double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]) {
    
    // Allocate memory for result
    unsigned char** result = (unsigned char**)malloc(DCT_BLOCK_SIZE * sizeof(unsigned char*));
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        result[i] = (unsigned char*)malloc(DCT_BLOCK_SIZE * sizeof(unsigned char));
    }
    
    // Temporary matrix for intermediate result
    double temp[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    double temp_res[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    
    // First multiplication: temp = cosine_matrix.T * block
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                temp[i][j] += cosine_matrix[k][i] * block[k][j];
            }
        }
    }
    
    // Second multiplication: result = temp * cosine_matrix
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            temp_res[i][j] = 0.0;
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                temp_res[i][j] += temp[i][k] * cosine_matrix[k][j]; // Note: j,k instead of k,j for transpose
            }
            result[i][j] = (unsigned char) temp_res[i][j];
        }
    }
        
    return result;
}

int main() {
    double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    compute_cosine_matrix(cosine_matrix);
    // Print the cosine matrix for verification
    // First, allocate the memory as you did
    unsigned char **test = (unsigned char **)malloc(DCT_BLOCK_SIZE * sizeof(unsigned char *));
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        test[i] = (unsigned char *)malloc(DCT_BLOCK_SIZE * sizeof(unsigned char));
    }

    // Now initialize the matrix with the given values
    unsigned char values[8][8] = {
        {135, 118, 236, 155,  32, 157,  48, 170},
        {212,  75,  22, 248, 117, 194, 250, 170},
        { 18, 166, 113,  18, 218, 245, 185, 137},
        { 98,  28, 162, 174,  29,  69, 123, 200},
        { 67, 161, 247, 223,  80, 223, 207, 208},
        {160, 207, 237,  57, 101,  59,  62,  90},
        {195,   5, 178,  68, 191, 250, 117,  85},
        { 56, 230, 169, 239, 116, 174, 217,  35}
    };

    // Copy values to test matrix
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            test[i][j] = values[i][j];
        }
    }

    for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
            printf("%.3lf ", cosine_matrix[i][j]);
        }
        printf("\n");
    }

    printf("\n\n");

    double **dct_block = dct_2d(test, cosine_matrix);

    for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
            printf("%.3lf ", dct_block[i][j]);
        }
        printf("\n");
    }

    for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
        free(test[i]);
    }
    free(test);

    test = idct_2d(dct_block, cosine_matrix);

    printf("\n\n");

    for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
            printf("%d ", (int) test[i][j]);
        }
        printf("\n");
    }

    for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
        free(test[i]);
        free(dct_block[i]);
    }
    free(test);
    free(dct_block);

    return 0;
}