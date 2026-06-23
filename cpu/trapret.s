.global trapret

trapret:
    mov $0x23, %ax      # ring 3 data selector
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    iret