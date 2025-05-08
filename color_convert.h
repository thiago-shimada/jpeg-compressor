#ifndef _COLOR_CONVERT_H
#define _COLOR_CONVERT_H
#include "bitmap.h"

typedef struct {
    unsigned char r, g, b;
} RGB_Pixel;

typedef struct {
    unsigned char y, cb, cr;
} YCbCr_Pixel;

typedef struct {
    unsigned char y[4];
    unsigned char cb, cr;
} YCbCr_MCU_420;

RGB_Pixel **init_rgb_image(int width, int height);
void free_rgb_image(RGB_Pixel **image, int height);
void read_rgb_image(FILE *fp, RGB_Pixel **image, BITMAPINFOHEADER info_header);
YCbCr_Pixel **init_ycbcr_image(int width, int height);
void free_ycbcr_image(YCbCr_Pixel **image, int height);
void rgb_to_ycbcr(RGB_Pixel **rgb_image, YCbCr_Pixel **ycbcr_image, int width, int height);
void ycbcr_to_rgb(YCbCr_Pixel **ycbcr_image, RGB_Pixel **rgb_image, int width, int height);
int save_rgb_image(const char *filename, RGB_Pixel **image, BITMAPFILEHEADER *original_file_header, BITMAPINFOHEADER *original_info_header);

#endif