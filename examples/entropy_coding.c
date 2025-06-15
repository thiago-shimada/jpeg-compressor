#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "ac_encode.h"
#include "dc_encode.h"

int main() {
    double block[64] = {
        -19.000, 9.000, -0.000, 0.000, 0.000, 0.000, 0.000, -0.000,
        -7.000, -0.000, 2.000, -0.000, 0.000, -0.000, 0.000, 0.000,
        1.000, -1.000, -0.000, 0.000, -0.000, -0.000, -0.000, 0.000,
        1.000, 0.000, -0.000, -0.000, -0.000, 0.000, -0.000, -0.000,
        0.000, -0.000, -0.000, -0.000, -0.000, -0.000, -0.000, -0.000,
        -0.000, -0.000, 0.000, 0.000, 0.000, -0.000, 0.000, -0.000,
        0.000, 0.000, -0.000, -0.000, 0.000, 0.000, 0.000, -0.000,
        0.000, 0.000, -0.000, -0.000, -0.000, 0.000, 0.000, 0.000
    };

    BitWriter writer;
    bitwriter_init(&writer, "saida.bin");

    int previous_dc = 0;
    encode_dc(&writer, block[0], previous_dc);
    encode_ac(&writer, block);

    bitwriter_flush(&writer);
    return 0;
}

