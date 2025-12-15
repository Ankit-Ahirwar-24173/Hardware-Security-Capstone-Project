#include <stdint.h>

volatile uint32_t *mem    = (uint32_t *)0x00000040;   // target BRAM region
volatile uint32_t *mirror = (uint32_t *)0x00000100;   // observation region

int main(void) {
    // --- Baseline full-word write ---
    mem[0] = 0xAAAAAAAA;        // WSTRB = 1111

    // --- Overlapping writes into the same word ---
    *((volatile uint8_t *)&mem[0]) = 0x11;            // byte 0 only, WSTRB = 0001
    *((volatile uint16_t *)((uint8_t *)mem + 1)) = 0x2233; // bytes 1-2, WSTRB = 0110
    *((volatile uint8_t *)((uint8_t *)mem + 3)) = 0x44;    // byte 3 only, WSTRB = 1000

    // After this sequence, the word at mem[0] should look like:
    // 0x44_33_22_11

    // --- Second test: different overlap order ---
    mem[1] = 0x55555555;        // WSTRB = 1111
    *((volatile uint16_t *)&mem[1]) = 0xABCD;         // bytes 0-1, WSTRB = 0011
    *((volatile uint8_t *)((uint8_t *)&mem[1] + 2)) = 0xEF; // byte 2, WSTRB = 0100

    // Expected final word at mem[1]:
    // 0x55_EF_CD_AB

    // Copy back results for observation
    mirror[0] = mem[0];
    mirror[1] = mem[1];

    while (1);
    return 0;
}

