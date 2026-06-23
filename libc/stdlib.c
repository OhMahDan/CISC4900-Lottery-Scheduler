#include "syscall.h"

void yield(void) {
    asm volatile ("int $0x80" : : "a"(SYS_YIELD));
}