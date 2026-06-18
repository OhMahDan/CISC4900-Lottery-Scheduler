.section .text
.align 4

.global idt_flush
.type idt_flush, @function

# void idt_flush(uint32_t pointer_address)
idt_flush:

    # Get the pointer from the stack
    mov 4(%esp), %eax

    # Load the pointer into the IDTR
    lidt (%eax)

    # Return
    ret