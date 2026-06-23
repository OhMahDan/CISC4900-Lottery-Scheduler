CC = gcc
AS = as
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -fno-builtin -Icpu -Idrivers -Ilibc -Iuser -Ikernel
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

OBJS = boot/boot.o \
		cpu/gdt.o cpu/gdt_flush.o cpu/idt.o cpu/idt_flush.o cpu/isr_handler.o cpu/isr.o cpu/tss_flush.o cpu/syscall.o cpu/trapret.o cpu/swtch.o \
		drivers/kbd.o drivers/pic.o drivers/vga.o \
       	kernel/kernel.o kernel/process.o \
    	libc/string.o libc/stdio.o libc/stdlib.o \
		user/printAB.o
       

casino.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o casino.bin $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

run: casino.bin 
	qemu-system-i386 -kernel casino.bin

clean:
	rm -f $(OBJS) casino.bin