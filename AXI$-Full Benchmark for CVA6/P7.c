#include <stdint.h>

// ------------------------------------------------------
// CVA6 memory map assumption:
// 0xC000_0000 – 0xC0FF_FFFF : Uncached, device, bufferable region
// (maps to AWCACHE = 0001 exactly)
//
// Write here to force bufferable-only AXI behavior.
// ------------------------------------------------------
#define BUF_BASE   0xC0003000       // bufferable region
#define OUT_BASE   0x80005000       // normal DRAM (for checking)

volatile uint32_t *BUF = (uint32_t *)BUF_BASE;
volatile uint32_t *OUT = (uint32_t *)OUT_BASE;

int main()
{
    // ---------------------------------------------
    // Patterned data and increasing addresses
    // ---------------------------------------------
    uint32_t pattern[5] = {
        0x11111111,
        0x22222222,
        0x33333333,
        0x44444444,
        0x55555555
    };

    // -------------------------------------------------------------
    // 1) WRITE 5 words to bufferable-only region
    //    -> should create AWCACHE = 0001
    //    -> No cache allocate
    //    -> No merging
    //    -> Writes may be delayed or posted!
    // -------------------------------------------------------------
    for (int i = 0; i < 5; i++) {
        BUF[i] = pattern[i];
    }

    // Strong ordering so writes actually hit the bus
    asm volatile("fence rw, rw");

    // -------------------------------------------------------------
    // 2) READ back the 5 values
    //    -> should create ARCACHE = 0001
    //    -> single-beat each read
    // -------------------------------------------------------------
    uint32_t r[5];
    for (int i = 0; i < 5; i++) {
        r[i] = BUF[i];
    }

    // -------------------------------------------------------------
    // 3) Export results into DRAM (cached region)
    //    -> easy to check in waveform/memory
    // -------------------------------------------------------------
    for (int i = 0; i < 5; i++) {
        OUT[i] = r[i];
    }

    while (1);
    return 0;
}
