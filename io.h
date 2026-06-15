#ifndef IO_H
#define IO_H

#include <stdint.h>

/* outb: Writes a byte to a port
 * port = port address (In x86 architecture, I/O port address is 16 bits)
 * val = byte
 * Place val into eax.
 * Port number is either a one byte constant or if bigger than a byte, load into dx.
 */
static inline void outb(uint16_t port, uint8_t val){
  __asm__ volatile("outb %0, %1"
		   :
		   : "a"(val), "Nd"(port)
		   : "memory"
		 );
}

/* inb: Reads a byte from a port
 * val = byte
 * port = port address
 * Val placed in al (low byte of eax)
 */
static inline uint8_t inb(uint16_t port){
  uint8_t val;
  __asm__ volatile("inb %1, %0"
		   : "=a"(val)
		   : "Nd"(port)
		   : "memory"
		 );
  return val;
}

#endif
