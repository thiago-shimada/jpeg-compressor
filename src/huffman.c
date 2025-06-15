#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "bitstream.h"

const char* huffman_ac_prefix[MAX_RUN][MAX_CATEGORY] = {
    {"1010", "00", "01", "100", "1011", "11010", "111000", "1111000", "1111110110", "1111111110000010", "1111111110000011", NULL},
    {NULL, "1100", "111001", "1111001", "111110110", "11111110110", "1111111110000100", "1111111110000101", "1111111110000110", "1111111110000111", "1111111110001000", NULL},
    {NULL, "11011", "11111000", "1111110111", "1111111110001001", "1111111110001010", "1111111110001011", "1111111110001100", "1111111110001101", "1111111110001110", "1111111110001111", NULL},
    {NULL, "111010", "111110111", "11111110111", "1111111110010000", "1111111110010001", "1111111110010010", "1111111110010011", "1111111110010100", "1111111110010101", "1111111110010110", NULL},
    {NULL, "111011", "1111111000", "1111111110010111", "1111111110011000", "1111111110011001", "1111111110011010", "1111111110011011", "1111111110011100", "1111111110011101", "1111111110011110", NULL},
    {NULL, "1111010", "1111111001", "1111111110011111", "1111111110100000", "1111111110100001", "1111111110100010", "1111111110100011", "1111111110100100", "1111111110100101", "1111111110100110", NULL},
    {NULL, "1111011", "11111111000", "1111111110100111", "1111111110101000", "1111111110101001", "1111111110101010", "1111111110101011", "1111111110101100", "1111111110101101", "1111111110101110", NULL},
    {NULL, "11111001", "11111111001", "1111111110101111", "1111111110110000", "1111111110110001", "1111111110110010", "1111111110110011", "1111111110110100", "1111111110110101", "1111111110110110", NULL},
    {NULL, "11111010", "111111111000000", "1111111110110111", "1111111110111000", "1111111110111001", "1111111110111010", "1111111110111011", "1111111110111100", "1111111110111101", "1111111110111110", NULL},
    {NULL, "111111000", "1111111110111111", "1111111111000000", "1111111111000001", "1111111111000010", "1111111111000011", "1111111111000100", "1111111111000101", "1111111111000110", "1111111111000111", NULL},
    {NULL, "111111001", "1111111111001000", "1111111111001001", "1111111111001010", "1111111111001011", "1111111111001100", "1111111111001101", "1111111111001110", "1111111111001111", "1111111111010000", NULL},
    {NULL, "111111010", "1111111111010001", "1111111111010010", "1111111111010011", "1111111111010100", "1111111111010101", "1111111111010110", "1111111111010111", "1111111111011000", "1111111111011001", NULL},
    {NULL, "1111111010", "1111111111011010", "1111111111011011", "1111111111011100", "1111111111011101", "1111111111011110", "1111111111011111", "1111111111100000", "1111111111100001", "1111111111100010", NULL},
    {NULL, "11111111010", "1111111111100011", "1111111111100100", "1111111111100101", "1111111111100110", "1111111111100111", "1111111111101000", "1111111111101001", "1111111111101010", "1111111111101011", NULL},
    {NULL, "111111110110", "1111111111101100", "1111111111101101", "1111111111101110", "1111111111101111", "1111111111110000", "1111111111110001", "1111111111110010", "1111111111110011", "1111111111110100", NULL},
    {"111111110111", "1111111111110101", "1111111111110110", "1111111111110111", "1111111111111000", "1111111111111001", "1111111111111010", "1111111111111011", "1111111111111100", "1111111111111101", "1111111111111110", "1111111111111111"}
};

const char* huffman_dc_prefix[MAX_CATEGORY] = {
    "010", "011", "100", "00", "101", "110",
    "1110", "11110", "111110", "1111110", "11111110", "111111110"
};

Huffman_node *create_huffman_tree() {
    Huffman_node *root = (Huffman_node *)malloc(sizeof(Huffman_node));
    root->run = 0;
    root->category = 0;
    root->is_leaf = false;
    root->left = NULL;
    root->right = NULL;
    const char *prefix = "";

    for(int i = 0; i < MAX_RUN; i++) {
        for(int j = 0; j < MAX_CATEGORY; j++) {
            if (huffman_ac_prefix[i][j] != NULL) {
                prefix = huffman_ac_prefix[i][j];
                create_node(root, prefix, i, j);
            }
        }
    }

    return root;
}

void create_node(Huffman_node *node, const char *prefix, int run, int category) {
    Huffman_node *current = node;

    for (int i = 0; prefix[i] != '\0'; i++) {
        if (prefix[i] == '0') {
            if (current->left == NULL) {
                current->left = (Huffman_node *)malloc(sizeof(Huffman_node));
                current->left->is_leaf = false;
                current->left->left = NULL;
                current->left->right = NULL;
            }
            current = current->left;
        } else if (prefix[i] == '1') {
            if (current->right == NULL) {
                current->right = (Huffman_node *)malloc(sizeof(Huffman_node));
                current->right->is_leaf = false;
                current->right->left = NULL;
                current->right->right = NULL;
            }
            current = current->right;
        }
    }
    current->run = run;
    current->category = category;
    current->is_leaf = true;
    current->left = NULL;
    current->right = NULL;
}


Huffman_node *read_ac_category(Huffman_node *huffman_tree, BitReader *br) {
    Huffman_node *current = huffman_tree;

    while (true) {

        int bit = bitreader_read_bit(br);

        if (bit == 0) {
            current = current->left;
        } else if (bit == 1) {
            current = current->right;
        }
        if (current->is_leaf) {
            return current;
        }else if (current == NULL) {
            return NULL; // Invalid prefix
        }
    }
}

int read_dc_category(BitReader *br) {
    char prefix[11];
    memset(prefix, 0, sizeof(prefix));

    for (int i = 0; i < 10; i++) {
        int bit = bitreader_read_bit(br);

        if (bit == 0) {
            prefix[i] = '0';
        } else if (bit == 1) {
            prefix[i] = '1';
        }

        prefix[i + 1] = '\0';

        for (int j = 0; j < MAX_CATEGORY; j++) {
            if (strcmp(prefix, huffman_dc_prefix[j]) == 0) {
                return j;
            }
        }
    }

    return -1;
}

int decode_value(int mantissa, int size) {
    // Se o bit mais significativo da mantissa for 0, o valor é negativo
    if (mantissa < (1 << (size - 1))) {
        int mask = (1 << size) - 1; // Máscara para o tamanho da categoria
        return -(~mantissa & mask); // Reconstrói o valor negativo
    }
    return mantissa; // Caso contrário, o valor é positivo
}

int get_category(int value) {
    int abs_value = abs(value);
    if (abs_value == 0) return 0;
    else if (abs_value == 1) return 1;
    else if (abs_value <= 3) return 2;
    else if (abs_value <= 7) return 3;
    else if (abs_value <= 15) return 4;
    else if (abs_value <= 31) return 5;
    else if (abs_value <= 63) return 6;
    else if (abs_value <= 127) return 7;
    else if (abs_value <= 255) return 8;
    else if (abs_value <= 511) return 9;
    else if (abs_value <= 1023) return 10;
    else if (abs_value <= 2047) return 11;
    else return -1;
}