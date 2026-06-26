#ifndef KBD_H
#define KBD_H

#include "isr.h"

// Hooked to IDT entry 33 (IRQ1). Called by assembly
void interrupt_kbd_handler_c(registers_t *r);
// Reads a char from keyboard buffer
char kbd_getchar();

#endif
