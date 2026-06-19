#include "syscall.h"

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

int readline(char *buf, int size){
  int n = 0;
  while(n < size - 1){
    char c = getchar();
    putchar(c);
    if(c == '\n') break;
    buf[n++] = c;
  }
  buf[n] = '\0';
  return n;
}
