#ifndef _DCT_H
#define _DCT_H

#include "color_convert.h"

#define DCT_BLOCK_SIZE 8
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    int luminance_height, luminance_width; // Dimensions of matrix of luminance blocks
    int chrominance_height, chrominance_width; // Dimensions of matrix of chrominance blocks
    double ****y_blocks;   // Luminance blocks
    double ****cb_blocks;  // Chrominance-blue blocks
    double ****cr_blocks;  // Chrominance-red blocks
} DCTBlocks;

void compute_cosine_matrix(double matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]);
double **dct_2d(double **block, double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]);
double **idct_2d(double **block, double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE]);
double **level_shift(double **block);
double **unlevel_shift(double **block);

DCTBlocks init_dct_blocks(int y_block_rows, int y_block_cols, int c_block_rows, int c_block_cols);
void free_dct_blocks(DCTBlocks *blocks);
double **create_block(int yoffset, int xoffset, unsigned char **image);
DCTBlocks divide_ycbcr_420_into_blocks(YCbCr_Image_420 ycbcr_image);
YCbCr_Image_420 merge_blocks_into_ycbcr_420(DCTBlocks blocks);

#endif