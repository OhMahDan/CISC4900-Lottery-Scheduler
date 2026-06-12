# Overview
Interrupts and exceptions are mechanisms that indicate to the CPU it must pause what it is currently doing and respond to an event occurring somewhere in the system. Each unique type of event (say 'divide-by-zero', trying to access restricted memory, or pressing a key on your keyboard) requires its own defined exception and handler, called an **Interrupt Service Routine.** Each ISR is assigned a unique number that indexes it into an **Interrupt Descriptor Table.** Similar to the GDT, the IDT itself resides in memory, and the processor uses an `IDTR` register to hold the location and size of the IDT. Per Intel's x86 specifications, the array itself is exactly 256 entries at all times that contain routing rules that say "If Event X happens, jump to Memory Address Y and execute the code there." Entries may be empty, but the array must be 256 entries regardless.
# IDT Structure
An IDT entry describes a destination, thus it only needs two primary pieces of information for its pointer:
1. **The Offset:** The physical 32-bit memory address of our Assembly ISR wrapper.
2. **The Segment Selector:** The 16-bit offset into the GDT (Always `0x08` for Kernel Code)
The awkwardness of Intel's 32-bit architecture continues here as well, as our offset will be split in two and located on opposite ends of the entry.
## Building the IDT Entry
1. **Offset Low: Bits 0-15.** The lower 16 bits of the memory address where the ISR lives.
2. **Segment Selector: Bits 16-31**. The GDT offset. For us, always `0x08`.
3. **Reserved: Bits 32-39.** Intel architecture mandates this byte must always be zeroed out.
4. **Type and Attributes: Bits 40-47.** The security and gate configuration, to be calculated.
5. **Offset High: Bits 48-63.** The upper 16 bits of the memory address of the ISR.
## Calculating the Attribute Byte
Just like the GDT, we have to calculate a "magic number" for the Attribute byte:
- **Bit 0-3: Gate Type.** The only value we will use is the one for 32-bit interrupt gates: `1110`.
- **Bit 4: Storage.** Always `0` for interrupt gates.
- **Bit 5-6: DPL** or privilege level. `00` for Ring 0, `11` for Ring 3.
- **Bit 7: Present.** Is this a valid ISR? Always `1`.
Following these rules, all of our standard hardware exceptions (Divide by Zero, Page Fault, Keyboard Press) will use the same attribute byte: `0x8E`. We will use `0xEE` for user space system calls.
## Installing the IDT
Installing the IDT is a simple task, only requiring us to load the IDT pointer into the `IDTR` register and returning. While structurally complete, the IDT is not functional yet as it needs to be populated with ISRs before it will be useful.