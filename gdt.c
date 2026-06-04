#include "gdt.h"

// The GDT table itself. 
// We need 5 entries (Null, Kernel Code, Kernel Data, User Code, and User Data).
gdt_entry_struct gdt_entries[5];

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {

    // Base Address
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    // Limits
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    // Granularity
    gdt_entries[num].granularity |= (gran & 0xF0);

    // Access flags
    gdt_entries[num].access = access;
}

extern void gdt_flush(uint32_t);    // Referencing a function written in assembly code.
gdt_ptr_struct gdt_ptr;  // Pointer to our GDT

void init_gdt() {
    
    // Configuring the GDT pointer.
    gdt_ptr.limit = (sizeof(gdt_entry_struct) * 5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // Inputing entries into the GDT
    // Entry 0: Null (Intel hardware requirement to catch null-pointers)
    gdt_set_gate(0, 0, 0, 0, 0);

    // Entry 1: Kernel Code
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entry 2: Kernel Data
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Entry 3: User Code
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // Entry 4: User Data
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // Give the pointer to the CPU using Assembly.
    gdt_flush((uint32_t)&gdt_ptr);
}