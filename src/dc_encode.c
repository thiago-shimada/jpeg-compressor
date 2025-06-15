#include <stdio.h>
#include <stdlib.h>
#include "dc_encode.h"
#include "huffman.h"


void encode_dc(BitWriter* bw, int current_dc, int previous_dc) {
    int diff = current_dc - previous_dc;
    int category = get_category(diff);
    bitwriter_write_bits(bw, huffman_dc_prefix[category]);

    if (category > 0) {
        int mask = (1 << category) - 1;
        int mantissa = diff > 0 ? diff : (~(-diff) & mask);
        bitwriter_write_int(bw, mantissa, category);
    }
}
