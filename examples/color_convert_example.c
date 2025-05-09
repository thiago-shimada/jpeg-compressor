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

    RGB_Pixel **image = init_rgb_image(info_header.Width, info_header.Height);
    read_rgb_image(fp, image, info_header);
    fclose(fp);

    // Process the image here (e.g., convert to grayscale)
    // ...
    YCbCr_Pixel **image2 = init_ycbcr_image(info_header.Width, info_header.Height);
    rgb_to_ycbcr(image, image2, info_header.Width, info_header.Height);
    
    free_rgb_image(image, info_header.Height);

    YCbCr_Pixel **subsampled_image = ycbcr_subsampling_420(image2, info_header.Width, info_header.Height);

    image = init_rgb_image(info_header.Width, info_header.Height);
    ycbcr_to_rgb(subsampled_image, image, info_header.Width, info_header.Height);

    save_rgb_image(argv[2], image, &file_header, &info_header);

    free_rgb_image(image, info_header.Height);
    free_ycbcr_image(image2, info_header.Height);
    free_ycbcr_image(subsampled_image, info_header.Height);

    return 0;
}