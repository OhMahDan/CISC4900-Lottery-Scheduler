CC = gcc
AS = as
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Final Product 
casino.bin: boot.o kernel.o string.o vga.o gdt.o gdt_flush.o idt.o idt_flush.o isr_handler.o isr.o
	ld $(LDFLAGS) -o casino.bin boot.o kernel.o string.o vga.o gdt.o gdt_flush.o idt.o idt_flush.o isr_handler.o isr.o

# Assembly Objects
boot.o: boot.s
	$(AS) $(ASFLAGS) boot.s -o boot.o

gdt_flush.o: gdt_flush.s
	$(AS) $(ASFLAGS) gdt_flush.s -o gdt_flush.o

idt_flush.o: idt_flush.s
	$(AS) $(ASFLAGS) idt_flush.s -o idt_flush.o

isr_handler.o: isr_handler.s
	$(AS) $(ASFLAGS) isr_handler.s -o isr_handler.o

# C Objects
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

gdt.o: gdt.c
	$(CC) $(CFLAGS) -c gdt.c -o gdt.o

idt.o: idt.c
	$(CC) $(CFLAGS) -c idt.c -o idt.o

isr.o: isr.c
	$(CC) $(CFLAGS) -c isr.c -o isr.o

# String Library Object
string.o: string.c
	$(CC) $(CFLAGS) -c string.c -o string.o

# VGA Library Object
vga.o: vga.c
	$(CC) $(CFLAGS) -c vga.c -o vga.o

# Emulator Target
run: casino.bin 
	qemu-system-i386 -kernel casino.bin

# Cleaup Utility
clean: 
	rm -f *.o *.bin