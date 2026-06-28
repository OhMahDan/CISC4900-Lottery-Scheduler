#include "process.h"
#include "tss.h"
#include "syscall.h"
#include "pit.h"
#include "vga.h"
#include "stdlib.h"

#define NUM_OF_PROCESSES 2
#define KERNEL_STACK_SIZE 4096      // 4KB kernel stack size
#define USER_STACK_BASE 0x00B00000  // Start of user space stack
#define USER_STACK_SIZE 0x00100000  // Size of each process's user space stack (1MB)

#define TEST_DRAWS 5000             // How many draws before we report
static uint32_t run_counts[NUM_OF_PROCESSES] = {0};
static uint32_t total_draws = 0;

extern void swtch(uint32_t *old_esp, uint32_t new_esp);
extern void trapret(void);

// Allocating the process table and Ring 0 stacks
pcb_t process_table[NUM_OF_PROCESSES];                                         
__attribute__((aligned(4))) uint8_t kernel_stacks[NUM_OF_PROCESSES][KERNEL_STACK_SIZE];


static uint32_t next_pid = 0;
uint32_t current_pid = 0;
static uint32_t seed = 0;

// Scheduler validation function. Prints the tally of times a process has run, then freezes so the results stay on screen.
static void print_lottery_results() {
    char buf[12];
    terminal_writestring("\n=== LOTTERY RESULTS ===\n");
    terminal_writestring("draws: ");
    intToChar(total_draws, buf); 
    terminal_writestring(buf);
    terminal_writestring("\n");

    for (uint32_t i = 0; i < NUM_OF_PROCESSES; i++) {
        terminal_writestring("PID ");
        intToChar(i, buf);
        terminal_writestring(buf);
        terminal_writestring(": ");
        intToChar(run_counts[i], buf);
        terminal_writestring(buf);
        terminal_writestring(" wins  (");
        intToChar(process_table[i].tickets, buf);
        terminal_writestring(buf);
        terminal_writestring(" tickets)\n");
    }
}

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
    uint32_t total_tickets = 0;
    uint32_t winner = 0;
    uint32_t accumulator = 0;
    uint32_t old_pid = current_pid;

    // Lottery scheduler
    // Count number of tickets from ready processes
    for(uint32_t i = 0; i < NUM_OF_PROCESSES; i++){
      if(process_table[i].state == PROCESS_READY || process_table[i].state == PROCESS_RUNNING)
	total_tickets += process_table[i].tickets;
    }

    if(total_tickets == 0)
      return;

    // Pick random ticket winner
    winner = rand() % total_tickets;

    uint32_t chosen = old_pid;
    // Find process with winning ticket
    // Since pcb only holds ticket count and not ticket ranges, we need accumulator to assign the ticket ranges.
    for(uint32_t i = 0; i < NUM_OF_PROCESSES; i++){
      if(process_table[i].state == PROCESS_READY || process_table[i].state == PROCESS_RUNNING){
	accumulator += process_table[i].tickets;
	if(accumulator > winner){
	  chosen = i;
	  break;
	}
      }
    }

    run_counts[chosen]++;
    total_draws++;
    if (total_draws >= TEST_DRAWS) {
        print_lottery_results();
        while(1);
    }

    if (chosen == old_pid) return;
    
    pcb_t *old = &process_table[old_pid];
    pcb_t *new = &process_table[chosen];
    current_pid = chosen;

    // Set the new TSS
    tss_set_stack(new->kernel_stack);

    // Switch into the new process
    swtch(&old->esp, new->esp);
}
