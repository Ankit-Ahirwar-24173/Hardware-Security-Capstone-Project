#include <stdint.h>

#define LINE_SIZE 64          // CVA6 uses 64B cache lines
#define DRAM_BASE 0x80000000  // DRAM base address

// Choose addresses in same DRAM zone but different cache sets
// to trigger cache misses
volatile uint32_t *A   = (uint32_t *)(DRAM_BASE + 0x00003000);
volatile uint32_t *B   = (uint32_t *)(DRAM_BASE + 0x00003000 + LINE_SIZE);
volatile uint32_t *C   = (uint32_t *)(DRAM_BASE + 0x00003000 + 2 * LINE_SIZE);

// Output region for observing effects in waveforms / memory
volatile uint32_t *OUT = (uint32_t *)(DRAM_BASE + 0x00005000);

int main(void)
{
    // ----------------------------------------------------------
    // 1) Warm up unrelated region (fills cache with junk)
    // ----------------------------------------------------------
    for (int i = 0; i < 32; i++) {
        volatile uint32_t temp =
            *(uint32_t *)(DRAM_BASE + 0x00007000 + i * 4);
        (void)temp;
    }

    // ----------------------------------------------------------
    // 2) Hammer: alternating writes that cause continuous WRITE-MISS
    //    Each write allocates a new cache line (Write Allocate)
    // ----------------------------------------------------------
    for (int i = 0; i < 256; i++) {
        *A = (uint32_t)i;
        *B = (uint32_t)(~i);
        *C = (uint32_t)(i ^ 0x12345678);
    }

    // ----------------------------------------------------------
    // 3) Re-read lines (should HIT initially)
    // ----------------------------------------------------------
    volatile uint32_t r1 = *B;
    volatile uint32_t r2 = *A;
    volatile uint32_t r3 = *C;

    // ----------------------------------------------------------
    // 4) Evict cache: touch large memory region
    //    Forces eviction → next reads must MISS
    // ----------------------------------------------------------
    for (int i = 0; i < 2048; i++) {
        volatile uint32_t trash =
            *(uint32_t *)(DRAM_BASE + 0x00008000 + i * 4);
        (void)trash;
    }

    // ----------------------------------------------------------
    // 5) Re-read A/B/C (guaranteed MISS)
    // ----------------------------------------------------------
    r1 = *A;
    r2 = *B;
    r3 = *C;

    // ----------------------------------------------------------
    // 6) Mirror results to DRAM for visibility
    // ----------------------------------------------------------
    OUT[0] = r1;
    OUT[1] = r2;
    OUT[2] = r3;

    // ----------------------------------------------------------
    // 7) Stay here for waveform inspection
    // ----------------------------------------------------------
    while (1) {
        // spin
    }

    return 0;
}
