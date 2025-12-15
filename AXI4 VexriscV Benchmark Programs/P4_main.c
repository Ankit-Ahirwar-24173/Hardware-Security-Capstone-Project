// main.c  (RV32I, bare-metal, no secrets, within 0x00000–0x1FFFF)
#include <stdint.h>

#define ARR_BASE  0x00000080
#define ARR_WORDS 1024      // 4 KB
#define STRIDE    16        // bytes

volatile uint32_t * const arr = (volatile uint32_t*)(uintptr_t)ARR_BASE;
volatile uint32_t * const logbuf = (volatile uint32_t*)(uintptr_t)0x00000100; // results

static inline uint32_t rdcycle(void){
    uint32_t x; asm volatile ("csrr %0, mcycle" : "=r"(x));
    return x;
}

static void touch_all(void){
    for (int i=0;i<ARR_WORDS;i++) arr[i] ^= 0x01010101;   // warm cache
}

static void evict_like(void){
    // Sweep a separate region to evict prior lines (simple thrash)
    volatile uint32_t * const ev = (volatile uint32_t*)(uintptr_t)0x00002000;
    for (int i=0;i<2048;i++) { ev[i&255] += i; } // small loop, wraps safely
}

int main(void){
    // init
    for (int i=0;i<ARR_WORDS;i++) arr[i] = i;

    // 1) warm access timing
    touch_all();
    for (int i=0, j=0; i<256; i++, j+=STRIDE/4){
        uint32_t t0 = rdcycle();
        volatile uint32_t v = arr[j & (ARR_WORDS-1)];
        (void)v;
        uint32_t t1 = rdcycle();
        logbuf[i] = t1 - t0;        // expected: small latency
    }

    // 2) cold-ish access timing after eviction
    evict_like();
    for (int i=0, j=0; i<256; i++, j+=STRIDE/4){
        uint32_t t0 = rdcycle();
        volatile uint32_t v = arr[j & (ARR_WORDS-1)];
        (void)v;
        uint32_t t1 = rdcycle();
        logbuf[256+i] = t1 - t0;    // expected: larger latency
    }

    for(;;) asm volatile("wfi");
    return 0;
}
