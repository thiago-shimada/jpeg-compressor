#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "color_convert.h"

/**
 * @brief Initializes a 2D array for storing RGB pixel data
 *
 * This function allocates memory for a 2D array of RGB_Pixel structures
 * with the specified width and height.
 *
 * @param width Width of the image in pixels
 * @param height Height of the image in pixels
 * @return Pointer to the allocated 2D array of RGB_Pixel structures
 */
RGB_Pixel **init_rgb_image(int width, int height) {
    RGB_Pixel **image = (RGB_Pixel **)malloc(height * sizeof(RGB_Pixel *));
    for (int i = 0; i < height; i++) {
        image[i] = (RGB_Pixel *)malloc(width * sizeof(RGB_Pixel));
    }
    return image;
}

/**
 * @brief Frees the memory allocated for an RGB image
 *
 * This function frees the memory previously allocated by init_rgb_image.
 *
 * @param image Pointer to the 2D array of RGB_Pixel structures to be freed
 * @param height Height of the image in pixels
 */
void free_rgb_image(RGB_Pixel **image, int height) {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

/**
 * @brief Reads RGB pixel data from a BMP file into a 2D array
 *
 * This function reads RGB pixel data from the given file pointer and stores it
 * in the provided image array. It assumes the file pointer is positioned at the
 * beginning of the file and skips the header.
 *
 * @param fp File pointer to an opened BMP file
 * @param image Pointer to a pre-allocated 2D array for storing the pixel data
 * @param info_header BMP info header containing image dimensions
 */
void read_rgb_image(FILE *fp, RGB_Pixel **image, BITMAPINFOHEADER info_header) {
    int height = info_header.Height;
    int width = info_header.Width;

    fseek(fp, 54, SEEK_SET); // Skip the header

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].b = fgetc(fp);
            image[i][j].g = fgetc(fp);
            image[i][j].r = fgetc(fp);
        }
    }
}

/**
 * @brief Saves an RGB image to a BMP file
 *
 * This function writes the RGB pixel data to a new BMP file, including the appropriate
 * file and info headers. Returns 0 on success, -1 on failure.
 *
 * @param filename Path to the output file
 * @param image Pointer to the 2D array of RGB_Pixel data to be saved
 * @param file_header Pointer to the original BMP file header to be copied
 * @param info_header Pointer to the original BMP info header to be copied
 * @return 0 on success, -1 on failure
 */
int save_rgb_image(const char *filename, RGB_Pixel **image, BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header){
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file for writing: %s\n", filename);
        return -1;
    }

    fwrite(&(file_header->Type), sizeof(unsigned short), 1, fp);
    fwrite(&(file_header->Size), sizeof(unsigned int), 1, fp);
    fwrite(&(file_header->Reserved1), sizeof(unsigned short), 1, fp);
    fwrite(&(file_header->Reserved2), sizeof(unsigned short), 1, fp);
    fwrite(&(file_header->OffBits), sizeof(unsigned int), 1, fp);

    fwrite(&(info_header->Size), sizeof(unsigned int), 1, fp);
    fwrite(&(info_header->Width), sizeof(int), 1, fp);
    fwrite(&(info_header->Height), sizeof(int), 1, fp);
    fwrite(&(info_header->Planes), sizeof(unsigned short int), 1, fp);
    fwrite(&(info_header->BitCount), sizeof(unsigned short int), 1, fp);
    fwrite(&(info_header->Compression), sizeof(unsigned int), 1, fp);
    fwrite(&(info_header->SizeImage), sizeof(unsigned int), 1, fp);
    fwrite(&(info_header->XResolution), sizeof(int), 1, fp);
    fwrite(&(info_header->YResolution), sizeof(int), 1, fp);
    fwrite(&(info_header->NColors), sizeof(unsigned int), 1, fp);
    fwrite(&(info_header->ImportantColors), sizeof(unsigned int), 1, fp);

    int height = info_header->Height;
    int width = info_header->Width;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fputc(image[i][j].b, fp);
            fputc(image[i][j].g, fp);
            fputc(image[i][j].r, fp);
        }
    }

    fclose(fp);

    return 0;
}

/**
 * @brief Initializes a 2D array for storing YCbCr pixel data
 *
 * This function allocates memory for a 2D array of YCbCr_Pixel structures
 * with the specified width and height.
 *
 * @param width Width of the image in pixels
 * @param height Height of the image in pixels
 * @return Pointer to the allocated 2D array of YCbCr_Pixel structures
 */
YCbCr_Pixel **init_ycbcr_image(int width, int height){
    YCbCr_Pixel **image = (YCbCr_Pixel **)malloc(height * sizeof(YCbCr_Pixel *));
    for (int i = 0; i < height; i++) {
        image[i] = (YCbCr_Pixel *)malloc(width * sizeof(YCbCr_Pixel));
    }

    return image;
}

