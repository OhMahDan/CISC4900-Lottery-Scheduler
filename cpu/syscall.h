#ifndef SYSCALL_H
#define SYSCALL_H

// Syscall number definitions
#define SYS_PUTCHAR 1
#define SYS_GETCHAR 2
#define SYS_YIELD 158

// Assembly wrapper
void syscall_handler_c();

#endif
