RISCV_PREFIX = riscv32-unknown-elf
CFLAGS = -O3 -march=rv32i -mabi=ilp32 -nostdlib -Wl,--gc-sections -ffreestanding
LDSCRIPT = linker.ld

all: software.elf software.bin software.hex software.memhex software.coe software.disasm

software.elf: main.c ctrl.S $(LDSCRIPT)
	$(RISCV_PREFIX)-gcc $(CFLAGS) -T$(LDSCRIPT) -o $@ ctrl.S main.c

software.bin: software.elf
	$(RISCV_PREFIX)-objcopy -O binary $< $@

software.hex: software.bin
	hexdump -v -e '1/4 "%08X\n"' $< > $@

software.memhex: software.bin
	hexdump -v -e '1/4 "%08X\n"' $< > $@

software.coe: software.bin
	@echo "memory_initialization_radix=16;" > $@
	@echo "memory_initialization_vector=" >> $@
	@hexdump -v -e '1/4 "%08X,\n"' $< | sed '$$s/,$$/;/' >> $@

software.disasm: software.elf
	$(RISCV_PREFIX)-objdump -d $< > $@

clean:
	rm -f *.elf *.bin *.hex *.memhex *.coe *.disasm
