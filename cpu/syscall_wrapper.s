	.global syscall_handler
	.align 4

syscall_handler:
	pusha			# push gp registers onto stack
	mov %esp, %eax		# copy stack ptr value to eax
	push %eax		# pass eax as arg for syscall_handler_c
	call syscall_handler_c
	pop %eax		# clean up stack
	popa			# restore gp registers
	iret			# return from interrupt
