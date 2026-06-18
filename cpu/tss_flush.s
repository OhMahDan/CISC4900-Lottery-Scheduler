.global tss_flush

tss_flush:
    movw $0x28, %ax     # Move the 16-bit offset into the AX register
    ltr %ax             # Load the Task Register
    ret                 # Return to the calling C function