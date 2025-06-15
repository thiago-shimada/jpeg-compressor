#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "ac_encode.h"
#include "dc_encode.h"
#include "huffman.h"
#include "bitstream.h"

int main() {

    const char *filename = "saida.bin";

    BitReader reader;
    bitreader_init(&reader, filename);

    Huffman_node *huffman_tree = create_huffman_tree();

    int dc_category  = read_dc_category(&reader);
    
    printf("DC Category: %d\n", dc_category);

    int mantissa = bitreader_read_bits(&reader, dc_category);

    int dc = decode_value(mantissa, dc_category);
    printf("DC Value: %d\n", dc);

    int block[64] = {0};

    block[0] = dc;

    int pos = 1;

    while (pos < 64) {
        Huffman_node *node = read_ac_category(huffman_tree, &reader);

        if (node == NULL) {
            break; // Error in reading AC category
        }
        int run = node->run;
        int category = node->category;

        printf("Run: %d, Category: %d\n", run, category);

        if (run == 0 && category == 0) {
            break; // EOB
        }

        int ac_mantissa = bitreader_read_bits(&reader, category);
        int ac_value = decode_value(ac_mantissa, category);

        for (int i = 0; i < run; i++) {
            block[pos++] = 0;
        }
        block[pos++] = ac_value;
    }

    printf("Decoded values:\n");
    for (int i = 0; i < 64; i++) {
        printf("%d ", block[i]);
    }
    printf("\n");
    bitreader_close(&reader);


    
    return 0;
}