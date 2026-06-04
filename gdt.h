#ifndef GDT_H
#define GDT_H
#include <stdint.h>

// Structure for a GDT entry
typedef struct {

    // Limit
    uint16_t limit_low;

    // Segment Address (24 bit length but we need to split this into 16 & 8 bits b/c there is no uint24_t)
    uint16_t base_low;
    uint8_t base_middle;

    // Access modes
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_struct;  // To prevent GCC padding

// Defining the GDT pointer
// The lgdt assembly instruction to load the GDT into the CPU requires
// a 48-bit pointer, so we must build a custom struct for this.
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_struct;

void init_gdt();

#endif