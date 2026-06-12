.section .text
.align 4

.global gdt_flush

gdt_flush:

    # Get the argument
    mov 4(%esp), %eax

    # Load the GDT pointer into the CPU register.
    lgdt (%eax)

    # Load Data Segment Registers (Flat memory model means all the same)
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %ss
    mov %ax, %gs

    # To load cs we do a "far jump"
    ljmp $0x08, $.flush
.flush:
    ret