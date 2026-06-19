.global launch_calculator

launch_calculator:
    # Fetch the function pointer (calc_main) passed from C
    mov 4(%esp), %ebx      

    # Load Ring 3 Data Segment (0x23) into segment registers
    mov $0x23, %ax         
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # Construct the fake IRET stack frame
    push $0x23             # SS (Ring 3 Data Segment)
    push $0xB00000         # ESP (Arbitrary safe memory address for the user stack)
    
    pushf                  # Push EFLAGS
    pop %eax               
    or $0x200, %eax        # Turn on the IF bit (Interrupts Enabled) so the keyboard works
    push %eax              # Push modified EFLAGS back onto the stack
    
    push $0x1B             # CS (Ring 3 Code Segment)
    push %ebx              # EIP (The calc_main function pointer)

    # Execute the drop
    iret