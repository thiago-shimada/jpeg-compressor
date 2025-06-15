#include <stdio.h>
#include <stdlib.h>
#include "bitstream.h"

void bitwriter_init(BitWriter* bw, const char* filename) {
    bw->file = fopen(filename, "wb");
    bw->buffer = 0;
    bw->bits_filled = 0;
}

void bitwriter_write_bit(BitWriter* bw, int bit) {
    bw->buffer = (bw->buffer << 1) | (bit & 1);
    bw->bits_filled++;
    if (bw->bits_filled == 8) {
        fwrite(&bw->buffer, 1, 1, bw->file);
        bw->bits_filled = 0;
        bw->buffer = 0;
    }
}

void bitwriter_write_bits(BitWriter* bw, const char* bits) {
    for (int i = 0; bits[i]; i++) {
        bitwriter_write_bit(bw, bits[i] - '0');
    }
}

void bitwriter_write_int(BitWriter* bw, int value, int size) {
    for (int i = size - 1; i >= 0; i--) {
        bitwriter_write_bit(bw, (value >> i) & 1);
    }
}

void bitwriter_flush(BitWriter* bw) {
    if (bw->bits_filled > 0) {
        bw->buffer <<= (8 - bw->bits_filled);
        fwrite(&bw->buffer, 1, 1, bw->file);
    }
    fclose(bw->file);
}

// Inicializa o BitReader
void bitreader_init(BitReader* br, const char* filename) {
    br->file = fopen(filename, "rb");
    br->buffer = 0;
    br->bits_available = 0;
}

// Lê um único bit
int bitreader_read_bit(BitReader* br) {
    if (br->bits_available == 0) {
        if (fread(&br->buffer, 1, 1, br->file) != 1) {
            return -1; // Retorna -1 em caso de erro ou fim do arquivo
        }
        br->bits_available = 8;
    }

    int bit = (br->buffer >> (br->bits_available - 1)) & 1;
    br->bits_available--;
    return bit;
}

// Lê múltiplos bits
int bitreader_read_bits(BitReader* br, int size) {
    int value = 0;
    for (int i = 0; i < size; i++) {
        int bit = bitreader_read_bit(br);
        if (bit == -1) {
            return -1; // Retorna -1 em caso de erro ou fim do arquivo
        }
        value = (value << 1) | bit;
    }
    return value;
}

// Fecha o BitReader
void bitreader_close(BitReader* br) {
    fclose(br->file);
}