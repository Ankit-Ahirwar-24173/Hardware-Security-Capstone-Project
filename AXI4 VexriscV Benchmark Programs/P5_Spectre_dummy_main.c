#include <stdint.h>
#include <stddef.h>

/*
 * spectre_gadget:
 *    idx           - index to read
 *    simpleByteArray - pointer to array
 *    length        - length of array
 *    probeTable    - pointer to probeTable
 *    junk          - pointer to junk accumulator variable (32-bit)
 */
void spectre_gadget(
    size_t idx,
    const uint8_t *simpleByteArray,
    size_t length,
    const uint8_t *probeTable,
    volatile uint32_t *junk)  // changed to 32-bit pointer
{
    __asm__ volatile (
        // Bounds check: if (idx >= length) goto end
        "bge %1, %3, 1f\n\t"
        // Load secret byte: tmp = simpleByteArray[idx]
        "add t0, %2, %1\n\t"
        "lbu t1, 0(t0)\n\t"
        // Multiply by 4096 (shift left 12)
        "slli t1, t1, 12\n\t"
        // Mask index: t1 &= 0x1ffffff
        "li t2, 0x1ffffff\n\t"
        "and t1, t1, t2\n\t"
        // Probe: probeTable[t1]
        "add t3, %4, t1\n\t"
        "lbu t4, 0(t3)\n\t"
        // XOR with *junk
        "lw t5, 0(%5)\n\t"      // use lw (32-bit load)
        "xor t4, t4, t5\n\t"
        // Save junk back
        "sw t4, 0(%5)\n\t"      // use sw (32-bit store)
        "1:\n\t"
        : // No outputs
        : "r"(idx),           // %0
          "r"(idx),           // %1
          "r"(simpleByteArray), // %2
          "r"(length),        // %3
          "r"(probeTable),    // %4
          "r"(junk)           // %5
        : "t0", "t1", "t2", "t3", "t4", "t5", "memory"
    );
}

uint8_t simpleByteArray[64] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    
};

uint8_t probeTable[0x10000] = {0}; // 64 KB

volatile uint32_t junk = 0;

int main(void) {
    size_t test_idx = 4;
    spectre_gadget(test_idx, simpleByteArray, sizeof(simpleByteArray), probeTable, &junk);
    while (1) {}
    return 0;  // never reached
}
