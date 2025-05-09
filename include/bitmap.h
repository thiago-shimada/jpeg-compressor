#ifndef _BITMAP_H
#define _BITMAP_H

typedef struct {
    unsigned short Type;       // Magic number
    unsigned int Size;        // Size of the file
    unsigned short Reserved1;  // Reserved
    unsigned short Reserved2;  // Reserved
    unsigned int OffBits;     // Offset to bitmap data
} BITMAPFILEHEADER;

# define BF_TYPE 0x4D42 // Bitmap file magic number

typedef struct {
    unsigned int Size;        // Size of this header
    int Width;               // Width of bitmap in pixels
    int Height;              // Height of bitmap in pixels
    unsigned short Planes;    // Number of color planes
    unsigned short BitCount;  // Bits per pixel
    unsigned int Compression; // Compression type
    unsigned int SizeImage;   // Size of image data
    int XResolution;        // Horizontal resolution
    int YResolution;        // Vertical resolution
    unsigned int NColors;     // Number of colors used
    unsigned int ImportantColors; // Important colors
} BITMAPINFOHEADER;

void load_bmp_header(FILE *fp, BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header);
void read_bmp_header(FILE *fp, BITMAPFILEHEADER *file_header);
void read_bmp_info(FILE *fp, BITMAPINFOHEADER *info_header);
void print_bmp_headers(BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header);

#endif // _BITMAP_H