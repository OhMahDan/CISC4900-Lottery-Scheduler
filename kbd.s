	.global interrupt_kbd_handler

interrupt_kbd_handler:
	pusha
	call interrupt_kbd_handler_c
	popa
	iret
