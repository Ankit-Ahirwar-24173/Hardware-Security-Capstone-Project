#include <stdint.h>

volatile uint8_t*  byte_mem  = (uint8_t*)  0x00000000;
volatile uint16_t* half_mem  = (uint16_t*) 0x00000020;
volatile uint32_t* word_mem  = (uint32_t*) 0x00000040;
volatile uint32_t* mirror    = (uint32_t*) 0x00000100;

int main() {
	
	register int i __asm__("a0");
    // 1. Byte-wise write
    for (i = 0; i < 8; i++) {
        byte_mem[i] = i * 3;       // AXI write with WSTRB = 0x1, 0x2, etc.
    }

    // 2. Halfword write
    for (i = 0; i < 4; i++) {
        half_mem[i] = i * 0x1234;  // WSTRB = 0x3 (2 bytes)
    }

    // 3. Word write
    for (i = 0; i < 4; i++) {
        word_mem[i] = 0xA5A50000 | i; // WSTRB = 0xF (full word)
    }

    // 4. Read from all and mix them
    for (i = 0; i < 4; i++) {
        uint32_t w = word_mem[i];
        uint16_t h = half_mem[i];
        uint8_t  b = byte_mem[i];
        mirror[i] = (w ^ h) + b;       // Multiple reads + write
    }

    // 5. Loop and shift
    uint32_t sum = 0;
    for (i = 0; i < 4; i++) {
        sum += mirror[i];
        mirror[i] = sum >> 1;
    }

    while (1); // Hold here
    return 0;
}
