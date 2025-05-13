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
                result[i][j] +=  (temp[i][k] * cosine_matrix[k][j]);
            }
            result[i][j] = round(result[i][j]);
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
 * @param block Input 8x8 block of double values
 * @return Pointer to a new 8x8 matrix with level-shifted double values
 */
double **level_shift(double **block) {
    double **shifted_block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            shifted_block[i][j] = block[i][j] - 128.0;
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
 * @return Pointer to a new 8x8 matrix with double pixel values
 */
double **unlevel_shift(double **block) {
    double **unshifted_block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            unshifted_block[i][j] = (block[i][j] + 128.0);
        }
    }

    return unshifted_block;
}

/**
 * @brief Initializes a DCTBlocks structure
 *
 * Allocates memory for the 3D arrays that will hold the DCT blocks for Y, Cb, and Cr channels.
 * Does not initialize the blocks themselves.
 *
 * @param y_block_rows Number of rows for Y blocks
 * @param y_block_cols Number of columns for Y blocks
 * @param c_block_rows Number of rows for Cb/Cr blocks
 * @param c_block_cols Number of columns for Cb/Cr blocks
 * @return Initialized DCTBlocks structure
 */
DCTBlocks init_dct_blocks(int y_block_rows, int y_block_cols, int c_block_rows, int c_block_cols) {
    DCTBlocks blocks;
    blocks.luminance_height = y_block_rows;
    blocks.luminance_width = y_block_cols;
    blocks.chrominance_height = c_block_rows;
    blocks.chrominance_width = c_block_cols;

    blocks.y_blocks = init_matrix_of_double_matrices(y_block_rows, y_block_cols);

    blocks.cb_blocks = init_matrix_of_double_matrices(c_block_rows, c_block_cols);

    blocks.cr_blocks = init_matrix_of_double_matrices(c_block_rows, c_block_cols);

    return blocks;
}

/**
 * @brief Frees memory allocated for DCTBlocks
 *
 * @param blocks Pointer to DCTBlocks structure to free
 */
void free_dct_blocks(DCTBlocks *blocks) {
    // Free Y blocks
    for (int i = 0; i < blocks->luminance_height; i++) {
        for (int j = 0; j < blocks->luminance_width; j++) {
            free_double_matrix(blocks->y_blocks[i][j], DCT_BLOCK_SIZE);
        }
    }
    free_matrix_of_double_matrices(blocks->y_blocks, blocks->luminance_height);
    
    // Free Cb blocks
    for (int i = 0; i < blocks->chrominance_height; i++) {
        for (int j = 0; j < blocks->chrominance_width; j++) {
            free_double_matrix(blocks->cb_blocks[i][j], DCT_BLOCK_SIZE);
        }
    }
    free_matrix_of_double_matrices(blocks->cb_blocks, blocks->chrominance_height);
    
    // Free Cr blocks
    for (int i = 0; i < blocks->chrominance_height; i++) {
        for (int j = 0; j < blocks->chrominance_width; j++) {
            free_double_matrix(blocks->cr_blocks[i][j], DCT_BLOCK_SIZE);
        }
    }
    free_matrix_of_double_matrices(blocks->cr_blocks, blocks->chrominance_height);

    blocks->chrominance_height = 0;
    blocks->chrominance_width = 0;
    blocks->luminance_height = 0;
    blocks->luminance_width = 0;
}

/**
 * @brief Creates an 8x8 block from the YCbCr image
 *
 * This function extracts an 8x8 block from the YCbCr image at the specified offsets.
 * It converts the pixel values to double for DCT processing.
 *
 * @param yoffset Y offset in the image
 * @param xoffset X offset in the image
 * @param image Pointer to the YCbCr image data
 * @return Pointer to a new 8x8 block of double values
 */
double **create_block(int yoffset, int xoffset, unsigned char **image) {
    double **block = init_double_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
    for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
        for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
            block[i][j] = (double)image[yoffset + i][xoffset + j];
        }
    }

    return block;
}

/**
 * @brief Divides a YCbCr_Image_420 into 8x8 blocks with level shifting
 *
 * This function takes a YCbCr_Image_420 and divides it into 8x8 blocks for DCT processing.
 * It assumes all images are multiple of 8, even after subsampling.
 *
 * @param ycbcr_image_420 Input YCbCr image with 4:2:0 subsampling
 * @return DCTBlocks structure containing the 8x8 blocks for each channel
 */
