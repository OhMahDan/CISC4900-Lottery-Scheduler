#include "idt.h"

// The IDT array must be exactly 256 entries.
idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

// Assembly function
extern void idt_flush(uint32_t);

// Set a single entry.
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    
    // Split the 32-bit base into our two 16-bit chunks.
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

    // Assign the Segment Selector
    idt_entries[num].sel = sel;

    // Set the zero byte
    idt_entries[num].zero = 0;

    // Set flags.
    idt_entries[num].flags = flags | 0x60;
}

void init_idt() {

    // Configure the IDT pointer.
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // Zero out array.
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
}