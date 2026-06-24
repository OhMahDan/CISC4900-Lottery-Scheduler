#include "syscall.h"
#include "kbd.h"
#include "vga.h"
#include "isr.h"
#include "process.h"

static void sys_putchar(char c);
static uint8_t sys_getchar();


/* syscall_handler_c: Reads syscall number from eax and dispatches to
   appropriate syscall function.
 * ebx returns uint32_t so we need to type cast to char since terminal_putchar
   takes a char.
 */
void syscall_handler_c(registers_t *regs){
  switch(regs->eax){
  case SYS_PUTCHAR:
    sys_putchar((char)regs->ebx);
    break;
  case SYS_GETCHAR:
    regs->eax = sys_getchar();
    break;
  case SYS_YIELD:
    schedule();
    break;
  }
}

/* sys_putchar: Puts a character on the screen
 * Calls on VGA function terminal_putchar to write char to VGA text buffer
   and display on screen.
 */
static void sys_putchar(char c){
  terminal_putchar(c);
}

/* sys_getchar: Reads a character from the keyboard buffer
 * Calls on kbd getchar to read a character from input buffer
 */
static uint8_t sys_getchar(){
  return kbd_getchar();
}
