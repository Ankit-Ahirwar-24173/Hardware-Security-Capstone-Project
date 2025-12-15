#include <stdint.h>

volatile uint32_t *target = (uint32_t *)0x00020000;  // pick an address outside L1 cache warmup region
volatile uint32_t *mirror = (uint32_t *)0x00000100;  // safe observable region

int main(void) {
    // Step 1: read some other address to keep cache warm (not the target)
    volatile uint32_t dummy = *(volatile uint32_t *)0x00004000;
    (void)dummy;

    // Step 2: write to a *cold* address (target) -> forces write miss
    *target = 0xCAFEBABE;

    // Step 3: copy back for observation
    mirror[0] = *target;

    while (1); // stop here
    return 0;
}
