/*#include <stdint.h>

volatile uint32_t *mem = (uint32_t *)0x00000040;   //target region we’ll write to
volatile uint32_t *mirror = (uint32_t *)0x00000100;  //another region where we’ll copy back the result for observation


int main(void) {
     // Full-word write 
    mem[0] = 0xAAAAAAAA;

    // Byte-only overwrite (lowest byte only, rest masked by WSTRB)
    *((volatile uint8_t *)&mem[0]) = 0x55;

    // Halfword overwrite (unaligned, offset = +1)
    *((volatile uint16_t *)((uint8_t *)mem + 1)) = 0xBEEF;

    // Read back what hardware actually stored
    mirror[0] = mem[0];

    while (1);
    return 0;
}
*/

#include <stdint.h>

volatile uint32_t *mem    = (uint32_t *)0x00000040;   // valid region
volatile uint32_t *mirror = (uint32_t *)0x00000100;   // observation

int main(void) {
    // Full-word write (OKAY)
    mem[0] = 0xAAAAAAAA;

    // Byte overwrite (OKAY, handled by WSTRB)
    *((volatile uint8_t *)&mem[0]) = 0x55;

    // Halfword overwrite, unaligned (still usually OKAY)
    *((volatile uint16_t *)((uint8_t *)mem + 1)) = 0xBEEF;

    // ---- FORCING BRESP ERROR ----
    // Write to an address outside mapped region.
    // This should trigger DECERR if no slave responds.
    volatile uint32_t *bad = (uint32_t *)0xDEADBEEF;
    *bad = 0x12345678;    // expect BRESP != OKAY here

    // Read back normal region
    mirror[0] = mem[0];

    while (1);
    return 0;
}
