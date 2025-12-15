.section .init
.globl _start
_start:
    la sp, _stack_end     # Initialize stack pointer
    call main             # Call main
    j .                   # Infinite loop after main
