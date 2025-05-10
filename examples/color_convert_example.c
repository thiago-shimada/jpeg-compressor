#include <stdlib.h>
#include <stdio.h>
#include "color_convert.h"

int main(int argc, char *argv[]) {
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

    ycbcr_upsampling_420(&ycbcr_image, subsampled_image);
    ycbcr_to_rgb(&rgb_image, ycbcr_image);

    // Free the ycbcr images
    free_ycbcr_image_420(&subsampled_image);
    free_ycbcr_image(&ycbcr_image);
    
    // Save the RGB image
    save_rgb_image(argv[2], rgb_image, &file_header, &info_header);
    
    // Free the RGB image
    free_rgb_image(&rgb_image);

    return 0;
}