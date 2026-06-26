#include "pic.h"

// Initialization flag
#define ICW1_INIT 0x10
// ICW4 needed flag
#define ICW1_ICW4 0x01
#define CASCADE_IRQ 2
#define offset1 0x20
#define offset2 0x28
#define ICW4_8086 0x01
#define MASK1 0b11111100
#define MASK2 0xFF

/* io_wait: Implements small delay (~ 1-4 microseconds)
 * Call on this in between PIC commands. PIC needs time to process commands
   before sending the next one.
 */
static void io_wait(){
  outb(0x80, 0);
}

/* PIC_remap: Reconfigures master and slave PIC so their IRQs dont conflict with
   CPU exceptions.
 * Default:
   Master PIC IRQ 0-7 maps to interrupts 8-15
   Slave PIC IRQ 8-15 maps to interrupts 16-23
 * The fix:
   Master PIC IRQ 0-7 maps to interrupts 32-39
   Slave PIC IRQ 8-15 maps to interrupts 40-47
 */
void PIC_remap(){
  // ICW1: Start initialization
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  // ICW2: Vector offset
  outb(PIC1_DATA, offset1);
  io_wait();
  outb(PIC2_DATA, offset2);
  io_wait();
  // ICW3: Cascade identity
  outb(PIC1_DATA, 1 << CASCADE_IRQ);
  io_wait();
  outb(PIC2_DATA, CASCADE_IRQ);
  io_wait();
  // ICW4: 8086 mode
  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();
  // Unmask IRQ0 (timer) and IRQ1 (keyboard)
  outb(PIC1_DATA, MASK1);
  outb(PIC2_DATA, MASK2);
}
