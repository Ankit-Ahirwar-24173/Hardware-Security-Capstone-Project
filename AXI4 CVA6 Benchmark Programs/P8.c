#include <stdint.h>

#define REGION_CACHEABLE_NOALLOC   0xA0003000   // MP regions usually "cacheable+bufferable" but NO allocate
#define REGION_OUTPUT               0x80005000

volatile uint32_t *REG   = (uint32_t *)(REGION_CACHEABLE_NOALLOC);
volatile uint32_t *OUT   = (uint32_t *)(REGION_OUTPUT);

int main() {

    // ----------------------------------------------------------
    // 1) Write patterned data to consecutive addresses
    //    AXI → AWCACHE = 0b0010 (cacheable, bufferable, NO allocate)
    // ----------------------------------------------------------
    REG[0] = 0x11111111;
    REG[1] = 0x22222222;
    REG[2] = 0x33333333;
    REG[3] = 0x44444444;
    REG[4] = 0x55555555;

    // Ensure writes reach AXI
    asm volatile("fence rw, rw");

    // ----------------------------------------------------------
    // 2) Read them back (ARCACHE = 0b0010)
    // ----------------------------------------------------------
    uint32_t r0 = REG[0];
    uint32_t r1 = REG[1];
    uint32_t r2 = REG[2];
    uint32_t r3 = REG[3];
    uint32_t r4 = REG[4];

    // ----------------------------------------------------------
    // 3) Store results in DRAM for debugging waveforms
    // ----------------------------------------------------------
    OUT[0] = r0;
    OUT[1] = r1;
    OUT[2] = r2;
    OUT[3] = r3;
    OUT[4] = r4;

    while (1);
    return 0;
}
