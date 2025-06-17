#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "quantization.h"
#include "dct.h"
#include "heap_manager.h"
#include "color_convert.h"
#include "bitmap.h"
#include "print.h"
#include "time.h"
#include "ac_encode.h"
#include "dc_encode.h"
#include "huffman.h"

int main(int argc, char *argv[]) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
    compute_cosine_matrix(cosine_matrix);

    if (argc != 3) {
        printf("Usage: %s <input.bin> <output.bmp>\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("Error opening file: %s\n", argv[1]);
        return 1;
    }
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;

    load_bmp_header(fp, &file_header, &info_header);

    // entropy decoding
    Huffman_node *huffman_tree = create_huffman_tree();

    // // Initialize a bit reader for entropy decoding
    BitReader bit_reader;

    bitreader_init(&bit_reader, argv[1]);

    //jump the header

    bit_reader.file = fp;

    // Decode luminance blocks
    int previous_dc = 0; // Reset previous DC value

    int *block;

    
    int luminance_height = info_header.Width / DCT_BLOCK_SIZE;
    int luminance_width = info_header.Height / DCT_BLOCK_SIZE;

    int chrominance_height = (luminance_height / 2) + (luminance_height / 2) % 2;
    int chrominance_width = (luminance_width / 2) + (luminance_width / 2) % 2;

    ZigzagMatrix zigzag_matrix = init_zigzag_matrix(luminance_height, luminance_width, chrominance_height, chrominance_width);

    for (int i = 0; i < luminance_height; i++) {
        for (int j = 0; j < luminance_width; j++) {
            // Read the DC coefficient for the luminance block
            int dc_category = read_dc_category(&bit_reader);

            int mantissa = bitreader_read_bits(&bit_reader, dc_category);

            int diff_dc = decode_value(mantissa, dc_category);

            int current_dc = previous_dc + diff_dc; // Differential decoding
            previous_dc = current_dc; // Update previous DC value

            block = init_int_array(DCT_BLOCK_SIZE * DCT_BLOCK_SIZE);

            for (int x = 0; x < DCT_BLOCK_SIZE; x++) {
                for (int y = 0; y < DCT_BLOCK_SIZE; y++) {
                    block[x * DCT_BLOCK_SIZE + y] = 0;
                }
            }

            block[0] = current_dc;

            int pos = 1;

            while (pos < 64) {
                Huffman_node *node = read_ac_category(huffman_tree, &bit_reader);

                if (node == NULL) {
                    break; // Error in reading AC category
                }
                int run = node->run;
                int category = node->category;
                
                if (run == 0 && category == 0) {
                    break; // EOB
                }
                
                int ac_mantissa = bitreader_read_bits(&bit_reader, category);
                int ac_value = decode_value(ac_mantissa, category);

                for (int z = 0; z < run && pos < 64; z++) block[pos++] = 0;
                if (pos < 64) block[pos++] = ac_value;
            }
            zigzag_matrix.y_zigzag[i][j] = block;
        }
    }

    // Decode chrominance blocks
    int previous_dc_cb = 0;
    int previous_dc_cr = 0;

    for (int i = 0; i < chrominance_height; i++) {
        for (int j = 0; j < chrominance_width; j++) {
            // Read the DC coefficient for the chrominance blue block
            int dc_category = read_dc_category(&bit_reader);

            int mantissa = bitreader_read_bits(&bit_reader, dc_category);

            int diff_dc = decode_value(mantissa, dc_category);

            int current_dc = previous_dc_cb + diff_dc;

            previous_dc_cb = current_dc; // Update previous DC value

            block = init_int_array(DCT_BLOCK_SIZE * DCT_BLOCK_SIZE);

            for (int x = 0; x < DCT_BLOCK_SIZE; x++) {
                for (int y = 0; y < DCT_BLOCK_SIZE; y++) {
                    block[x * DCT_BLOCK_SIZE + y] = 0;
                }
            }
            
            block[0] = current_dc;

            int pos = 1;
            while (pos < 64) {
                Huffman_node *node = read_ac_category(huffman_tree, &bit_reader);
                
                
                if (node == NULL) {
                    break; // Error in reading AC category
                }
                int run = node->run;
                int category = node->category;
                
                if (run == 0 && category == 0) {
                    break; // EOB
                }
                
                int ac_mantissa = bitreader_read_bits(&bit_reader, category);
                int ac_value = decode_value(ac_mantissa, category);

                for (int z = 0; z < run && pos < 64; z++) block[pos++] = 0;
                if (pos < 64) block[pos++] = ac_value;

                
            }

            zigzag_matrix.cb_zigzag[i][j] = block;

            dc_category = read_dc_category(&bit_reader);

            mantissa = bitreader_read_bits(&bit_reader, dc_category);

            diff_dc = decode_value(mantissa, dc_category);

            current_dc = previous_dc_cr + diff_dc;

            previous_dc_cr = current_dc; // Update previous DC value

            block = init_int_array(DCT_BLOCK_SIZE * DCT_BLOCK_SIZE);

            for (int x = 0; x < DCT_BLOCK_SIZE; x++) {
                for (int y = 0; y < DCT_BLOCK_SIZE; y++) {
                    block[x * DCT_BLOCK_SIZE + y] = 0;
                }
            }

            block[0] = current_dc;

            pos = 1;

            while (pos < 64) {
                Huffman_node *node = read_ac_category(huffman_tree, &bit_reader);

                if (node == NULL) {
                    break; // Error in reading AC category
                }
                int run = node->run;
                int category = node->category;

                if (run == 0 && category == 0) {
                    break; // EOB
                }

                int ac_mantissa = bitreader_read_bits(&bit_reader, category);
                int ac_value = decode_value(ac_mantissa, category);

                for (int z = 0; z < run && pos < 64; z++) block[pos++] = 0;
                if (pos < 64) block[pos++] = ac_value;
            }
            zigzag_matrix.cr_zigzag[i][j] = block;
        }
    }

    // Convert zigzag arrays back to DCT blocks
    DCTBlocks quantized_blocks = arrays_to_blocks(zigzag_matrix);

    // Free the zigzag matrix
    free_zigzag_matrix(&zigzag_matrix);

    DCTBlocks blocks = init_dct_blocks(luminance_height, luminance_width, chrominance_height, chrominance_width);

    // Dequantize quantized_blocks to blocks
    for (int i = 0; i < quantized_blocks.luminance_height; i++) {
        for (int j = 0; j < quantized_blocks.luminance_width; j++) {
            // Dequantize the block
            double **dequantized_block = dequantize_block(quantized_blocks.y_blocks[i][j], 1.0, LUMINANCE);
            // Perform IDCT on the dequantized block
            double **idct_block = idct_2d(dequantized_block, cosine_matrix);
            // Unlevel shift the block
            double **unshifted_block = unlevel_shift(idct_block);
            // Store the unlevel shifted block
            blocks.y_blocks[i][j] = unshifted_block;
            // Free the dequantized block
            free_double_matrix(dequantized_block, DCT_BLOCK_SIZE);
            // Free the IDCT block
            free_double_matrix(idct_block, DCT_BLOCK_SIZE);
        }
    }

    for (int i = 0; i < quantized_blocks.chrominance_height; i++) {
        for (int j = 0; j < quantized_blocks.chrominance_width; j++) {
            // Dequantize the block
            double **dequantized_block_cb = dequantize_block(quantized_blocks.cb_blocks[i][j], 1.0, CHROMINANCE);
            double **dequantized_block_cr = dequantize_block(quantized_blocks.cr_blocks[i][j], 1.0, CHROMINANCE);
            // Perform IDCT on the dequantized block
            double **idct_block_cb = idct_2d(dequantized_block_cb, cosine_matrix);
            double **idct_block_cr = idct_2d(dequantized_block_cr, cosine_matrix);
            // Unlevel shift the block
            double **unshifted_block_cb = unlevel_shift(idct_block_cb);
            double **unshifted_block_cr = unlevel_shift(idct_block_cr);
            // Store the IDCT block
            blocks.cb_blocks[i][j] = unshifted_block_cb;
            blocks.cr_blocks[i][j] = unshifted_block_cr;
            // Free the dequantized block
            free_double_matrix(dequantized_block_cb, DCT_BLOCK_SIZE);
            free_double_matrix(dequantized_block_cr, DCT_BLOCK_SIZE);
            // Free the IDCT block
            free_double_matrix(idct_block_cb, DCT_BLOCK_SIZE);
            free_double_matrix(idct_block_cr, DCT_BLOCK_SIZE);
        }
    }
    // Free the quantized blocks
    free_dct_blocks(&quantized_blocks);

    // Create YCbCr_420 structure
    YCbCr_Image_420 subsampled_image = init_ycbcr_image_420();

    // Merge the blocks back into a YCbCr image
    subsampled_image = merge_blocks_into_ycbcr_420(blocks);

    // Free the blocks
    free_dct_blocks(&blocks);


    YCbCr_Image ycbcr_image = init_ycbcr_image();

    // Upsample the YCbCr_420 image to YCbCr
    ycbcr_upsampling_420(&ycbcr_image, subsampled_image);
    // Free the YCbCr_420 image
    free_ycbcr_image_420(&subsampled_image);
    // Convert YCbCr to RGB
    RGB_Image rgb_image = init_rgb_image();

    ycbcr_to_rgb(&rgb_image, ycbcr_image);
    // Free the YCbCr image
    free_ycbcr_image(&ycbcr_image);
    // Save the RGB image to a new BMP file
    save_rgb_image(argv[2], rgb_image, &file_header, &info_header);
    // Free the RGB image
    free_rgb_image(&rgb_image);

    end = clock(); // Record end time

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // Calculate execution time

    printf("Time taken: %f seconds\n", cpu_time_used);
    
    return 0;
}