#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "quantization.h"
#include "heap_manager.h"

int luminance_quantization_table[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {79, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
}; 

int chrominance_quantization_table[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}
};

int **quantize_block(double **block, double factor, QuantizationType type) {
    int **quantized_block = init_int_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);

    if(type == LUMINANCE) {
        for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
            for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
                quantized_block[i][j] = round(block[i][j] / (luminance_quantization_table[i][j] * factor));
            }
        }
    } else if(type == CHROMINANCE) {
        for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
            for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
                quantized_block[i][j] = round(block[i][j] / (chrominance_quantization_table[i][j] * factor));
            }
        }
    }

    return quantized_block;
}

double **dequantize_block(int **block, double factor, QuantizationType type) {
    double **dequantized_block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);

    if(type == LUMINANCE) {
        for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
            for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
                dequantized_block[i][j] = block[i][j] * (luminance_quantization_table[i][j] * factor);
            }
        }
    } else if(type == CHROMINANCE) {
        for(int i = 0; i < DCT_BLOCK_SIZE; i++) {
            for(int j = 0; j < DCT_BLOCK_SIZE; j++) {
                dequantized_block[i][j] = block[i][j] * (chrominance_quantization_table[i][j] * factor);
            }
        }
    }

    return dequantized_block;
}