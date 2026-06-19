#include "isr.h"
#include "vga.h"

// Array for all our handler functions.
isr_t interrupt_handlers[256];

// To use when creating more interrupts for things like keyboard input.
void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

// The 32 Intel reserved exception messages
char *exception_messages[] = {
    "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds", "Invalid Opcode", "No Coprocessor",
    "Double Fault", "Coprocessor Segment Overrun", "Bad TSS", "Segment Not Present",
    "Stack Fault", "General Protection Fault", "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
    "Virtualization Exception", "Control Protection Exception", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Hypervisor Injection Exception",
    "VMM Communication Exception", "Security Exception", "Reserved"
};

// Master handler function.
void isr_dispatch(registers_t *r) {

    // If interrupt is a standard Intel hardware exception (0-31)
    if(r->int_no < 32) {
        terminal_setcolor(4);
        terminal_writestring("SYSTEM PANIC:");
        terminal_writestring(exception_messages[r->int_no]);
        terminal_writestring("\n");
        
        // Freeze the machine.
        while(1);
    }

    // Run other interrupts
    if(interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}