/**
 * @brief Frees the memory allocated for a YCbCr image
 *
 * This function frees the memory previously allocated by init_ycbcr_image.
 *
 * @param image Pointer to the 2D array of YCbCr_Pixel structures to be freed
 * @param height Height of the image in pixels
 */
void free_ycbcr_image(YCbCr_Pixel **image, int height) {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

/**
 * @brief Converts an RGB image to YCbCr color space
 *
 * This function converts RGB pixel values to YCbCr using standard
 * conversion formulas. The Y component represents luminance, while Cb and Cr
 * represent blue-difference and red-difference chroma components.
 *
 * @param rgb_image Source RGB image data
 * @param ycbcr_image Pre-allocated destination for YCbCr image data
 * @param width Width of the image in pixels
 * @param height Height of the image in pixels
 */
void rgb_to_ycbcr(RGB_Pixel **rgb_image, YCbCr_Pixel **ycbcr_image, int width, int height) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            double r = (double)rgb_image[i][j].r;
            double g = (double)rgb_image[i][j].g;
            double b = (double)rgb_image[i][j].b;

            double y = 0.299*r + 0.587*g + 0.114*b;
            double cb = 128 - 0.168736*r - 0.331264*g + 0.5*b;
            double cr = 128 + 0.5*r - 0.418688*g - 0.081312*b;

            ycbcr_image[i][j].y = (unsigned char)(y < 0 ? 0 : (y > 255 ? 255 : y));
            ycbcr_image[i][j].cb = (unsigned char)(cb < 0 ? 0 : (cb > 255 ? 255 : cb));
            ycbcr_image[i][j].cr = (unsigned char)(cr < 0 ? 0 : (cr > 255 ? 255 : cr));
        }
    }
}

/**
 * @brief Converts a YCbCr image back to RGB color space
 *
 * This function converts YCbCr pixel values to RGB using standard
 * conversion formulas, reversing the process performed by rgb_to_ycbcr.
 *
 * @param ycbcr_image Source YCbCr image data
 * @param rgb_image Pre-allocated destination for RGB image data
 * @param width Width of the image in pixels
 * @param height Height of the image in pixels
 */
void ycbcr_to_rgb(YCbCr_Pixel **ycbcr_image, RGB_Pixel **rgb_image, int width, int height) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            double y = (double)ycbcr_image[i][j].y;
            double cb = (double)ycbcr_image[i][j].cb - 128;
            double cr = (double)ycbcr_image[i][j].cr - 128;

            double r = y + 1.402*cr;
            double g = y - 0.344136*cb - 0.714136*cr;
            double b = y + 1.772*cb;

            rgb_image[i][j].r = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
            rgb_image[i][j].g = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
            rgb_image[i][j].b = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));
        }
    }
}

/**
 * @brief Performs 4:2:0 chroma subsampling on a YCbCr image
 *
 * This function creates a new YCbCr image where the chroma components (Cb and Cr)
 * are subsampled by averaging each 2x2 block of pixels. The luminance component (Y)
 * is preserved for all pixels. This is a common compression technique in image and 
 * video processing.
 *
 * @param ycbcr_image Source YCbCr image data
 * @param width Width of the image in pixels
 * @param height Height of the image in pixels
 * @return Pointer to a new YCbCr image with subsampled chroma components
 */
YCbCr_Pixel **ycbcr_subsampling_420(YCbCr_Pixel **ycbcr_image, int width, int height) {
    YCbCr_Pixel **subsampled_image = init_ycbcr_image(width, height);

    for (int i = 0; i < height; i+=2) {
        for (int j = 0; j < width; j+=2) {
            // Average in the chromatic channels
            unsigned char cb = (ycbcr_image[i][j].cb + ycbcr_image[i][j+1].cb + ycbcr_image[i+1][j].cb + ycbcr_image[i+1][j+1].cb) >> 2;
            unsigned char cr = (ycbcr_image[i][j].cr + ycbcr_image[i][j+1].cr + ycbcr_image[i+1][j].cr + ycbcr_image[i+1][j+1].cr) >> 2;

            // Assign all Y values to the subsampled image
            subsampled_image[i][j].y = ycbcr_image[i][j].y;
            subsampled_image[i+1][j].y = ycbcr_image[i+1][j].y;
            subsampled_image[i][j+1].y = ycbcr_image[i][j+1].y;
            subsampled_image[i+1][j+1].y = ycbcr_image[i+1][j+1].y;  
            
            // Assign the averaged chromatic values
            subsampled_image[i][j].cb = subsampled_image[i+1][j].cb = subsampled_image[i][j+1].cb = subsampled_image[i+1][j+1].cb = cb;
            subsampled_image[i][j].cr = subsampled_image[i+1][j].cr = subsampled_image[i][j+1].cr = subsampled_image[i+1][j+1].cr = cr;
        }
    }

    return subsampled_image;
}