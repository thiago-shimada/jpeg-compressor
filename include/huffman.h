#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdbool.h>
#include "bitstream.h"

#define MAX_RUN 16
#define MAX_CATEGORY 12

extern const char *huffman_ac_prefix[MAX_RUN][MAX_CATEGORY];

extern const char *huffman_dc_prefix[MAX_CATEGORY];

typedef struct Huffman_node{
    int run;
    int category;
    bool is_leaf;
    struct Huffman_node *left;
    struct Huffman_node *right;

} Huffman_node;

int get_category(int value);

Huffman_node *read_ac_category(Huffman_node *huffman_tree, BitReader *br);

int read_dc_category(BitReader *br);

int decode_value(int mantissa, int size);

Huffman_node *create_huffman_tree();

void create_node(Huffman_node *node, const char *prefix, int run, int category);




#endif // HUFFMAN_H

