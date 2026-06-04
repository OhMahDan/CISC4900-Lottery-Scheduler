#include "vga.h"
#include "gdt.h"

void kernel_main(void) {

    // Call terminal_clear
    terminal_clear();

    // Test the GDT
    terminal_writestring("Loading GDT...\n");
    init_gdt();
    terminal_writestring("GDT Loaded Successfully!\n");

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}