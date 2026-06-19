.section .text
.align 4

# Macro for ISRs with no error code
.macro ISR_NOERR index
    .global isr\index
    .type isr\index, @function
    isr\index:
        cli             # Disable interrupts to prevent nested panics
        push $0         # Push dummy error code for stack alignment
        push $\index    # Push the interrupt number
        jmp isr_common  # Jump to the main handler
.endm

# Macro for ISRs with an error code
.macro ISR_ERR index
    .global isr\index
    .type isr\index, @function
    isr\index:
        cli             # Disable interrupts
        push $\index    # Push interrupt number
        jmp isr_common  # Jump to main handler
.endm 

# IRQ Macro: mirrors ISR_NOERR so it lands in isr_common with a registers_t frame
.macro IRQ index, vector
    .global irq\index
    .type irq\index, @function
    irq\index:
        cli
        push $0          # Dummy error code (hardware IRQs don't push one)
        push $\vector    # The remapped vector number
        jmp isr_common
.endm

IRQ 1, 33    # keyboard
# IRQ 0, 32  # timer — for later

# int 0x80
.global isr128
.type isr128, @function
isr128:
    cli
    push $0
    push $128         
    jmp isr_common

# Routing Table for Intel Pre-built handlers.
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR 29
ISR_ERR 30
ISR_NOERR 31

.extern isr_dispatch
.type isr_dispatch, @function

isr_common:

    # Push all general-purpose registers into the stack
    pusha                   

    # Push the four Data Segment Registers to the stack
    push %ds                
    push %es
    push %fs
    push %gs

    # Overwrite the Data Segment registers to Kernel Data
    mov $0x10, %ax          
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    cld

    # Push the current value at the stack pointer and jump to it
    push %esp               
    call isr_dispatch
    add $4, %esp    # Delete the pushed value

    # Returned from the handler, begin restoring the original segment and general registers.
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    # Delete data pushed by our macros
    add $8, %esp

    # Interrupt return, resuming the interrupted program.
    iret
