// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
// #include "dct.h"
// #include "quantization.h"
// #include "print.h"
// #include "heap_manager.h"

// int main() {
//     double cosine_matrix[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE];
//     compute_cosine_matrix(cosine_matrix);
//     // Print the cosine matrix for verification
//     // First, allocate the memory as you did
//     unsigned char **test = init_uchar_matrix(DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);

//     // Now initialize the matrix with the given values
//     unsigned char values[8][8] = {
//         {52, 55, 61, 66, 70, 61, 64, 73},
//         {63, 59, 66, 90, 109, 85, 69, 72},
//         {62, 59, 68, 113, 144, 104, 66, 73},
//         {63, 58, 71, 122, 154, 106, 70, 69},
//         {67, 61, 68, 104, 126, 88, 68, 70},
//         {79, 65, 60, 70, 77, 68, 58, 75},
//         {85, 71, 64, 59, 55, 61, 65, 83},
//         {87, 79, 69, 68, 65, 76, 78, 94}
//     };

//     // Copy values to test matrix
//     for (int i = 0; i < DCT_BLOCK_SIZE; i++) {
//         for (int j = 0; j < DCT_BLOCK_SIZE; j++) {
//             test[i][j] = values[i][j];
//         }
//     }

//     double **shifted_block = level_shift(test);
//     printf("Shifted Block:\n");
//     print_double_matrix(shifted_block, DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
//     double **dct_result = dct_2d(shifted_block, cosine_matrix);
//     printf("DCT Result:\n");
//     print_double_matrix(dct_result, DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
//     double **quantized_result = quantize_block(dct_result, 1.0, LUMINANCE);
//     printf("Quantized Result:\n");
//     print_double_matrix(quantized_result, DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
//     double **dequantized_result = dequantize_block(quantized_result, 1.0, LUMINANCE);
//     printf("Dequantized Result:\n");
//     print_double_matrix(dequantized_result, DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
//     double **idct_result = idct_2d(dequantized_result, cosine_matrix);
//     printf("IDCT Result:\n");
//     print_double_matrix(idct_result, DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
//     unsigned char **unshifted_result = unlevel_shift(idct_result);
//     printf("Unshifted Result:\n");
//     print_uchar_matrix(unshifted_result, DCT_BLOCK_SIZE, DCT_BLOCK_SIZE);
//     // Free allocated memory
//     free_double_matrix(shifted_block, DCT_BLOCK_SIZE);
//     free_double_matrix(dct_result, DCT_BLOCK_SIZE);
//     free_double_matrix(quantized_result, DCT_BLOCK_SIZE);
//     free_double_matrix(dequantized_result, DCT_BLOCK_SIZE);
//     free_double_matrix(idct_result, DCT_BLOCK_SIZE);
//     free_uchar_matrix(unshifted_result, DCT_BLOCK_SIZE);
//     free_uchar_matrix(test, DCT_BLOCK_SIZE);

//     return 0;
// }