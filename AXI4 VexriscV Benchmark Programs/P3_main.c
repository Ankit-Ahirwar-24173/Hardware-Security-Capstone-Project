#include <stdint.h>

volatile uint8_t*  byte_mem  = (uint8_t*)  0x00000000;
volatile uint16_t* half_mem  = (uint16_t*) 0x00000020;
volatile uint32_t* word_mem  = (uint32_t*) 0x00000040;

int main() {
    int i;

    // Fill byte memory with 0x11, 0x22, ..., 0x88
    for (i = 0; i < 8; i++) {
        byte_mem[i] = (i + 1) * 0x11;
    }

    // Fill halfword memory with pattern 0x1001, 0x1002, ...
    for (i = 0; i < 8; i++) {
        half_mem[i] = 0x1000 + i + 1;
    }

    // Fill word memory with pattern 0xABCDEF01, 0xABCDEF02, ...
    for (i = 0; i < 8; i++) {
        word_mem[i] = 0xABCDEF00 | (i + 1);
    }

    while (1); // halt forever
    return 0;
}
