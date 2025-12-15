#include <stdint.h>

#define DRAM_BASE 0x80003000
volatile uint32_t *BASE = (uint32_t *)DRAM_BASE;

// Output buffer in DRAM to inspect read-back values
volatile uint32_t *OUT = (uint32_t *)(0x80005000);

int main(void)
{
    // ----------------------------------------------------------
    // Patterned data values
    // ----------------------------------------------------------
    uint32_t pattern[5] = {
        0xAAAAAAAA,   // Pattern 1
        0xBBBBBBBB,   // Pattern 2
        0xCCCCCCCC,   // Pattern 3
        0xDDDDDDDD,   // Pattern 4
        0xEEEEEEEE    // Pattern 5
    };

    // ----------------------------------------------------------
    // Step 1: Write patterned data to sequential addresses
    // ----------------------------------------------------------
    for (int i = 0; i < 5; i++) {
        BASE[i] = pattern[i];

        // Small fence = ensures AXI write is committed
        asm volatile("fence rw, rw");
    }

    // ----------------------------------------------------------
    // Step 2: Read back same locations
    // ----------------------------------------------------------
    for (int i = 0; i < 5; i++) {
        OUT[i] = BASE[i];     // store result in another DRAM location
        asm volatile("fence r, r");
    }

    while (1);
    return 0;
}
