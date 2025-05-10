#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "color_convert.h"
#include "heap_manager.h"

/**
 * @brief Initializes an RGB_Image structure
 *
 * This function initializes an RGB_Image structure with height and width set to 0
 * and pointers to the color channels set to NULL.
 *
 * @return An initialized RGB_Image structure
 */
RGB_Image init_rgb_image() {
    RGB_Image rgb_image;
    rgb_image.height = 0;
    rgb_image.width = 0;
    rgb_image.r = NULL;
    rgb_image.g = NULL;
    rgb_image.b = NULL;
    return rgb_image;
}

/**
 * @brief Initializes a YCbCr_Image structure
 *
 * This function initializes a YCbCr_Image structure with height and width set to 0
 * and pointers to the color channels set to NULL.
 *
 * @return An initialized YCbCr_Image structure
 */
YCbCr_Image init_ycbcr_image() {
    YCbCr_Image ycbcr_image;
    ycbcr_image.height = 0;
    ycbcr_image.width = 0;
    ycbcr_image.y = NULL;
    ycbcr_image.cb = NULL;
    ycbcr_image.cr = NULL;
    return ycbcr_image;
}

/**
 * @brief Initializes a YCbCr_Image_420 structure
 *
 * This function initializes a YCbCr_Image_420 structure with luminance and chrominance
 * dimensions set to 0 and pointers to the color channels set to NULL.
 *
 * @return An initialized YCbCr_Image_420 structure
 */
YCbCr_Image_420 init_ycbcr_image_420() {
    YCbCr_Image_420 ycbcr_image_420;
    ycbcr_image_420.luminance_height = 0;
    ycbcr_image_420.luminance_width = 0;
    ycbcr_image_420.chrominance_height = 0;
    ycbcr_image_420.chrominance_width = 0;
    ycbcr_image_420.y = NULL;
    ycbcr_image_420.cb = NULL;
    ycbcr_image_420.cr = NULL;
    return ycbcr_image_420;
}

/**
 * @brief Reads RGB pixel data from a BMP file into an RGB_Image structure
 *
 * This function reads RGB pixel data from the given file pointer and stores it
 * in an RGB_Image structure. It assumes the file pointer is positioned at the
 * beginning of the file and skips the header. If the provided RGB_Image structure
 * already contains allocated memory (height and width not 0), it frees that memory first.
 *
 * @param rgb_image Pointer to an RGB_Image structure to store the data
 * @param fp File pointer to an opened BMP file
 * @param info_header BMP info header containing image dimensions
 */
void read_rgb_image(RGB_Image *rgb_image, FILE *fp, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header) {
    int height = info_header.Height;
    int width = info_header.Width;
    
    // Free any previously allocated memory if height and width are not 0
    if (rgb_image->height != 0 && rgb_image->width != 0) {
        free_rgb_image(rgb_image);
    }
    
    rgb_image->height = height;
    rgb_image->width = width;
    
    // Allocate memory for each color channel using heap_manager functions
    rgb_image->r = init_uchar_matrix(height, width);
    rgb_image->g = init_uchar_matrix(height, width);
    rgb_image->b = init_uchar_matrix(height, width);
    
    fseek(fp, file_header.OffBits, SEEK_SET); // Skip the header
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            rgb_image->b[i][j] = fgetc(fp);
            rgb_image->g[i][j] = fgetc(fp);
            rgb_image->r[i][j] = fgetc(fp);
        }
    }
}


/**
 * @brief Converts an RGB_Image to YCbCr color space
 *
 * This function converts RGB pixel values to YCbCr using standard
 * conversion formulas. The Y component represents luminance, while Cb and Cr
 * represent blue-difference and red-difference chroma components. If the provided 
 * YCbCr_Image already contains allocated memory (height and width not 0), it frees 
 * that memory first.
 *
 * @param ycbcr_image Pointer to YCbCr_Image structure to store the result
 * @param rgb_image Source RGB_Image data
 */
void rgb_to_ycbcr(YCbCr_Image *ycbcr_image, RGB_Image rgb_image) {
    int height = rgb_image.height;
    int width = rgb_image.width;
    
    // Free any previously allocated memory if height and width are not 0
    if (ycbcr_image->height != 0 && ycbcr_image->width != 0) {
        free_ycbcr_image(ycbcr_image);
    }
    
    ycbcr_image->height = height;
    ycbcr_image->width = width;
    
    // Allocate memory for each color channel using heap_manager functions
    ycbcr_image->y = init_uchar_matrix(height, width);
    ycbcr_image->cb = init_uchar_matrix(height, width);
    ycbcr_image->cr = init_uchar_matrix(height, width);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double r = (double)rgb_image.r[i][j];
            double g = (double)rgb_image.g[i][j];
            double b = (double)rgb_image.b[i][j];

            double y = 0.299*r + 0.587*g + 0.114*b;
            double cb = 128 - 0.168736*r - 0.331264*g + 0.5*b;
            double cr = 128 + 0.5*r - 0.418688*g - 0.081312*b;

            ycbcr_image->y[i][j] = (unsigned char)(y < 0 ? 0 : (y > 255 ? 255 : y));
            ycbcr_image->cb[i][j] = (unsigned char)(cb < 0 ? 0 : (cb > 255 ? 255 : cb));
            ycbcr_image->cr[i][j] = (unsigned char)(cr < 0 ? 0 : (cr > 255 ? 255 : cr));
        }
    }
}

