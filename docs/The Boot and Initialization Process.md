The boot process of our OS is essentially a relay race. The CPU powers on completely blind and severely limited in capabilities. Each component we build is a slightly more capable layer, passing the baton until our kernel is in total control.
# 1. Power Delivery
You press the power button. The power supply sends a signal to the motherboard called `POWERGOOD`. The CPU resets and wakes up in **16-bit Real Mode.** This is a legacy state, backward-compatibility feature that mimics a 1978 Intel 8086 processor. It has access to exactly 1MB of RAM, zero security features, no operating system, no memory manager, and no concept of C code. The CPU is hardwired to look at exactly one physical memory address on startup: `0xFFFFFFF0`. This address does not map to RAM, but directly to the ROM chip containing the BIOS (or the UEFI in modern systems).
# 2. The Basic Input/Output System
The BIOS checks to make sure all necessary hardware is present (RAM, Graphics). It also searches storage drives (USB, HDD, SSD) for a specific 2-byte signature (`0xAA55`) at the end of the very first sector. When it finds this signature, it loads that 512-byte sector into RAM at address `0x7C00` and instructs the CPU to jump there. This is where our bootloader (Grand Unified Bootloader, A.K.A. GRUB) lives.
# 3. The Bootloader
512 bytes is too small to fit a modern OS. Thus, GRUB uses a multi-stage process to read our filesystem to find our compiled `casino.bin` file. GRUB does the heavy lifting of building a modern CPU environment before our code runs:
1. Enables memory access beyond 1MB through the A20 line, a legacy hardware gate.
2. Sets up a temporary, rudimentary Global Descriptor Table.
3. Flips the Protection Enable bit in the CPU's `CR0` register, officially transitioning the hardware from 16-bit Real Mode into **32-bit Protected Mode**.
4. It reads the Multiboot Header at the top of our `boot.s` file to ensure our kernel is compliant.
5. It loads our kernel into RAM (usually at the 1MB mark) and jumps to the `_start` label in our `boot.s` file
# 4. Kernel Entry
Our OS is running, but we have no stack initialized. Without a stack, we don't have a C environment to call functions in.
1. `boot.s` explicitly reserves a block of uninitialized memory (e.g., 16KB) in the BSS (Block Started by Symbol) section, a memory segment that stores uninitialized global and static variables.
2. It points the CPU's Stack Pointer register (`esp`) to the very top of this memory block. This is the **Boot Stack**.
3. With a valid stack now existing, `boot.s` executes `call kernel_main`, bridging the gap into C code.
# 5. Memory Initialization
Our kernel is currently running on GRUB's temporary GDT. We must now begin to build our own permanent memory rulebook to establish our flat memory model.
1. We allocate an array of 6 GDT entries (Null, Kernel Code, Kernel Data, User Code, User Data, TSS).
2. We define the base addresses (`0x0`) and limits (4GB) for each, differentiating them only by their Ring 0 and Ring 3 access bytes.
3. We load the pointer to this array into the CPU using the `lgdt` instruction.
4. We execute a far jump to flush the CPU's internal segment caches and force it to adopt our new GDT. The hardware privilege boundaries now exist.
The GDT will also hold the routines for when Ring 3 user programs get interrupted while running:
5. We initialize a 104-byte TSS entry.
6. Set its `ss0` field to our Ring 0 Data Segment (`0x10`) and attach the TSS to Entry 5 of our GDT.
7. We execute the `ltr` instruction. The CPU now knows exactly where to look for a secure kernel stack pointer (`esp0`) anytime a Ring 3 program causes an exception or hardware interrupt.
## Structures Built: The Global Descriptor Table & Task State Segment
Every single time the CPU accesses a memory address from this point forward, it cross-references the GDT table in hardware to ensure the process has the correct privileges.
When an interrupt fires during a User Space program, the CPU hardware autonomously reads this structure, finds `ss0` and `esp0`, and swaps the stack pointers before executing our interrupt handler.
# 6. Interrupt Routing
With memory built, we must now tell the CPU how to handle processes and errors.
1. We allocate an array of 256 IDT entries.
2. We map the first 32 entries to our internal CPU exception wrappers (Divide by Zero, Page Fault, etc.)
3. The next 16 entries require remapping the Programmable Interrupt Controller. This chip defaults to sending IRQs on vectors 8-15, overlapping with Intel's dedicated CPU exceptions we just mapped. We utilize `outb` to send Initialization Command Words (ICWs) to the PIC, mapping it to entries 32-47.
4. Load the table into the CPU using `lidt`.
5. We send the `0xFC` and `0xFF` bitmasks to the PIC to physically mask out every hardware device except the Timer and the Keyboard.
6. We execute the `sti` (set interrupt flag) instruction, officially allowing the CPU to perform interrupts.
## Structures Built: Interrupt Descriptor Table
Every time an interrupt fires, the CPU pushes its state, looks at the IDT, jumps to the Interrupt Service Routine, and our C handler reads the struct to see exactly what happened.
