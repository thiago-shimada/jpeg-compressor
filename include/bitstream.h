#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdio.h>

// Estrutura para escrita de bits
typedef struct {
    FILE* file;
    uint8_t buffer;
    int bits_filled;
} BitWriter;

void bitwriter_init(BitWriter* bw, const char* filename);
void bitwriter_write_bit(BitWriter* bw, int bit);
void bitwriter_write_bits(BitWriter* bw, const char* bits);
void bitwriter_write_int(BitWriter* bw, int value, int size);
void bitwriter_flush(BitWriter* bw);

// Estrutura para leitura de bits
typedef struct {
    FILE* file;
    uint8_t buffer;
    int bits_available;
} BitReader;

void bitreader_init(BitReader* br, const char* filename);
int bitreader_read_bit(BitReader* br);
int bitreader_read_bits(BitReader* br, int size);
void bitreader_close(BitReader* br);

#endif // BITSTREAM_H