// Interrupt subroutine that replaces the keyboard normally used.

#include "kbd.h"
#include "io.h"
#include <stdint.h>

#define KBD_DATA_PORT 0x60
#define BUFFER_SIZE 256
#define EOI 0x20
#define MASK 0x80
#define LSHIFT_PRESS 0x2A
#define RSHIFT_PRESS 0x36
#define LSHIFT_RELEASE 0xAA
#define RSHIFT_RELEASE 0xB6
#define BACKSPACE 0x0E

// scan code to ASCII table
static char scan_code_table[2][89] = {
  {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', 
   '=', 0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
   '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 
   'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
   'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
   '3', '0', '.', 0, 0, 0, 0, 0}, 
  {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', 
   '+', 0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
   '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
  ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
  '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
  0, 0, 0, 0, 0}
};

/* shift = shift flag
 * buffer = input buffer
 * read_pos = read position in buffer
 * write_pos = write position in bufer
 */
static int shift = 0;
static char buffer[BUFFER_SIZE];
static uint8_t read_pos = 0;
static uint8_t write_pos = 0;

/* interrupt_kbd_handler: Not a conventional function. It is never explicitly called
   in a program.
   It is hooked to IDT entry 33 (IRQ 1). "When int 33 fires, jump to this address".
   Read a byte from keyboard port and put into buffer.
 */
void interrupt_kbd_handler_c(){
  uint8_t scancode;
  char c;
  scancode = inb(KBD_DATA_PORT);
  // check if break code
  if(scancode & MASK){
    if(scancode == LSHIFT_RELEASE || scancode == RSHIFT_RELEASE)
       shift = 0;
    outb(PIC1_COMMAND, EOI);
    return;
  }
  // if make code
  switch(scancode){
  case LSHIFT_PRESS:
    shift = 1;
    break;
  case RSHIFT_PRESS:
    shift = 1;
    break;
  case BACKSPACE:
    if(write_pos > 0)
      write_pos--;
    break;
  default:
    c = scan_code_table[shift][scancode];
    if(c && write_pos != read_pos - 1)
      buffer[write_pos++] = c;
  }
  // signals end of interrupt
  outb(PIC1_COMMAND, EOI);
}

// Called by user space getchar()
char getchar(){
  /* spin if buffer is empty
   * REMOVE when scheduler is implemented.
   * REPLACE with blocking.
  */
  while(read_pos == write_pos){}
  return buffer[read_pos++];
}
