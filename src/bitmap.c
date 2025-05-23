#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "platform.h"

/**
 * @brief Loads and processes the BMP file headers
 *
 * This function reads both file header and info header from the BMP file.
 * If the file uses compression, it prints a warning and closes the file.
 * Otherwise, it displays the header information.
 *
 * @param fp File pointer to an opened BMP file
 * @param file_header Pointer to store the bitmap file header data
 * @param info_header Pointer to store the bitmap info header data
 */
void load_bmp_header(FILE *fp, BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header) {
    read_bmp_header(fp, file_header);
    read_bmp_info(fp, info_header);
    if(info_header->Compression != 0) {
        printf("This is a compressed bitmap file.\n");
        fclose(fp);
        return;
    }

    print_bmp_headers(file_header, info_header);
}

/**
 * @brief Reads the bitmap file header from a file
 *
 * This function reads the BITMAPFILEHEADER structure from the provided file pointer.
 * It exits the program if there's an error reading the header.
 *
 * @param fp File pointer positioned at the start of the bitmap file header
 * @param file_header Pointer to store the bitmap file header data
 */
void read_bmp_header(FILE *fp, BITMAPFILEHEADER *file_header) {
    if (fread(&file_header->Type, sizeof(file_header->Type), 1, fp) != 1 ||
        fread(&file_header->Size, sizeof(file_header->Size), 1, fp) != 1 ||
        fread(&file_header->Reserved1, sizeof(file_header->Reserved1), 1, fp) != 1 ||
        fread(&file_header->Reserved2, sizeof(file_header->Reserved2), 1, fp) != 1 ||
        fread(&file_header->OffBits, sizeof(file_header->OffBits), 1, fp) != 1) {
        fprintf(stderr, "Error reading bitmap file header\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Reads the bitmap info header from a file
 *
 * This function reads the BITMAPINFOHEADER structure from the provided file pointer.
 * It exits the program if there's an error reading the info header.
 *
 * @param fp File pointer positioned at the start of the bitmap info header
 * @param info_header Pointer to store the bitmap info header data
 */
void read_bmp_info(FILE *fp, BITMAPINFOHEADER *info_header) {
    if (fread(&info_header->Size, sizeof(info_header->Size), 1, fp) != 1 ||
        fread(&info_header->Width, sizeof(info_header->Width), 1, fp) != 1 ||
        fread(&info_header->Height, sizeof(info_header->Height), 1, fp) != 1 ||
        fread(&info_header->Planes, sizeof(info_header->Planes), 1, fp) != 1 ||
        fread(&info_header->BitCount, sizeof(info_header->BitCount), 1, fp) != 1 ||
        fread(&info_header->Compression, sizeof(info_header->Compression), 1, fp) != 1 ||
        fread(&info_header->SizeImage, sizeof(info_header->SizeImage), 1, fp) != 1 ||
        fread(&info_header->XResolution, sizeof(info_header->XResolution), 1, fp) != 1 ||
        fread(&info_header->YResolution, sizeof(info_header->YResolution), 1, fp) != 1 ||
        fread(&info_header->NColors, sizeof(info_header->NColors), 1, fp) != 1 ||
        fread(&info_header->ImportantColors, sizeof(info_header->ImportantColors), 1, fp) != 1) {
        fprintf(stderr, "Error reading bitmap info header\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Prints the contents of bitmap headers
 *
 * This function displays all fields from both the bitmap file header
 * and the bitmap info header in a formatted way. After displaying the
 * information, it pauses execution using the system's PAUSE command.
 *
 * @param file_header Pointer to the bitmap file header to be displayed
 * @param info_header Pointer to the bitmap info header to be displayed
 */
void print_bmp_headers(BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header) {
    printf("Bitmap File Header:\n");
    printf("  Type: %u\n", file_header->Type);
    printf("  Size: %u\n", file_header->Size);
    printf("  Reserved1: %u\n", file_header->Reserved1);
    printf("  Reserved2: %u\n", file_header->Reserved2);
    printf("  OffBits: %u\n", file_header->OffBits);

    printf("\nBitmap Info Header:\n");
    printf("  Size: %u\n", info_header->Size);
    printf("  Width: %d\n", info_header->Width);
    printf("  Height: %d\n", info_header->Height);
    printf("  Planes: %u\n", info_header->Planes);
    printf("  BitCount: %u\n", info_header->BitCount);
    printf("  Compression: %u\n", info_header->Compression);
    printf("  SizeImage: %u\n", info_header->SizeImage);
    printf("  XResolution: %d\n", info_header->XResolution);
    printf("  YResolution: %d\n", info_header->YResolution);
    printf("  NColors: %u\n", info_header->NColors);
    printf("  ImportantColors: %u\n", info_header->ImportantColors);

    int result = system(PAUSE);
    if (result != 0) {
        fprintf(stderr, "Warning: system pause command failed with code %d\n", result);
    }
}