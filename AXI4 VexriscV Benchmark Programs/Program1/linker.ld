ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 128K
}

SECTIONS
{
  .text : {
    *(.init)
    *(.text*)
    *(.rodata*)
  } > RAM

  .data : {
    *(.data*)
    *(.sdata*)
  } > RAM

  .bss : {
    *(.bss*)
    *(.sbss*)
    *(COMMON)
  } > RAM

  .stack (NOLOAD) : {
    . = ALIGN(4);
    _stack_start = .;
    . = . + 2K;
    _stack_end = .;
  } > RAM
}
