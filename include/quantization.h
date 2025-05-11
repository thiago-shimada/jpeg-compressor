#ifndef _QUANTIZATION_H
#define _QUANTIZATION_H

#include "dct.h"

#ifndef DCT_BLOCK_SIZE
#define DCT_BLOCK_SIZE 8
#endif

typedef enum {
    LUMINANCE,
    CHROMINANCE
} QuantizationType;

typedef struct {
    int luminance_height, luminance_width; // Dimensions of matrix of luminance zigzag arrays
    int chrominance_height, chrominance_width; // Dimensions of matrix of chrominance zigzag arrays
    int ***y_zigzag;   // Luminance zigzag arrays
    int ***cb_zigzag;  // Chrominance-blue zigzag arrays
    int ***cr_zigzag;  // Chrominance-red zigzag arrays
} ZigzagMatrix;

extern int luminance_quantization_table[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
extern int chrominance_quantization_table[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];

extern int zigzag_table[DCT_BLOCK_SIZE * DCT_BLOCK_SIZE][2];

double **quantize_block(double **block, double factor, QuantizationType type);
double **dequantize_block(double **block, double factor, QuantizationType type);
int *zigzag_scan(double **block);
double **inverse_zigzag_scan(int *zigzag_array);
ZigzagMatrix init_zigzag_matrix(int y_block_rows, int y_block_cols, int c_block_rows, int c_block_cols);
void free_zigzag_matrix(ZigzagMatrix *zigzag_matrix);
ZigzagMatrix blocks_to_arrays(DCTBlocks blocks);
DCTBlocks arrays_to_blocks(ZigzagMatrix zigzag_matrix);

#endif