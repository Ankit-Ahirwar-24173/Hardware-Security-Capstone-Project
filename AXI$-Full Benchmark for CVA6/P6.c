#include <stdint.h>

#define DRAM_BASE 0x80003000U
#define OUT_BASE  0x80005000U

volatile uint32_t *OUT = (uint32_t *)OUT_BASE;

int main(void) {
    // Addresses: contiguous 32-bit words
    volatile uint32_t *addr0 = (uint32_t *)(DRAM_BASE + 0*4);
    volatile uint32_t *addr1 = (uint32_t *)(DRAM_BASE + 1*4);
    volatile uint32_t *addr2 = (uint32_t *)(DRAM_BASE + 2*4);
    volatile uint32_t *addr3 = (uint32_t *)(DRAM_BASE + 3*4);
    volatile uint32_t *addr4 = (uint32_t *)(DRAM_BASE + 4*4);

    // Distinct patterns (easy to spot in hex)
    const uint32_t p0 = 0x11111111U;
    const uint32_t p1 = 0x22222222U;
    const uint32_t p2 = 0x33333333U;
    const uint32_t p3 = 0xDEADBEEFU;
    const uint32_t p4 = 0xBADCAB1EU;

    // Write patterns
    addr0[0] = p0;
    addr1[0] = p1;
    addr2[0] = p2;
    addr3[0] = p3;
    addr4[0] = p4;

    // Ensure stores are ordered out (helps make AXI transactions visible in expected order)
    asm volatile("fence rw, rw" ::: "memory");

    // Read back (forces loads / line fills)
    volatile uint32_t r0 = addr0[0];
    volatile uint32_t r1 = addr1[0];
    volatile uint32_t r2 = addr2[0];
    volatile uint32_t r3 = addr3[0];
    volatile uint32_t r4 = addr4[0];

    // Mirror reads to OUT so you can inspect memory/waveform content after program halts
    OUT[0] = r0;
    OUT[1] = r1;
    OUT[2] = r2;
    OUT[3] = r3;
    OUT[4] = r4;

    // stop here
    while (1) { asm volatile("wfi"); }
    return 0;
}
