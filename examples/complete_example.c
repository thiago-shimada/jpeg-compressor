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
        printf("Usage: %s <input.bmp> <output.bmp>\n", argv[0]);
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
    // Create RGB image structure
    RGB_Image rgb_image = init_rgb_image();
    // Read the RGB image data from the BMP file
    read_rgb_image(&rgb_image, fp, file_header, info_header);
    fclose(fp);
    // Create YCbCr image structure
    YCbCr_Image ycbcr_image = init_ycbcr_image();
    // Convert RGB to YCbCr
    rgb_to_ycbcr(&ycbcr_image, rgb_image);
    // We can free the original RGB image now
    free_rgb_image(&rgb_image);
    // Create YCbCr_420 structure
    YCbCr_Image_420 subsampled_image = init_ycbcr_image_420();
    // Apply chroma subsampling (4:2:0)
    ycbcr_subsampling_420(&subsampled_image, ycbcr_image);
    // Free the original YCbCr image
    free_ycbcr_image(&ycbcr_image);
    // Divide the YCbCr image into 8x8 blocks
    DCTBlocks blocks = divide_ycbcr_420_into_blocks(subsampled_image);
    // Free the subsampled image
    free_ycbcr_image_420(&subsampled_image);

    // Process each block for DCT and quantization
    DCTBlocks quantized_blocks = init_dct_blocks(blocks.luminance_height, blocks.luminance_width, blocks.chrominance_height, blocks.chrominance_width);

    // Quantize the luminance blocks
    for (int i = 0; i < blocks.luminance_height; i++) {
        for (int j = 0; j < blocks.luminance_width; j++) {
            // Perform DCT on the luminance block
            // Level shift the block
            double **block = level_shift(blocks.y_blocks[i][j]);
            // Perform DCT on the shifted block
            double **dct_block = dct_2d(block, cosine_matrix);
            // Quantize the DCT coefficients
            double **quantized_block = quantize_block(dct_block, 1.0, LUMINANCE);
            // Store the quantized block
            quantized_blocks.y_blocks[i][j] = quantized_block;
            // Free the DCT block
            free_double_matrix(dct_block, DCT_BLOCK_SIZE);
            // Free the shifted block
            free_double_matrix(block, DCT_BLOCK_SIZE);
        }
    }

    // Quantize the chrominance blocks
    for (int i = 0; i < blocks.chrominance_height; i++) {
        for (int j = 0; j < blocks.chrominance_width; j++) {
            // Perform DCT on the chrominance blocks
            // Level shift the block
            double **block_cb = level_shift(blocks.cb_blocks[i][j]);
            double **block_cr = level_shift(blocks.cr_blocks[i][j]);
            // Perform DCT on the shifted block
            double **dct_block_cb = dct_2d(block_cb, cosine_matrix);
            double **dct_block_cr = dct_2d(block_cr, cosine_matrix);
            // Quantize the DCT coefficients
            double **quantized_block_cb = quantize_block(dct_block_cb, 1.0, CHROMINANCE);
            double **quantized_block_cr = quantize_block(dct_block_cr, 1.0, CHROMINANCE);
            // Store the quantized block
            quantized_blocks.cb_blocks[i][j] = quantized_block_cb;
            quantized_blocks.cr_blocks[i][j] = quantized_block_cr;
            // Free the DCT block
            free_double_matrix(dct_block_cb, DCT_BLOCK_SIZE);
            free_double_matrix(dct_block_cr, DCT_BLOCK_SIZE);
            // Free the shifted block
            free_double_matrix(block_cb, DCT_BLOCK_SIZE);
            free_double_matrix(block_cr, DCT_BLOCK_SIZE);
        }
    }
    
    // Free blocks
    free_dct_blocks(&blocks);
    blocks = init_dct_blocks(quantized_blocks.luminance_height, quantized_blocks.luminance_width, quantized_blocks.chrominance_height, quantized_blocks.chrominance_width);
    
    // Convert quantized_blocks to zigzag arrays
    ZigzagMatrix zigzag_matrix = blocks_to_arrays(quantized_blocks);

    print_int_array(zigzag_matrix.y_zigzag[0][0], DCT_BLOCK_SIZE * DCT_BLOCK_SIZE);
    // Free the quantized blocks
    free_dct_blocks(&quantized_blocks);

    //entropy coding
    BitWriter bit_writer;
    
    bitwriter_init(&bit_writer, "saida.bin");

    // Encode the DC coefficients
    int previous_dc = 0; // Initialize previous DC value
    for (int i = 0; i < zigzag_matrix.luminance_height; i++) {
        for (int j = 0; j < zigzag_matrix.luminance_width; j++) {
            // Encode the DC coefficient for the luminance block
            int current_dc = zigzag_matrix.y_zigzag[i][j][0]; // DC coefficient is at (0, 0)

            encode_dc(&bit_writer, current_dc, previous_dc);
            encode_ac(&bit_writer, zigzag_matrix.y_zigzag[i][j]);
            previous_dc = current_dc; // Update previous DC value
        }
    }

    int previous_dc_cb = 0; // Initialize previous DC value for chrominance
    int previous_dc_cr = 0; // Initialize previous DC value for chrominance
    for (int i = 0; i < zigzag_matrix.chrominance_height; i++) {
        for (int j = 0; j < zigzag_matrix.chrominance_width; j++) {
            // Encode the DC coefficient for the chrominance blocks
            int current_dc_cb = zigzag_matrix.cb_zigzag[i][j][0]; // DC coefficient is at (0, 0)
            int current_dc_cr = zigzag_matrix.cr_zigzag[i][j][0]; // DC coefficient is at (0, 0)

            encode_dc(&bit_writer, current_dc_cb, previous_dc_cb);

            encode_ac(&bit_writer, zigzag_matrix.cb_zigzag[i][j]);

            encode_dc(&bit_writer, current_dc_cr, previous_dc_cr);

            encode_ac(&bit_writer, zigzag_matrix.cr_zigzag[i][j]);

            previous_dc_cb = current_dc_cb; // Update previous DC value for chrominance
            previous_dc_cr = current_dc_cr; // Update previous DC value for chrominance
        }
    }

    // Flush the bit writer to write all bits to the file
    bitwriter_flush(&bit_writer);
    // Free the Huffman tree
    //free_huffman_tree(huffman_tree);
    printf("Entropy coding completed.\n");

    // entropy decoding
    Huffman_node *huffman_tree = create_huffman_tree();

    // // Initialize a bit reader for entropy decoding
    BitReader bit_reader;

    bitreader_init(&bit_reader, "saida.bin");
    // // Decode luminance blocks
    previous_dc = 0; // Reset previous DC value

    int *block;

    for (int i = 0; i < zigzag_matrix.luminance_height; i++) {
        for (int j = 0; j < zigzag_matrix.luminance_width; j++) {
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
            free(zigzag_matrix.y_zigzag[i][j]);
            zigzag_matrix.y_zigzag[i][j] = block;
        }
    }

    printf("---------------------------------\n");

    // Decode chrominance blocks
    previous_dc_cb = 0; // Reset previous DC value for chrominance
    previous_dc_cr = 0; // Reset previous DC value for chrominance

    for (int i = 0; i < zigzag_matrix.chrominance_height; i++) {
        for (int j = 0; j < zigzag_matrix.chrominance_width; j++) {
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

            free(zigzag_matrix.cb_zigzag[i][j]);

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
            free(zigzag_matrix.cr_zigzag[i][j]);
            zigzag_matrix.cr_zigzag[i][j] = block;
        }
    }

    // Convert zigzag arrays back to DCT blocks
    quantized_blocks = arrays_to_blocks(zigzag_matrix);

    // Free the zigzag matrix
    free_zigzag_matrix(&zigzag_matrix);
    
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

    // Merge the blocks back into a YCbCr image
    subsampled_image = merge_blocks_into_ycbcr_420(blocks);

    // Free the blocks
    free_dct_blocks(&blocks);

    // Upsample the YCbCr_420 image to YCbCr
    ycbcr_upsampling_420(&ycbcr_image, subsampled_image);
    // Free the YCbCr_420 image
    free_ycbcr_image_420(&subsampled_image);
    // Convert YCbCr to RGB
    rgb_image = init_rgb_image();
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