/**
 * @brief Converts a YCbCr_Image back to RGB color space
 *
 * This function converts YCbCr pixel values to RGB using standard
 * conversion formulas, reversing the process performed by rgb_to_ycbcr.
 * If the provided RGB_Image already contains allocated memory (height and width not 0),
 * it frees that memory first.
 *
 * @param rgb_image Pointer to RGB_Image structure to store the result
 * @param ycbcr_image Source YCbCr_Image data
 */
void ycbcr_to_rgb(RGB_Image *rgb_image, YCbCr_Image ycbcr_image) {
    int height = ycbcr_image.height;
    int width = ycbcr_image.width;
    
    // Free any previously allocated memory if height and width are not 0
    if (rgb_image->height != 0 && rgb_image->width != 0) {
        free_rgb_image(rgb_image);
    }
    
    rgb_image->height = height;
    rgb_image->width = width;
    
    // Allocate memory for each color channel using heap_manager functions
    rgb_image->r = init_uchar_matrix(height, width);
    rgb_image->g = init_uchar_matrix(height, width);
    rgb_image->b = init_uchar_matrix(height, width);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double y = (double)ycbcr_image.y[i][j];
            double cb = (double)ycbcr_image.cb[i][j] - 128;
            double cr = (double)ycbcr_image.cr[i][j] - 128;

            double r = y + 1.402*cr;
            double g = y - 0.344136*cb - 0.714136*cr;
            double b = y + 1.772*cb;

            rgb_image->r[i][j] = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
            rgb_image->g[i][j] = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
            rgb_image->b[i][j] = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));
        }
    }
}

/**
 * @brief Saves an RGB_Image to a BMP file
 *
 * This function writes the RGB pixel data to a new BMP file, including the appropriate
 * file and info headers. Returns 0 on success, -1 on failure.
 *
 * @param filename Path to the output file
 * @param rgb_image RGB_Image structure containing data to be saved
 * @param original_file_header Pointer to the original BMP file header to be copied
 * @param original_info_header Pointer to the original BMP info header to be copied
 * @return 0 on success, -1 on failure
 */
int save_rgb_image(const char *filename, RGB_Image rgb_image, BITMAPFILEHEADER *original_file_header, BITMAPINFOHEADER *original_info_header) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file for writing: %s\n", filename);
        return -1;
    }

    // Write file header
    fwrite(&(original_file_header->Type), sizeof(unsigned short), 1, fp);
    fwrite(&(original_file_header->Size), sizeof(unsigned int), 1, fp);
    fwrite(&(original_file_header->Reserved1), sizeof(unsigned short), 1, fp);
    fwrite(&(original_file_header->Reserved2), sizeof(unsigned short), 1, fp);
    fwrite(&(original_file_header->OffBits), sizeof(unsigned int), 1, fp);

    // Write info header
    fwrite(&(original_info_header->Size), sizeof(unsigned int), 1, fp);
    fwrite(&(original_info_header->Width), sizeof(int), 1, fp);
    fwrite(&(original_info_header->Height), sizeof(int), 1, fp);
    fwrite(&(original_info_header->Planes), sizeof(unsigned short int), 1, fp);
    fwrite(&(original_info_header->BitCount), sizeof(unsigned short int), 1, fp);
    fwrite(&(original_info_header->Compression), sizeof(unsigned int), 1, fp);
    fwrite(&(original_info_header->SizeImage), sizeof(unsigned int), 1, fp);
    fwrite(&(original_info_header->XResolution), sizeof(int), 1, fp);
    fwrite(&(original_info_header->YResolution), sizeof(int), 1, fp);
    fwrite(&(original_info_header->NColors), sizeof(unsigned int), 1, fp);
    fwrite(&(original_info_header->ImportantColors), sizeof(unsigned int), 1, fp);

    // Standard headers total 54 bytes. If original offset is larger, add padding
    int current_pos = ftell(fp);
    int padding_needed = original_file_header->OffBits - current_pos;
    
    // Add any additional header data/padding to match original offset
    if (padding_needed > 0) {
        unsigned char padding = 0;
        for (int i = 0; i < padding_needed; i++) {
            fwrite(&padding, 1, 1, fp);
        }
    }

    int height = rgb_image.height;
    int width = rgb_image.width;

    // Write pixel data
    for (int i = 0; i < (int) height; i++) {
        for (int j = 0; j < (int) width; j++) {
            fputc(rgb_image.b[i][j], fp);
            fputc(rgb_image.g[i][j], fp);
            fputc(rgb_image.r[i][j], fp);
        }
    }

    fclose(fp);
    return 0;
}

