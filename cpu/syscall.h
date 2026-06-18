#ifndef SYSCALL_H
#define SYSCALL_H

// Syscall number definitions
#define SYS_PUTCHAR 1
#define SYS_GETCHAR 2

// Assembly wrapper
void syscall_handler();

#endif
