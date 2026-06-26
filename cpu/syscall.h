#ifndef SYSCALL_H
#define SYSCALL_H

#include "isr.h"

// Syscall number definitions
#define SYS_PUTCHAR 1
#define SYS_GETCHAR 2
#define SYS_YIELD 158

// Syscall dispatcher
void syscall_handler_c(registers_t *r);

#endif
