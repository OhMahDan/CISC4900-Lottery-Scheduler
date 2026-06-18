#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

// Function Pointer called isr_t, returns void and accepts registers_t as a parameter.
typedef void (*isr_t)(registers_t*);

void fault_handler(registers_t *r);
void register_interrupt_handler(uint8_t n, isr_t handler);

#endif