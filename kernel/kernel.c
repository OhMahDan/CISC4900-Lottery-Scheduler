#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "../user/prime.h"

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

    // Start Prime Calculations
    calculate_primes();

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}