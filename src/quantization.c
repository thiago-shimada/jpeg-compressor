#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "quantization.h"
#include "heap_manager.h"

/**
 * @brief Standard JPEG luminance quantization table
 *
 * This table is used for quantizing the luminance (Y) component's DCT coefficients.
 * Higher values lead to more aggressive quantization (more lossy compression).
 * Values are specially designed to preserve important visual information.
 */
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

/**
 * @brief Standard JPEG chrominance quantization table
 *
 * This table is used for quantizing the chrominance (Cb, Cr) components' DCT coefficients.
 * It's more aggressive than the luminance table since human eyes are less sensitive to
 * color details than brightness details. Higher values (99) in the table lead to
 * significant compression in the higher frequency components.
 */
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

/**
 * @brief Quantizes DCT coefficients using standard JPEG quantization tables
 *
 * This function divides each DCT coefficient by the corresponding value from the
 * appropriate quantization table (luminance or chrominance) multiplied by the
 * quality factor. Higher frequency components are typically quantized more aggressively.
 * The resulting values are rounded to the nearest integer.
 *
 * @param block Input 8x8 block of DCT coefficients
 * @param factor Quality factor to scale the quantization (higher value = more compression)
 * @param type LUMINANCE or CHROMINANCE to determine which quantization table to use
 * @return Pointer to a new 8x8 matrix with quantized coefficients
 */
double **quantize_block(double **block, double factor, QuantizationType type) {
    double **quantized_block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);

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

/**
 * @brief Dequantizes coefficients during decompression
 *
 * This function reverses the quantization process by multiplying each coefficient
 * by the corresponding value from the quantization table multiplied by the quality factor.
 *
 * @param block Input 8x8 block of quantized DCT coefficients
 * @param factor Quality factor used during quantization
 * @param type LUMINANCE or CHROMINANCE to determine which quantization table to use
 * @return Pointer to a new 8x8 matrix with dequantized DCT coefficients
 */
double **dequantize_block(double **block, double factor, QuantizationType type) {
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