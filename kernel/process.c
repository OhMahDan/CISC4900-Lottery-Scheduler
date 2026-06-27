#include "process.h"
#include "tss.h"
#include "syscall.h"
#include "pit.h"

#define NUM_OF_PROCESSES 2
#define KERNEL_STACK_SIZE 4096      // 4KB kernel stack size
#define USER_STACK_BASE 0x00B00000  // Start of user space stack
#define USER_STACK_SIZE 0x00100000  // Size of each process's user space stack (1MB)

extern void swtch(uint32_t *old_esp, uint32_t new_esp);
extern void trapret(void);

// Allocating the process table and Ring 0 stacks
pcb_t process_table[NUM_OF_PROCESSES];                                         
__attribute__((aligned(4))) uint8_t kernel_stacks[NUM_OF_PROCESSES][KERNEL_STACK_SIZE];


static uint32_t next_pid = 0;
uint32_t current_pid = 0;
static uint32_t seed = 0;

// Function for creating user processes. This will construct the stack frame for the process for the scheduler to execute.
void create_process(void (*entry)(void), uint32_t tickets){
    
    // Add process PCB to table and initialize.
    pcb_t *p = &process_table[next_pid];

    p->pid = next_pid;
    p->tickets = tickets;
    p->state = PROCESS_READY;

    // Allocating the kernel and user space stacks
    uint32_t kstack = (uint32_t)kernel_stacks[next_pid] + KERNEL_STACK_SIZE;
    uint32_t ustack = USER_STACK_BASE - next_pid * USER_STACK_SIZE;
    p->kernel_stack = kstack;           // For tss_set_stack

    // Building the iret frame.
    uint32_t *sp = (uint32_t *)kstack;
    *(--sp) = 0x23;                     // ss - ring 3 data segment
    *(--sp) = ustack;                   // esp - user stack pointer
    *(--sp) = 0x202;                    // eflags - bit 1 reserved + IF (0x200)
    *(--sp) = 0x1B;                     // cs - ring 3 code segment
    *(--sp) = (uint32_t)entry;          // eip - function pointer we want to run

    // Switch frame.
    *(--sp) = (uint32_t)trapret;        // Function pointer to iret entry code.
    *(--sp) = 0;                        // ebp
    *(--sp) = 0;                        // ebx
    *(--sp) = 0;                        // esi
    *(--sp) = 0;                        // edi

    // Point PCB.esp to the bottom of our frames in the stack.
    p->esp = (uint32_t)sp;

    next_pid++;

}

// We need a function to run our very first process, as there is no old process to switch from yet.
void init_scheduler(void){
    uint32_t dummy_old;
    pcb_t *first = &process_table[0];
    current_pid = 0;
    tss_set_stack(first->kernel_stack);
    swtch(&dummy_old, first->esp);
}

/* PRNG: Linear Congruential Generator -- x + 1 = (a * x + c) mod m, where x + 1 is in range [0, m - 1]
 * seed = ticks from PIT
 * Sources:
   glibc/POSIX rand() manual
 * Mod result with number of distributed tickets
 */
uint32_t rand(){
  if(seed == 0)
    seed = ticks;
  seed = seed * 1103515245 + 12345;
  // Discard lower 16 bits. POSIX does /65536 (2^16) which is equivalent to >> 16.
  // Classic LCG problem: low bits of LCG outputs are poor quality.
  return (seed >> 16);
}

// The main context switching function.
void schedule(void) {

    // Get the old and new process PCBs.
    pcb_t *old = &process_table[current_pid];
    current_pid = (current_pid + 1) % NUM_OF_PROCESSES;
    pcb_t *new = &process_table[current_pid];

    // Set the new TSS
    tss_set_stack(new->kernel_stack);

    // Switch into the new process
    swtch(&old->esp, new->esp);
}
