	.global interrupt_kbd_handler
	.align 4

interrupt_kbd_handler:
	pusha				# Saves all gp registers to stack
	call interrupt_kbd_handler_c	# Call C interrupt handler
	popa				# Restore gp registers
	iret				# Return from interrupt
