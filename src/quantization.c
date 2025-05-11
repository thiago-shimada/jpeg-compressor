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
 * @brief Zigzag lookup table for scanning DCT coefficients
 *
 * This table is used to reorder the DCT coefficients in a zigzag pattern,
 * which is useful for encoding the coefficients in a more efficient manner.
 * The zigzag pattern helps to group low-frequency coefficients together,
 * which are typically more significant.
 */
int zigzag_table[DCT_BLOCK_SIZE * DCT_BLOCK_SIZE][2] = {
    {0, 0}, {0, 1}, {1, 0}, {2, 0}, {1, 1}, {0, 2}, {0, 3}, {1, 2},
    {2, 1}, {3, 0}, {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 5},
    {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0}, {6, 0}, {5, 1}, {4, 2},
    {3, 3}, {2, 4}, {1, 5}, {0, 6}, {0, 7}, {1, 6}, {2, 5}, {3, 4},
    {4, 3}, {5, 2}, {6, 1}, {7, 0}, {7, 1}, {6, 2}, {5, 3}, {4, 4},
    {3, 5}, {2, 6}, {1, 7}, {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3},
    {7, 2}, {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7}, {4, 7}, {5, 6},
    {6, 5}, {7, 4}, {7, 5}, {6, 6}, {5, 7}, {6, 7}, {7, 6}, {7, 7}
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

/**
 * @brief Zigzag scan of a block of DCT coefficients
 *
 * This function rearranges the DCT coefficients in a zigzag order, which is useful
 * for encoding the coefficients more efficiently. The zigzag pattern helps to group
 * low-frequency coefficients together.
 *
 * @param block Input 8x8 block of DCT coefficients
 * @param zigzag_array Output array to store the zigzag-scanned coefficients
 */
int *zigzag_scan(double **block) {
    int *zigzag_array = init_int_array(DCT_BLOCK_SIZE * DCT_BLOCK_SIZE);

    for (int i = 0; i < DCT_BLOCK_SIZE * DCT_BLOCK_SIZE; i++) {
        int row = zigzag_table[i][0];
        int col = zigzag_table[i][1];
        zigzag_array[i] = (int) round(block[row][col]);
    }

    return zigzag_array;
}

/**
 * @brief Inverse zigzag scan of a zigzag-ordered array
 *
 * This function rearranges the coefficients from a zigzag order back to their
 * original 8x8 block format. This is useful during the decompression process.
 * Also frees the zigzag_array after use.
 *
 * @param zigzag_array Input array of zigzag-scanned coefficients
 * @param block Output 8x8 block to store the rearranged coefficients
 */
 double **inverse_zigzag_scan(int *zigzag_array) {
    double **block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);

    for (int i = 0; i < DCT_BLOCK_SIZE * DCT_BLOCK_SIZE; i++) {
        int row = zigzag_table[i][0];
        int col = zigzag_table[i][1];
        block[row][col] = (double) zigzag_array[i];
    }

    return block;
}

/**
 * @brief Initializes a ZigzagMatrix structure
 *
 * This function allocates memory for the ZigzagMatrix structure and initializes
 * its dimensions and zigzag arrays for luminance and chrominance components.
 *
 * @param luminance_height Height of the luminance zigzag array
 * @param luminance_width Width of the luminance zigzag array
 * @param chrominance_height Height of the chrominance zigzag array
 * @param chrominance_width Width of the chrominance zigzag array
 * @return Pointer to the initialized ZigzagMatrix structure
 * @note The caller is responsible for freeing the allocated memory.
 */
ZigzagMatrix init_zigzag_matrix(int luminance_height, int luminance_width,
                            int chrominance_height, int chrominance_width) {
    ZigzagMatrix zigzag_matrix;
    zigzag_matrix.luminance_height = luminance_height;
    zigzag_matrix.luminance_width = luminance_width;
    zigzag_matrix.chrominance_height = chrominance_height;
    zigzag_matrix.chrominance_width = chrominance_width;

    zigzag_matrix.y_zigzag = init_matrix_of_int_arrays(luminance_height, luminance_width);
    zigzag_matrix.cb_zigzag = init_matrix_of_int_arrays(chrominance_height, chrominance_width);
    zigzag_matrix.cr_zigzag = init_matrix_of_int_arrays(chrominance_height, chrominance_width);

    return zigzag_matrix;
}

