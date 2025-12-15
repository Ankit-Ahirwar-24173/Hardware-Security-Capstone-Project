// main.c
#include <stdint.h>
#include <stddef.h>

#define BYTE_BASE   0x00000000U
#define HALF_BASE   0x00000020U
#define WORD_BASE   0x00000040U
#define MIRROR_BASE 0x00000100U

/* volatile pointers for aligned accesses */
volatile uint8_t  * const byte_mem = (volatile uint8_t  * const)(uintptr_t)BYTE_BASE;
volatile uint16_t * const half_mem = (volatile uint16_t * const)(uintptr_t)HALF_BASE;
volatile uint32_t * const word_mem = (volatile uint32_t * const)(uintptr_t)WORD_BASE;
volatile uint32_t * const mirror   = (volatile uint32_t * const)(uintptr_t)MIRROR_BASE;

int main(void)
{
    /* 1) byte-wise checkerboard pattern */
    for (uint32_t i = 0U; i < 8U; ++i) {
        byte_mem[i] = (uint8_t)(((i & 1U) != 0U) ? 0xAAU : 0x55U);
    }

    /* 2) unaligned halfword writes using RISC-V 'sh' instruction
       This performs a single halfword store at unaligned addresses:
       addr = HALF_BASE + 1 + i*3  (i.e. 0x21, 0x24, 0x27, 0x2A)
       Using inline asm avoids the static-analysis warning about writing
       2 bytes into a "region of size 0". */
    for (uint32_t i = 0U; i < 4U; ++i) {
        uintptr_t addr = (uintptr_t)HALF_BASE + 1U + i * 3U;
        uint16_t  val  = (uint16_t)(0xBEEFU + (uint16_t)i);
        /* sh value, 0(addr) */
        asm volatile ("sh %1, 0(%0)" :: "r"(addr), "r"(val) : "memory");
    }

    /* 3) reverse-order word writes */
    for (int32_t i = 3; i >= 0; --i) {
        uint32_t v = 0x12345678U ^ ((uint32_t)i * 0x11111111U);
        word_mem[i] = v;
    }

    /* 4) cross-region reads and mixes -> mirror */
    for (uint32_t i = 0U; i < 4U; ++i) {
        uint8_t  b = byte_mem[7U - i];
        uint16_t h = half_mem[i];
        uint32_t w = word_mem[(i + 1U) & 0x3U];
        mirror[i] = ((uint32_t)b << 24) | ((uint32_t)h << 8) | (w & 0xFFU);
    }

    /* 5) rotate bits in mirror region */
    for (uint32_t i = 0U; i < 4U; ++i) {
        uint32_t tmp = mirror[i];
        mirror[i] = (tmp >> 4) | (tmp << 28);
    }

    /* keep CPU alive quietly (WFI) — change to `while(1); */
    for (;;) {
        asm volatile ("wfi");
    }

    /* never reached, but tidy */
    return 0;
}
