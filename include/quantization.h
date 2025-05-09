#ifndef _QUANTIZATION_H
#define _QUANTIZATION_H

#ifndef DCT_BLOCK_SIZE
#define DCT_BLOCK_SIZE 8
#endif

typedef enum {
    LUMINANCE,
    CHROMINANCE
} QuantizationType;

extern int luminance_quantization_table[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
extern int chrominance_quantization_table[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];

extern int zigzag_table[DCT_BLOCK_SIZE * DCT_BLOCK_SIZE][2];

double **quantize_block(double **block, double factor, QuantizationType type);
double **dequantize_block(double **block, double factor, QuantizationType type);
double *zigzag_scan(double **block);
double **inverse_zigzag_scan(double *zigzag_array);

#endif