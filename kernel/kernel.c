#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "syscall.h"
#include "pic.h"
#include "tss.h"
#include "io.h"
#include "process.h"

extern void printA();
extern void printB();

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

    // Drop into Ring 3 and begin running processes.
    create_process(printA, 1);
    create_process(printB, 1);
    init_scheduler();

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}
