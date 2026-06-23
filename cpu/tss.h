#ifndef _TSS_H
#define _TSS_H

#include <stdint.h>

extern void tss_set_stack(uint32_t kernel_esp);
extern void tss_flush(void);

typedef struct {
    uint32_t prev_tss;      // Depreciated ts selector
    uint32_t esp0;          // Ring 0 stack pointer
    uint32_t ss0;           // Ring 0 stack segment selector
    // Most fields below here are predominantly unused, for ring 1 and ring 2 use.
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__ ((packed)) tss_entry_struct;

#endif