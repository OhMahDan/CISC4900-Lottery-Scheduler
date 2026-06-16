CC = gcc
AS = as
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -Icpu -Idrivers -Ilibc
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Final Product 
casino.bin: boot.o kernel.o string.o vga.o gdt.o gdt_flush.o idt.o idt_flush.o isr_handler.o isr.o tss_flush.o prime.o
	ld $(LDFLAGS) -o casino.bin boot.o kernel.o string.o vga.o gdt.o gdt_flush.o idt.o idt_flush.o isr_handler.o isr.o tss_flush.o prime.o

# Assembly Objects
boot.o: boot/boot.s
	$(AS) $(ASFLAGS) $< -o boot.o

gdt_flush.o: cpu/gdt_flush.s
	$(AS) $(ASFLAGS) $< -o gdt_flush.o

idt_flush.o: cpu/idt_flush.s
	$(AS) $(ASFLAGS) $< -o idt_flush.o

isr_handler.o: cpu/isr_handler.s
	$(AS) $(ASFLAGS) $< -o isr_handler.o

tss_flush.o: cpu/tss_flush.s
	$(AS) $(ASFLAGS) $< -o tss_flush.o

# C Objects
kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o kernel.o

gdt.o: cpu/gdt.c
	$(CC) $(CFLAGS) -c $< -o gdt.o

idt.o: cpu/idt.c
	$(CC) $(CFLAGS) -c $< -o idt.o

isr.o: cpu/isr.c
	$(CC) $(CFLAGS) -c $< -o isr.o

prime.o: user/prime.c
	$(CC) $(CFLAGS) -c $< -o prime.o

# String Library Object
string.o: libc/string.c
	$(CC) $(CFLAGS) -c $< -o string.o

# VGA Library Object
vga.o: drivers/vga.c
	$(CC) $(CFLAGS) -c $< -o vga.o

# Emulator Target
run: casino.bin 
	qemu-system-i386 -kernel casino.bin

# Cleaup Utility
clean: 
	rm -f *.o *.bin