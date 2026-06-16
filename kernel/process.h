#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>

// Process States
typedef enum {
    PROCESS_READY,   // Waiting for a lottery drawing
    PROCESS_RUNNING, // Currently occupying the CPU
    PROCESS_BLOCKED, // Waiting for I/O
    PROCESS_DEAD     // Finished, waiting to be cleaned up
} process_state_t;

// The Process Control Block
typedef struct {
    uint32_t pid;             // Process ID
    process_state_t state;    // Current execution state
    uint32_t tickets;         // Number of chances to win the CPU cycle
    uint32_t esp;             // The current stack pointer
    uint32_t kernel_stack;    // The fixed top of this process's Ring 0 stack
    //uint32_t page_directory;  // The CR3 value (if we implement paging later)
} pcb_t;

#endif