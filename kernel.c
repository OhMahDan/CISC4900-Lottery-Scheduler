#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "syscall.h"
#include "pic.h"

void kernel_main(void) {

    // Call terminal_clear
    terminal_clear();

    // Test the GDT
    terminal_writestring("Loading GDT...\n");
    init_gdt();
    terminal_writestring("GDT Loaded Successfully!\n");

    // Test the IDT
    terminal_writestring("Loading IDT...\n");
    init_idt();
    terminal_writestring("IDT Loaded Successfully!\n");

    // Test PIC remap
    terminal_writestring("PIC remapping in process...\n");
    PIC_remap();
    terminal_writestring("PIC successfully mapped!\n");

    // Enable hardware interrupts
    __asm__ volatile("sti");
    terminal_writestring("Keyboard handler loaded successfully!\n");

    // Intentionally crash the kernel with a Breakpoint
    __asm__ volatile ("int $0x3");
    terminal_writestring("Breakpoint passed.\n");

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}
