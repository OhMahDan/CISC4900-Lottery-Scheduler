#ifndef KBD_H
#define KBD_H

// Assembly wrapper
void interrupt_kbd_handler();
// Hooked to IDT entry 33 (IRQ1). Called by assembly
void interrupt_kbd_handler_c();
// Reads a char from keyboard buffer
char getchar();

#endif
