#include <stdint.h>

// BRAM base (valid region)
volatile uint32_t *mem    = (uint32_t *)0x00000040;   // valid AXI BRAM region
volatile uint32_t *mirror = (uint32_t *)0x00000100;   // copy-back region

int main(void) {
    // Normal full-word write (OKAY response expected)
    mem[0] = 0xAAAAAAAA;

    // Normal partial write (OKAY, masked by WSTRB internally)
    *((volatile uint8_t *)(&mem[0])) = 0x11;  // only lowest byte updated

    // Overlapping but legal write (still OKAY)
    *((volatile uint16_t *)((uint8_t *)&mem[0] + 1)) = 0x2233;

    // Unusual Case 1: Out-of-range write (should trigger DECERR)
    volatile uint32_t *bad_addr = (uint32_t *)0x00001000; // outside BRAM range
    *bad_addr = 0xBADF00D;

    // Unusual Case 2: Misaligned halfword crossing word boundary (should trigger SLVERR)
    *((volatile uint16_t *)((uint8_t *)&mem[0] + 3)) = 0xBEEF;

    //Mirror back (read valid memory to observe results)
    for (int i = 0; i < 4; i++) {
        mirror[i] = mem[i];  // copy first few words for observation
    }

    while (1) {
        // Infinite loop so you can probe signals in ILA
    }

    return 0;
}