/**
 * @brief Frees memory allocated for a ZigzagMatrix structure
 *
 * This function frees the memory allocated for the zigzag matrix and its arrays in the
 * ZigzagMatrix structure.
 *
 * @param zigzag_matrix Pointer to the ZigzagMatrix structure to free
 */
void free_zigzag_matrix(ZigzagMatrix *zigzag_matrix) {
    for (int i = 0; i < zigzag_matrix->luminance_height; i++) {
        for (int j = 0; j < zigzag_matrix->luminance_width; j++) {
            free(zigzag_matrix->y_zigzag[i][j]);
        }
    }
    free_matrix_of_int_arrays(zigzag_matrix->y_zigzag, zigzag_matrix->luminance_height);

    for (int i = 0; i < zigzag_matrix->chrominance_height; i++) {
        for (int j = 0; j < zigzag_matrix->chrominance_width; j++) {
            free(zigzag_matrix->cb_zigzag[i][j]);
            free(zigzag_matrix->cr_zigzag[i][j]);
        }
    }
    free_matrix_of_int_arrays(zigzag_matrix->cb_zigzag, zigzag_matrix->chrominance_height);
    free_matrix_of_int_arrays(zigzag_matrix->cr_zigzag, zigzag_matrix->chrominance_height);

    zigzag_matrix->luminance_height = 0;
    zigzag_matrix->luminance_width = 0;
    zigzag_matrix->chrominance_height = 0;
    zigzag_matrix->chrominance_width = 0;
}

/**
 * @brief Converts DCT blocks to zigzag arrays
 *
 * This function takes DCT blocks and converts them into zigzag arrays for
 * luminance and chrominance components. It uses the zigzag scan function to
 * rearrange the coefficients.
 *
 * @param blocks DCTBlocks structure containing the 8x8 blocks for each channel
 * @return ZigzagMatrix structure containing the zigzag arrays
 */
ZigzagMatrix blocks_to_arrays(DCTBlocks blocks) {
    ZigzagMatrix zigzag_matrix = init_zigzag_matrix(blocks.luminance_height, blocks.luminance_width,
                                                    blocks.chrominance_height, blocks.chrominance_width);

    for (int i = 0; i < blocks.luminance_height; i++) {
        for (int j = 0; j < blocks.luminance_width; j++) {
            zigzag_matrix.y_zigzag[i][j] = zigzag_scan(blocks.y_blocks[i][j]);
        }
    }

    for (int i = 0; i < blocks.chrominance_height; i++) {
        for (int j = 0; j < blocks.chrominance_width; j++) {
            zigzag_matrix.cb_zigzag[i][j] = zigzag_scan(blocks.cb_blocks[i][j]);
            zigzag_matrix.cr_zigzag[i][j] = zigzag_scan(blocks.cr_blocks[i][j]);
        }
    }

    return zigzag_matrix;
}

/**
 * @brief Converts zigzag arrays back to DCT blocks
 *
 * This function takes zigzag arrays and converts them back into DCT blocks for
 * luminance and chrominance components. It uses the inverse zigzag scan function
 * to rearrange the coefficients.
 *
 * @param zigzag_matrix ZigzagMatrix structure containing the zigzag arrays
 * @return DCTBlocks structure containing the 8x8 blocks for each channel
 * @note The caller is responsible for freeing the allocated memory in the zigzag_matrix.
 */
DCTBlocks arrays_to_blocks(ZigzagMatrix zigzag_matrix) {
    DCTBlocks blocks = init_dct_blocks(zigzag_matrix.luminance_height, zigzag_matrix.luminance_width,
                                       zigzag_matrix.chrominance_height, zigzag_matrix.chrominance_width);

    for (int i = 0; i < blocks.luminance_height; i++) {
        for (int j = 0; j < blocks.luminance_width; j++) {
            blocks.y_blocks[i][j] = inverse_zigzag_scan(zigzag_matrix.y_zigzag[i][j]);
        }
    }

    for (int i = 0; i < blocks.chrominance_height; i++) {
        for (int j = 0; j < blocks.chrominance_width; j++) {
            blocks.cb_blocks[i][j] = inverse_zigzag_scan(zigzag_matrix.cb_zigzag[i][j]);
            blocks.cr_blocks[i][j] = inverse_zigzag_scan(zigzag_matrix.cr_zigzag[i][j]);
        }
    }

    return blocks;
}