/**
 * @brief Performs 4:2:0 chroma subsampling on a YCbCr image
 *
 * This function creates a YCbCr_Image_420 where the chroma components (Cb and Cr)
 * are subsampled by averaging each 2x2 block of pixels. The luminance component (Y)
 * is preserved at full resolution. If the provided YCbCr_Image_420 already contains
 * allocated memory, it frees that memory first.
 *
 * @param ycbcr_image_420 Pointer to YCbCr_Image_420 structure to store the result
 * @param ycbcr_image Source YCbCr_Image data
 */
void ycbcr_subsampling_420(YCbCr_Image_420 *ycbcr_image_420, YCbCr_Image ycbcr_image) {
    int luminance_height = ycbcr_image.height;
    int luminance_width = ycbcr_image.width;

    int chrominance_height = (luminance_height / 2) + (luminance_height / 2) % 8;
    int chrominance_width = (luminance_width / 2) + (luminance_width / 2) % 8;
    
    // Free any previously allocated memory if dimensions are not 0
    if (ycbcr_image_420->luminance_height != 0 && ycbcr_image_420->luminance_width != 0) {
        free_ycbcr_image_420(ycbcr_image_420);
    }
    
    ycbcr_image_420->luminance_height = luminance_height;
    ycbcr_image_420->luminance_width = luminance_width;
    ycbcr_image_420->chrominance_height = chrominance_height;
    ycbcr_image_420->chrominance_width = chrominance_width;
    
    // Allocate memory for luminance at full resolution
    ycbcr_image_420->y = init_uchar_matrix(luminance_height, luminance_width);
    
    // Allocate memory for chrominance at reduced resolution
    ycbcr_image_420->cb = init_uchar_matrix(chrominance_height, chrominance_width);
    ycbcr_image_420->cr = init_uchar_matrix(chrominance_height, chrominance_width);
    
    // Copy luminance (Y) values at full resolution
    for (int i = 0; i < (int) luminance_height; i++) {
        for (int j = 0; j < (int) luminance_width; j++) {
            ycbcr_image_420->y[i][j] = ycbcr_image.y[i][j];
        }
    }
    
    // Subsample chrominance (Cb, Cr) values
    for (int i = 0; i < luminance_height; i += 2) {
        for (int j = 0; j < luminance_width; j += 2) {
            // Average the chrominance values in 2x2 blocks
            unsigned char cb_avg = (ycbcr_image.cb[i][j] + ycbcr_image.cb[i][j+1] + 
                                   ycbcr_image.cb[i+1][j] + ycbcr_image.cb[i+1][j+1]) >> 2;
            unsigned char cr_avg = (ycbcr_image.cr[i][j] + ycbcr_image.cr[i][j+1] + 
                                   ycbcr_image.cr[i+1][j] + ycbcr_image.cr[i+1][j+1]) >> 2;
            
            // Store averaged values in the reduced resolution arrays
            ycbcr_image_420->cb[i/2][j/2] = cb_avg;
            ycbcr_image_420->cr[i/2][j/2] = cr_avg;
        }
    }

    for (int i = luminance_height/2; i < chrominance_height; i++) {
        for (int j = 0; j < luminance_width/2; j++) {
            ycbcr_image_420->cb[i][j] = ycbcr_image.cb[luminance_height-1][j];
            ycbcr_image_420->cr[i][j] = ycbcr_image.cr[luminance_height-1][j];
        }
    }

    for (int i = 0; i < luminance_height; i++) {
        for (int j = luminance_width/2; j < chrominance_width; j++) {
            ycbcr_image_420->cb[i][j] = ycbcr_image.cb[i][luminance_width-1];
            ycbcr_image_420->cr[i][j] = ycbcr_image.cr[i][luminance_width-1];
        }
    }

    for (int i = luminance_height/2; i < chrominance_height; i++) {
        for (int j = luminance_width/2; j < chrominance_width; j++) {
            ycbcr_image_420->cb[i][j] = ycbcr_image.cb[luminance_height-1][luminance_width-1];
            ycbcr_image_420->cr[i][j] = ycbcr_image.cr[luminance_height-1][luminance_width-1];
        }
    }
}

