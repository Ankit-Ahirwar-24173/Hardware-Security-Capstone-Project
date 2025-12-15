/* #include <stdint.h>

volatile uint32_t *mem    = (uint32_t *)0x00000040;   // target region
volatile uint32_t *mirror = (uint32_t *)0x00000100;   // observation region

int main(void) {
    // 1. Full-word write (WSTRB = 1111)
    mem[0] = 0xDEADBEEF;

    // 2. Byte overwrite, lowest byte only (WSTRB = 0001)
    *((volatile uint8_t *)&mem[0]) = 0x11;

    // 3. Halfword overwrite at aligned address (WSTRB = 0011)
    *((volatile uint16_t *)&mem[0]) = 0x2222;

    // 4. Halfword overwrite at unaligned address (+2 offset → WSTRB = 1100)
    *((volatile uint16_t *)((uint8_t *)mem + 2)) = 0x3333;

    // 5. Word overwrite but unaligned at +1 (forces split: two beats with WSTRB like 1110 + 0001)
    *((volatile uint32_t *)((uint8_t *)mem + 1)) = 0x44444444;

    // 6. Read back what hardware stored after each step
    mirror[0] = mem[0];   // after corruption #1..#5
    mirror[1] = mem[1];   // see if unaligned spill touched next word

    while (1);
    return 0;
}
 */
 
 #include <stdint.h>

static inline void do_sw(uint32_t addr, uint32_t v){
    register uint32_t a __asm__("a0") = addr;
    register uint32_t x __asm__("a1") = v;
    __asm__ volatile ("sw %1, 0(%0)\n\tfence iorw, iorw" :: "r"(a), "r"(x) : "memory");
}
static inline void do_sb(uint32_t addr, uint8_t v){
    register uint32_t a __asm__("a0") = addr;
    register uint32_t x __asm__("a1") = v;
    __asm__ volatile ("sb %1, 0(%0)\n\tfence iorw, iorw" :: "r"(a), "r"(x) : "memory");
}
static inline void do_sh(uint32_t addr, uint16_t v){
    register uint32_t a __asm__("a0") = addr;
    register uint32_t x __asm__("a1") = v;
    __asm__ volatile ("sh %1, 0(%0)\n\tfence iorw, iorw" :: "r"(a), "r"(x) : "memory");
}

#define WORD_BASE   0x00000040u
#define MIRROR_BASE 0x00000100u

volatile uint32_t * const word   = (volatile uint32_t*)WORD_BASE;
volatile uint32_t * const mirror = (volatile uint32_t*)MIRROR_BASE;

int main(void){
    do_sw(WORD_BASE,   0xDEADBEEF);   // expect WSTRB=1111, WDATA=DEADBEEF, AW=0x40
    do_sb(WORD_BASE,   0x55);         // expect WSTRB=0001, WDATA=00000055, AW=0x40
    do_sh(WORD_BASE+1, 0xBEEF);       // expect WSTRB=0110, WDATA=0000BEEF, AW=0x41

    mirror[0] = word[0];              // read back to 0x100 for easy probing
    for(;;) __asm__ volatile("wfi");
    return 0;
}
