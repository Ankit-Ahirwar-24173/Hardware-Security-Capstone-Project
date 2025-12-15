#include <stdint.h>

// DRAM region assumed cacheable in your system
#define DRAM_A   ((volatile uint32_t*)0x80002000)
#define DRAM_B   ((volatile uint32_t*)0x80003000)
#define DRAM_C   ((volatile uint32_t*)0x80004000)

// A known NON-cacheable MMIO region (replace with your UART / CLINT / PLIC base)
#define MMIO_X   ((volatile uint32_t*)0x10000000)

int main() {

    // 1. Force DRAM_A into cache (read miss → allocate)
    volatile uint32_t t1 = DRAM_A[0];

    // 2. Thrash the line by writing to DRAM_B (write miss → allocate)
    DRAM_B[0] = 0x11111111;

    // 3. Immediately read DRAM_B again (should hit now)
    volatile uint32_t t2 = DRAM_B[0];

    // 4. Create a conflicting pattern: write DRAM_A again (write hit? depends on eviction)
    DRAM_A[0] = 0x22222222;

    // 5. Trigger a **non-cacheable** transaction to observe ARCACHE/AWCACHE=0000
    MMIO_X[0] = 0xDEADBEEF;

    // 6. Now force a burst of writes that typically leads to write-combining on AXI
    for (int i = 0; i < 32; i++)
        DRAM_C[i] = 0xA0000000 + i;

    // 7. Alternate read + write pattern to same line → forces RAW hazards on AXI
    for (int i = 0; i < 16; i++) {
        DRAM_B[0] = 0xB0000000 + i;
        volatile uint32_t r = DRAM_B[0];
        (void)r;
    }

    // 8. Create cross-line conflict → flush / eviction / new allocation
    DRAM_A[16] = 0x33333333;

    // Prevent compiler removal
    (void)t1;
    (void)t2;

    while(1); // Observe AXI on waveform
    return 0;
}
