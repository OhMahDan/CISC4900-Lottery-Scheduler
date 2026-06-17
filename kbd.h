#ifndef KBD_H
#define KBD_H

// Hooked to IDT entry 33 (IRQ1)
void interrupt_kbd_handler();
// Reads a char from keyboard buffer
char getchar();

#endif
