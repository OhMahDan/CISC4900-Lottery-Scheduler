#include "process.h"

// Allocating the process table and Ring 0 stacks
pcb_t process_table[1];                                         // Currently only need 1 for the single process we will be running.
__attribute__((aligned(4))) uint8_t kernel_stacks[1][4096];     // Kernel Stacks of 4KB size for each process

