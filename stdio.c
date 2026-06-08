//#include "syscall.h"
// REMOVE WHEN SYSCALL.H IS CREATED AND PUT BACK INCLUDE SYSCALL.H
#define SYS_PUTCHAR 1
#define SYS_GETCHAR 2

/* putchar
 * c = character being put in memory
 * Move syscall num for putchar into eax and c into ebx
   "a"(SYS_PUTCHAR) == mov SYS_PUTCHAR, %%eax. "b"(c) == mov %c %%ebx
   * Return c as int
 */
int putchar(char c){
  __asm__ volatile (
		    "int $0x80"
		    : 
		    : "a"(SYS_PUTCHAR), "b"(c)
		    : "memory"
		   );
  return c;
}

/* getchar
 * c = character being read from memory
 * Move syscall num for getchar into eax
 * Write from eax and store into c
 * Return c
 */
char getchar(){
  char c;
  __asm__ volatile(
		   "int $0x80"
		   : "=a"(c)
		   : "a"(SYS_GETCHAR)
		   : "memory"
		   );
  return c;
}
