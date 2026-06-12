# Overview
ISRs are what populate our IDT table. When an interrupt request (IRQ) is received, the CPU pauses the current process and checks the IDT for the specific interrupt number requested where it will find the corresponding ISR. The CPU must save its current state and jump to the ISR function and execute it. The ISR finishes with a special return instruction, which restores the CPU's state so the interrupted program can continue where it left off. Because ISRs switch privilege level (e.g., going from Ring 3 to Ring 0), they must be written in assembly carefully. Luckily, Intel already includes 32 interrupt handlers for us to use for events like divide by zero, double faults, and page faults. All we need to do is wire everything together.
# The Basic ISR Structure
The basic handler workflow for all of our interrupts will look something like this:
```
.global isr
isr:
	
	# Disable further interrupts to prevent another interrupt from firing until this one completes
	cli
	
	# Save the current process's registers in RAM
	pusha
	
	# Call the fault hander, written in C
	call fault_handler
	
	# Restore the registers after the fault handler completes
	popa
	
	# Interrupt return, a special Intel hardware instruction
	iret
```
Certain interrupts push a 4-byte error code to the stack before jumping to the ISR. When coding up an ISR wrapper, we have to be careful to take it into account when aligning the stack. Otherwise, using `iret` could result in a triple-fault. One solution to this is pushing a dummy error code for non-error interrupts so that the stack is always 8 bytes regardless and can be aligned with a single instruction in both scenarios.