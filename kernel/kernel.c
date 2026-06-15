#include "vga.h"
#include "gdt.h"
#include "idt.h"

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

    // Intentionally crash the kernel with a Breakpoint
    __asm__ volatile ("int $0x3");
    terminal_writestring("Breakpoint passed.\n");

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}