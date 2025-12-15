#include <stdint.h>

#define DRAM_CACHED   0x80003000  // normal DRAM (cacheable)
#define DRAM_UNCACHED 0xC0003000  // uncached / MMIO region (no L1 caching)

volatile uint32_t *CACHED   = (uint32_t *)(DRAM_CACHED);
volatile uint32_t *UNCACHED = (uint32_t *)(DRAM_UNCACHED);
volatile uint32_t *OUT      = (uint32_t *)(0x80005000);

int main(void)
{
    // ----------------------------------------------------------
    // 1) Warm-up to force some cache state
    // ----------------------------------------------------------
    volatile uint32_t w = *(uint32_t *)0x80007000;
    (void)w;

    // ----------------------------------------------------------
    // 2) STRANGE PATTERN:
    // Same address index, but first UNCACHED then CACHED.
    // ----------------------------------------------------------

    // Uncached write (will FORCE AWCACHE = 0011)
    UNCACHED[0] = 0xAAAAAAAA;

    // Fence so CPU commits immediately (weird AXI timing otherwise)
    asm volatile("fence rw, rw");

    // Cached write (AWCACHE = 1111) immediately after uncached
    CACHED[0] = 0xBBBBBBBB;

    // Fence to make sure it hits AXI
    asm volatile("fence rw, rw");

    // Uncached load — single beat AR (ARCACHE = 0011)
    volatile uint32_t a = UNCACHED[0];

    // Cached load — line fill (ARCACHE = 1111)
    volatile uint32_t b = CACHED[0];

    // ----------------------------------------------------------
    // 3) Export results for checking
    // ----------------------------------------------------------
    OUT[0] = a;
    OUT[1] = b;

    while (1) {
        // spin
    }

    return 0;
}
