#include "gdt.h"
#include "tss.h"

// The GDT table itself. 
// We need 6 entries (Null, Kernel Code, Kernel Data, User Code, User Data, and a TSS).
gdt_entry_struct gdt_entries[6];
gdt_ptr_struct gdt_ptr;  // Pointer to our GDT
tss_entry_struct tss;     

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

static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0) {
    
    // Zero out the TSS memory
    uint8_t *tss_mem = (uint8_t *)&tss;
    for (int i = 0; i < sizeof(tss_entry_struct); i++) {
        tss_mem[i] = 0;
    }

    // Set the Ring 0 stack segment and pointer
    tss.ss0 = ss0;
    tss.esp0 = esp0;
    
    // Prevent Ring 3 programs from directly talking to hardware ports
    tss.iomap_base = sizeof(tss_entry_struct);

    // Attach the TSS to the GDT at Entry 'num'
    uint32_t tss_base = (uint32_t)&tss;
    uint32_t tss_limit = sizeof(tss_entry_struct) - 1;
    
    // access = 0x89 (Present, Ring 0, Available 32-bit TSS)
    // gran = 0x00 (Byte granularity)
    gdt_set_gate(num, tss_base, tss_limit, 0x89, 0x00);
}

void init_gdt() {
    
    // Configuring the GDT pointer.
    gdt_ptr.limit = (sizeof(gdt_entry_struct) * 6) - 1;
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

    // Entry 5: TSS
    write_tss(5, 0x10, 0);

    // Give the GDT ptr to the CPU using Assembly.
    gdt_flush((uint32_t)&gdt_ptr);

    // Load the Task Register
    tss_flush();
}

// This will be called by our scheduler during every context switch
void tss_set_stack(uint32_t kernel_esp) {
    tss.esp0 = kernel_esp;
}