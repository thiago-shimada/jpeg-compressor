#ifndef _COLOR_CONVERT_H
#define _COLOR_CONVERT_H
#include "bitmap.h"

typedef struct {
    int height, width;
    unsigned char **r;
    unsigned char **g;
    unsigned char **b;
} RGB_Image;

typedef struct {
    int height, width;
    unsigned char **y;
    unsigned char **cb;
    unsigned char **cr;
} YCbCr_Image;

typedef struct {
    int luminance_height, luminance_width;
    int chrominance_height, chrominance_width;

    unsigned char **y;
    unsigned char **cb;
    unsigned char **cr;
} YCbCr_Image_420;

RGB_Image init_rgb_image();
YCbCr_Image init_ycbcr_image();
YCbCr_Image_420 init_ycbcr_image_420();
void read_rgb_image(RGB_Image *rgb_image, FILE *fp, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header);
void free_rgb_image(RGB_Image *rgb_image);
void free_ycbcr_image(YCbCr_Image *ycbcr_image);
void free_ycbcr_image_420(YCbCr_Image_420 *ycbcr_image_420);
void rgb_to_ycbcr(YCbCr_Image *ycbcr_image, RGB_Image rgb_image);
void ycbcr_to_rgb(RGB_Image *rgb_image, YCbCr_Image ycbcr_image);
int save_rgb_image(const char *filename, RGB_Image rgb_image, BITMAPFILEHEADER *original_file_header, BITMAPINFOHEADER *original_info_header);
void ycbcr_subsampling_420(YCbCr_Image_420 *ycbcr_image_420, YCbCr_Image ycbcr_image);
void ycbcr_upsampling_420(YCbCr_Image *ycbcr_image, YCbCr_Image_420 ycbcr_image_420);

#endif