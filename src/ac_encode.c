#include <stdio.h>
#include <stdlib.h>
#include "ac_encode.h"
#include "huffman.h"

void encode_ac(BitWriter* bw, int ac[64]) {
    int zero_run = 0;
    for (int i = 1; i < 64; i++) {
        int val = (int)ac[i];
        if (val == 0) {
            zero_run++;
        } else {
            while (zero_run > 15) {
                bitwriter_write_bits(bw, huffman_ac_prefix[15][0]); // ZRL F/0
                zero_run -= 16;
            }
            int size = get_category(val);
            const char* prefix = huffman_ac_prefix[zero_run][size];
            int mask = (1 << size) - 1;
            int mantissa = val > 0 ? val : (~(-val) & mask);
            bitwriter_write_bits(bw, prefix);
            bitwriter_write_int(bw, mantissa, size);
            zero_run = 0;
        }
    }
    if (zero_run > 0) {
        bitwriter_write_bits(bw, huffman_ac_prefix[0][0]); // EOB
    }
    
}
