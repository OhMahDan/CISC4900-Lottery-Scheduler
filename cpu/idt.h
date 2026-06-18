#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Struct for an IDT entry
typedef struct {
    uint16_t base_low;          // Lower 16 bits of the ISR.
    uint16_t sel;               // GDT segment selector
    uint8_t zero;               // Unused bits, set to 0. Required by Intel hardware.
    uint8_t flags;              // Attributes: Gate type, privilege, and present bit.
    uint16_t base_high;         // Upper 16 bits of the ISR.
} __attribute__((packed)) idt_entry_t;

// Struct for the IDT pointer
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

void init_idt();

#endif