DCTBlocks divide_ycbcr_420_into_blocks(YCbCr_Image_420 ycbcr_image) {
    int luminance_height = ycbcr_image.luminance_height / 8;
    int luminance_width = ycbcr_image.luminance_width / 8;
    int chrominance_height = ycbcr_image.chrominance_height / 8;
    int chrominance_width = ycbcr_image.chrominance_width / 8;

    DCTBlocks blocks = init_dct_blocks(luminance_height, luminance_width, chrominance_height, chrominance_width);

    for (int i = 0; i < luminance_height; i++) {
        for (int j = 0; j < luminance_width; j++) {
            blocks.y_blocks[i][j] = create_block(i * DCT_BLOCK_SIZE, j * DCT_BLOCK_SIZE, ycbcr_image.y);
        }
    }

    for (int i = 0; i < chrominance_height; i++) {
        for (int j = 0; j < chrominance_width; j++) {
            blocks.cb_blocks[i][j] = create_block(i * DCT_BLOCK_SIZE, j * DCT_BLOCK_SIZE, ycbcr_image.cb);
            blocks.cr_blocks[i][j] = create_block(i * DCT_BLOCK_SIZE, j * DCT_BLOCK_SIZE, ycbcr_image.cr);
        }
    }

    return blocks;
}

/**
 * @brief Merges 8x8 blocks into a YCbCr_Image_420
 *
 * This function takes DCTBlocks and merges them back into a YCbCr_Image_420 format.
 * It assumes all images are multiple of 8, even after subsampling.
 *
 * @param blocks DCTBlocks structure containing the 8x8 blocks for each channel
 * @return Merged YCbCr_Image_420 structure
 */
YCbCr_Image_420 merge_blocks_into_ycbcr_420(DCTBlocks blocks) {
    YCbCr_Image_420 ycbcr_image;
    ycbcr_image.luminance_height = blocks.luminance_height * DCT_BLOCK_SIZE;
    ycbcr_image.luminance_width = blocks.luminance_width * DCT_BLOCK_SIZE;
    ycbcr_image.chrominance_height = blocks.chrominance_height * DCT_BLOCK_SIZE;
    ycbcr_image.chrominance_width = blocks.chrominance_width * DCT_BLOCK_SIZE;

    ycbcr_image.y = init_uchar_matrix(ycbcr_image.luminance_height, ycbcr_image.luminance_width);
    ycbcr_image.cb = init_uchar_matrix(ycbcr_image.chrominance_height, ycbcr_image.chrominance_width);
    ycbcr_image.cr = init_uchar_matrix(ycbcr_image.chrominance_height, ycbcr_image.chrominance_width);

    for (int i = 0; i < blocks.luminance_height; i++) {
        for (int j = 0; j < blocks.luminance_width; j++) {
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                for (int l = 0; l < DCT_BLOCK_SIZE; l++) {
                    double temp_y = round(blocks.y_blocks[i][j][k][l]);
                    ycbcr_image.y[i * DCT_BLOCK_SIZE + k][j * DCT_BLOCK_SIZE + l] = (unsigned char)(temp_y < 0 ? 0 : (temp_y > 255 ? 255 : temp_y));
                }
            }
        }
    }

    for (int i = 0; i < blocks.chrominance_height; i++) {
        for (int j = 0; j < blocks.chrominance_width; j++) {
            for (int k = 0; k < DCT_BLOCK_SIZE; k++) {
                for (int l = 0; l < DCT_BLOCK_SIZE; l++) {
                    double temp_cb = round(blocks.cb_blocks[i][j][k][l]);
                    double temp_cr = round(blocks.cr_blocks[i][j][k][l]);
                    ycbcr_image.cb[i * DCT_BLOCK_SIZE + k][j * DCT_BLOCK_SIZE + l] = (unsigned char)(temp_cb < 0 ? 0 : (temp_cb > 255 ? 255 : temp_cb));
                    ycbcr_image.cr[i * DCT_BLOCK_SIZE + k][j * DCT_BLOCK_SIZE + l] = (unsigned char)(temp_cr < 0 ? 0 : (temp_cr > 255 ? 255 : temp_cr));
                }
            }
        }
    }

    return ycbcr_image;
}