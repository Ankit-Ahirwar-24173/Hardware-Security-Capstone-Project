#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

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

#define ARRAY_SIZE 256
#define PROBE_TABLE_SIZE 0x10000

// Larger input array to exceed cache size
uint8_t simpleByteArray[ARRAY_SIZE];
uint8_t probeTable[PROBE_TABLE_SIZE] = {0};
volatile uint32_t junk = 0;

void delay(volatile int count) {
    while (count--) {
        __asm__ volatile ("nop");
    }
}

int main(void) {
	
    // Initialize simpleByteArray with some pseudo-random data
    for (int i = 0; i < ARRAY_SIZE; i++) {
        simpleByteArray[i] = (i * 37 + 13) & 0xFF;
    }

    // Loop repeatedly over indices to cause cache churn and bus activity
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        spectre_gadget(i, simpleByteArray, ARRAY_SIZE, probeTable, &junk);
        delay(1000); // small delay between calls
    }
	
	while (1) {
		return 0;
	}
	
}
/*	//unsigned int *ptr1 = (unsigned int*) (0x8100);
	//short *ptr1 = (short *) (0x8100);
	uint8_t *ptr1 = (uint8_t *) (0x8100);
	
	for(uint8_t i=0; i<100; i=i+1){
		*(ptr1+i) = i;
		}
	for(uint8_t i=0; i<100; i=i+1){
		*(ptr1) = *(ptr1+i);
		}
    while (1) {
		
	}

    return 0;  // never reached
}
 
 
 
/*#include <stdlib.h>
#include <stdint.h>

int main(void) {
    unsigned int *ptr1 = (unsigned int*) malloc(sizeof(unsigned int) * 10);
    //unsigned int *ptr2;
    //unsigned int var2;

    //ptr2 = &var2;

    while (1) {
        for (unsigned int i = 0; i < 10; i++) {
            *(ptr1 + i) = i;
        }
    }

    return 0;  // never reached
}

 //Provide minimal sbrk implementation for malloc 
void *_sbrk(ptrdiff_t incr) {
    extern char _heap_start; // Defined in linker.ld 
    extern char _heap_end;
    static char *heap_ptr = 0;
    char *prev_heap_ptr;

    if (heap_ptr == 0)
        heap_ptr = &_heap_start;

    prev_heap_ptr = heap_ptr;
    if (heap_ptr + incr > &_heap_end) {
        return (void *) -1; // Out of memory 
    }

    heap_ptr += incr;
    return (void *) prev_heap_ptr;
}
*/
