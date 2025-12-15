#include <stdint.h>

static volatile uint8_t  secret_byte = 0x3A;        // benign “secret” we already know
static uint8_t probe[256 * 64];                     // probe table (stride = 64 bytes)
static uint32_t timings[256];                       // store measured latencies

// Read mcycle CSR for very rough timing on VexRiscv
static inline uint32_t rdcycle(void) {
    uint32_t c;
    asm volatile ("csrr %0, mcycle" : "=r"(c));
    return c;
}

// Touch many lines to (roughly) evict small caches
static void evict(void) {
    static uint8_t trash = 0;
    for (int i = 0; i < 4096; i += 32) {
        trash ^= probe[(i & 0xFF) * 64];
    }
}

// Victim-like function: encode a known byte into cache
static inline void encode(uint8_t b) {
    volatile uint8_t *p = &probe[(uint32_t)b * 64];
    // “Transient-looking” sequence (but entirely architecturally legal here)
    // This mimics: value -> shift -> probe[value<<6]
    asm volatile ("" ::: "memory");
    (void)*p; // bring that line into cache
    asm volatile ("" ::: "memory");
}

// Time each candidate line
static uint32_t time_access(volatile uint8_t *p) {
    uint32_t t0 = rdcycle();
    (void)*p;
    uint32_t t1 = rdcycle();
    return t1 - t0;
}

int main(void) {
    // 1) Initialize probe to avoid linker zero-page surprises
    for (int i = 0; i < (int)sizeof(probe); ++i) probe[i] = (uint8_t)i;

    // 2) Evict (crudely), then encode our benign byte into cache
    evict();
    encode(secret_byte);

    // 3) Measure timing of each candidate slot
    for (int i = 0; i < 256; ++i) {
        volatile uint8_t *p = &probe[i * 64];
        timings[i] = time_access(p);
    }

    // 4) Write a tiny result banner into a fixed RAM area you already watch
    //    (e.g., your 0x100 “mirror” window). This keeps AXI traffic visible.
    volatile uint32_t *mirror = (volatile uint32_t *)0x00000100;
    mirror[0] = 0xCAFE0000 | (uint32_t)secret_byte;   // which byte we encoded
    // Find “winner” (fastest)
    uint32_t best_i = 0, best_t = timings[0];
    for (int i = 1; i < 256; ++i) {
        if (timings[i] < best_t) { best_t = timings[i]; best_i = (uint32_t)i; }
    }
    mirror[1] = (best_i << 8) | (best_t & 0xFF);      // rough report
    // Dump a small subset of timing bins so you can view on the bus
    for (int i = 0; i < 8; ++i) mirror[2 + i] = timings[i];

    // Idle 
    for (;;) asm volatile ("wfi");
}

