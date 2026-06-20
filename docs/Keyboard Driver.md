## Overview
**x86 Hardware Interrupt Handling:**
- **Hardware Interrupts:** Hardware interrupts are physical signals sent from an external device to the CPU. The signals are sent over dedicated interrupt request lines (IRQ lines). The CPU pauses execution and looks up the associated interrupt number from the PIC in the IDT (Interrupt Descriptor Table). Then, jumps to an address to execute the ISR (Interrupt Service Routine). After execution, the CPU returns to its task before the interrupt. Examples of hardware interrupts include: keyboard input, mouse movement/clicks, disk drive I/O, hardware timers, and power/system failures.
There are 2 types of hardware interrupts:
	1. Maskable (INTR): The CPU can choose to ignore or defer these interrupts by clearing the interrupt flag.
	2. Non-maskable (NMI): High priority interrupts. The CPU cannot ignore or disable these because they bypass the interrupt flag entirely. They are hardwired to always be processed. These are reserved for critical, emergency-level hardware events (e.g. hardware failures).

- **Interrupt Flag:** The interrupt flag is a bit in the CPU's ELFAGS register that controls whether the CPU responds to maskable hardware interrupts. The Global Interrupt Flag (IF) controls whether interrupts are enabled or disabled. 1 = interrupts are enabled. CPU responds to IRQ signals. 0 = interrupts are disabled. CPU ignores IRQ signals. In Casino OS, sti (set interrupt flag) enables interrupts after the IDT and PIC are initialized. cli (clear interrupt flag) disables interrupts insides interrupt handlers automatically.

## Keyboard Input
Keyboard interrupts are maskable interrupts. They do not directly communicate with the CPU. The pipeline from key stroke to CPU: PS/2 Keyboard -> 8042 Keyboard Controller -> 8259 PIC -> CPU.

1. **PS/2 Keyboard:** PS/2 is a synchronous serial interface standard for connecting keyboards to a computer. The PS/2 keyboard is the device used for user input. Whenever a key is pressed and released, the PS/2 keyboard generates the scan code associated with the key. There are 2 types of scan codes:
	-**Make code:** Key is pressed.
	-**Break code:** Key is released. The break code is the make code with bit 7 set. E.g. 'a' make code = 0x1E, break code = 0x9E.

2. **8042 Keyboard Controller:** The keyboard controller is a chip on the motherboard. It receives the scan code from the keyboard and buffers it into a small register. The data port for the keyboard controller is 0x60, this is where the scan code is read from. The status/command port is 0x64.

3. **8259 PIC:** The 8259 Programmable Interrupt Controller (PIC) chip is a core part of the x86 architecture. Its main responsibility is to manage hardware interrupts and direct them to the appropriate system interrupts. It also handles interrupt priority. I.e. it controls the CPU's interrupt mechanism. The PIC accepts interrupt requests and feeds them to the processor. Whenever a key is pressed on the keyboard, a pulse is sent along the interrupt request line (IRQ 1) to the PIC. The PIC translates IRQ into a system interrupt, and sends the interrupt to the CPU. Each PIC has 8 IRQ lines. In the x86 architecture, there are 2 PICs:
	-**Master PIC:** IRQ 0-7 maps to interrupt 8-15 in real mode.
	-**Slave PIC:** IRQ 8-15 maps to interrupt 16-23 in real mode.

4. **CPU:** The CPU receives the interrupt number from the PIC and looks it up in the IDT. It saves the current execution state onto the stack (EIP, CS, EFLAGS). It clears the interrupt flag  preventing other maskable interrupts from firing during handler execution. Then, it jumps to the ISR stored in the IDT entry. Before the ISR finishes execution, it sends an EOI (End of Interrupt) signal to the PIC  to allow future interrupts to come through. Then, it executes iret, which pops the saved state off the stack. It resumes execution before the interrupt.

## Software Implementation
1. io.h: Port I/O primitives. x86 hardware devices communicate through dedicated I/O ports. Ports are 16-bit. inb and outb are written in inline assembly. They are both declared static inline to reduce overhead.
	-inb(uint16_t port): Reads a byte from a port using x86 in instruction.
	-outb(uint16_t port, uint8_t val): Writes a byte to a port using x86 out instruction.

2. pic.c: Remaps PIC.
The problem: Interrupt 0-31 is reserved for CPU exceptions.
The solution: Remapping the PIC. Master PIC IRQ 0-7 remaps to Int 32-39. Slave PIC IRQ 8-15 remaps to Int 40-47.
	-PIC_remap(): Reconfigures two PIC chips (Master and Slave). 8259 PIC has a 4 step initialization sequence:
		1. ICW1: "Start initialization". Sent to the PIC command port with the value 0x11 (initialization flag + ICW4 needed flag). Lets the PIC know there are 3 more commands on the way.
		2. ICW2: Vector offset. Sent to the data port. Lets the PIC know it's new interrupt numbers. Set the Master PIC offset to 0x20 and the Slave PIC offset to 0x28.
		3. ICW3: Cascade identity. Sent to the data port. Lets the PICs know how its connected to each other. Master PIC is told slave is on IRQ2 (0x04), and slave is told its cascade identity is 2(0x02).
		4. ICW4: Environment. Sent to the data port. Lets PIC know what "environment" it's running in. For Casino OS, it will be 8086 mode.

After initialization, all IRQs are masked except IRQ1 (keyboard) and IRQ0 (timer) since those are the only two we need for Casino OS. Leaving other interrupts that have not been implemented open could lead to system crashes. Masking is done by writing to the PIC data port. Each bit in the mask byte corresponds to an IRQ line, 0 = unmasked, 1 = masked.

	-io_wait(): Implements a small delay (~1-4 micro seconds). This is called between each PIC command. In older systems, PIC needs time to process commands before another command can be sent. Implemented as outb(0x80, 0), no-op wastes time.

3. kbd.c: Interrupt handler. The C handler called by the assembly wrapper at every interrupt 33 (IRQ1) call.
	- scan_code_table: Maps scan code to ASCII characters. Implemented as a 2D array, first row is unshifted characters, second row is shifted characters. The index corresponds directly to the scan code value.
	-interrupt_kbd_handler_c(): Reads a byte from the keyboard port and stores it into a buffer. We need to filter out break codes. Every time a key is pressed, we get two scan codes (make and break code). Since the break code is the make code with bit 7 set, we use a mask 0x80 to isolate bit 7 so we can differentiate between make and break code. We also need to account for shift key presses. Read_pos and write_pos are used to read and write from buffer. They are declared volatile to prevent compiler caching so they will always be read from memory.
	-kbd_getchar(): Reads from buffer. Called by the user space getchar().

Note: there is an assembly wrapper (isr_handler.s) that the CPU jumps to save general purpose and segment registers. It calls on isr_dispatch() to look up the registered handler and calls it (interrupt_kbd_handler_c()). After the handler returns, it restores the registers and returns from the interrupt with iret. C functions use a regular ret instruction which does not restore EFLAGS and CS.

**Keyboard Interrupt Handler Flow:**
1. Key is pressed. PIC sends signal along IRQ1 line and translates it to interrupt 33.
2. CPU looks up IDT entry 33 and jumps to isr_handler.s
3. General purpose and segment registers are saved. Calls on isr_dispatch(), which then calls on interrupt_kbd_handler_c().
4. Scan code is read and translated from the scan_code_table. Break codes are filtered. Shift is  handled.
5. Char is stored in a buffer.
6. EOI (end of interrupt) is sent to PIC via outb.
7. Registers are restored and we return from interrupt with iret.

