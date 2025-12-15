#include <stdint.h>

#define TARGET_BASE 0x00007000   // cold region for burst
#define MIRROR_BASE 0x00000080   // small safe region for results

volatile uint32_t *target = (uint32_t *)TARGET_BASE;
volatile uint32_t *mirror = (uint32_t *)MIRROR_BASE;

int main(void) {
    // Step 1: warm up some other cache line
    volatile uint32_t dummy = *(volatile uint32_t *)0x00004000;
    (void)dummy;

    // Step 2: burst write 16 words (64B) -> guaranteed write miss
    for (int i = 0; i < 16; i++) {
        target[i] = 0x11110000 | i;
    }

    // Step 3: read them back into mirror region for observation
    for (int i = 0; i < 16; i++) {
        mirror[i] = target[i];
    }

    while (1);
    return 0;
}
