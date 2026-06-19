#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "syscall.h"
#include "pic.h"
#include "tss.h"
#include "io.h"

extern void calc_main();
extern void launch_calculator(void *);

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

    // Drop into Ring 3
    // Set the TSS Ring 0 stack to our kernel stack so system calls dont crash.
    // This will not be necessary when we fully implement our PCBs and process.c
    uint32_t current_stack;
    __asm__ volatile ("mov %%esp, %0" : "=r"(current_stack));
    tss_set_stack(current_stack);

    // Execute.
    launch_calculator(calc_main);

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}
