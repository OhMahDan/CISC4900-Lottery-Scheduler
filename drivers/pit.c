#include "pit.h"
#include "io.h"
//REMOVE when scheduler is implemented
#include "vga.h"
#include "process.h"

#define PIT_DATA_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define BASE_FREQUENCY 1193182
#define CHANNEL0_COMMAND 0x36
#define EOI 0x20
#define PIC1_COMMAND 0x20

// Seed for random number generator
uint32_t ticks = 0;

/* pit_init: Sets the tick frequency for the OS timer (each tick, sends a signal down IRQ0).
 * divisor = the desired frequency.
 * PIT data port is 8-bit:
   Bits 7 and 6: Usage -- 00 = channel 0 (directly connected to IRQ0 on Master PIC)
   Bits 5 and 4: Access mode -- 11 = lobyte/hibyte
   Bits 3 to 1: Operating mode -- 0 1 1 = square wave generator, fires repeatedly. Ideal for timer.
   Bit 0: BCD/Binary mode -- 0 = 16-bit binary
 * The PIT counter is 16 bits but the data port is only 8 bits, so we send one byte
   at a time (low byte then high byte).
 * Sources:
   http://www.osdever.net/bkerndev/Docs/pit.htm
   https://wiki.osdev.org/Programmable_Interval_Timer
 */
void PIT_init(uint32_t frequency){
  uint16_t divisor = BASE_FREQUENCY / frequency;
  outb(PIT_COMMAND_PORT, CHANNEL0_COMMAND);
  outb(PIT_DATA_PORT, (uint8_t)divisor);
  outb(PIT_DATA_PORT, (uint8_t)(divisor >> 8));
}

/* pit_handler: function that gets called at every tick.
 * Calls lottery scheduler.
 */
void PIT_handler_c(registers_t *r){
  // REMOVE when scheduler is implemented
  ticks++;
  if(ticks % 100 == 0)
    terminal_writestring("1 sec passed.\n");
  //schedule();
  outb(PIC1_COMMAND, EOI);
}