/**
 * @brief Performs an upsampling of a YCbCr image from 4:2:0 to full resolution
 *
 * This function takes a YCbCr_Image_420 structure and converts it back to a full
 * resolution YCbCr_Image structure. The chrominance components (Cb and Cr) are
 * upsampled by duplicating the values of the neighboring pixels. If the provided
 * YCbCr_Image already contains allocated memory (height and width not 0), it frees
 * that memory first.
 *
 * @param ycbcr_image Pointer to YCbCr_Image structure to store the result
 * @param ycbcr_image_420 Source YCbCr_Image_420 data
 */
void ycbcr_upsampling_420(YCbCr_Image *ycbcr_image, YCbCr_Image_420 ycbcr_image_420) {
    int height = ycbcr_image_420.luminance_height;
    int width = ycbcr_image_420.luminance_width;

    if (ycbcr_image->height != 0 && ycbcr_image->width != 0) {
        free_ycbcr_image(ycbcr_image);
    }

    ycbcr_image->height = height;
    ycbcr_image->width = width;

    // Allocate memory for each color channel using heap_manager functions
    ycbcr_image->y = init_uchar_matrix(height, width);
    ycbcr_image->cb = init_uchar_matrix(height, width);
    ycbcr_image->cr = init_uchar_matrix(height, width);

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            ycbcr_image->y[i][j] = ycbcr_image_420.y[i][j];
        }
    }

    // Upsample chrominance channels (Cb and Cr)
    // In 4:2:0 format, chrominance is sampled at half the resolution in both dimensions
    for (int i = 0; i < height; i += 2) {
        for (int j = 0; j < width; j += 2) {
            // Calculate source position in the downsampled chrominance planes
            int cb_i = i / 2;
            int cb_j = j / 2;
            
            // Each chrominance value gets copied to a 2x2 block in the full resolution image
            unsigned char cb_val = ycbcr_image_420.cb[cb_i][cb_j];
            unsigned char cr_val = ycbcr_image_420.cr[cb_i][cb_j];
            
            // Copy the same value to a 2x2 block (nearest neighbor upsampling)
            ycbcr_image->cb[i][j] = cb_val;
            ycbcr_image->cb[i][j+1] = cb_val;
            ycbcr_image->cb[i+1][j] = cb_val;
            ycbcr_image->cb[i+1][j+1] = cb_val;
            
            ycbcr_image->cr[i][j] = cr_val;
            ycbcr_image->cr[i][j+1] = cr_val;
            ycbcr_image->cr[i+1][j] = cr_val;
            ycbcr_image->cr[i+1][j+1] = cr_val;
        }
    }
}

/**
 * @brief Frees the memory allocated for an RGB_Image
 *
 * This function frees all memory previously allocated for the RGB_Image structure.
 * Additionally, sets height and width to 0.
 *
 * @param rgb_image Pointer to RGB_Image structure to be freed
 */
 void free_rgb_image(RGB_Image *rgb_image) {
    free_uchar_matrix(rgb_image->r, rgb_image->height);
    free_uchar_matrix(rgb_image->g, rgb_image->height);
    free_uchar_matrix(rgb_image->b, rgb_image->height);
    
    // Set height and width to 0
    rgb_image->height = 0;
    rgb_image->width = 0;
}

/**
 * @brief Frees the memory allocated for a YCbCr_Image
 *
 * This function frees all memory previously allocated for the YCbCr_Image structure.
 * Additionally, sets height and width to 0.
 *
 * @param ycbcr_image YCbCr_Image structure to be freed
 */
void free_ycbcr_image(YCbCr_Image *ycbcr_image) {
    free_uchar_matrix(ycbcr_image->y, ycbcr_image->height);
    free_uchar_matrix(ycbcr_image->cb, ycbcr_image->height);
    free_uchar_matrix(ycbcr_image->cr, ycbcr_image->height);
    ycbcr_image->height = 0;
    ycbcr_image->width = 0;
}

/**
 * @brief Frees the memory allocated for a YCbCr_Image_420
 *
 * This function frees all memory previously allocated for the YCbCr_Image_420 structure.
 * Additionally, sets height and width to 0.
 *
 * @param ycbcr_image_420 YCbCr_Image_420 structure to be freed
 */
void free_ycbcr_image_420(YCbCr_Image_420 *ycbcr_image_420) {
    // Free luminance plane
    free_uchar_matrix(ycbcr_image_420->y, ycbcr_image_420->luminance_height);
    
    // Free chrominance planes
    free_uchar_matrix(ycbcr_image_420->cb, ycbcr_image_420->chrominance_height);
    free_uchar_matrix(ycbcr_image_420->cr, ycbcr_image_420->chrominance_height);

    // Set dimensions to 0
    ycbcr_image_420->luminance_height = 0;
    ycbcr_image_420->luminance_width = 0;
    ycbcr_image_420->chrominance_height = 0;
    ycbcr_image_420->chrominance_width = 0;
}