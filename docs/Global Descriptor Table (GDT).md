## Overview
Memory management on Intel's x86 architecture exists in two parts:
- **Segmentation:** A mandatory mechanism that allows for the division of our addressable memory space into smaller protected address spaces called **segments.** Different segments can enforce different rules to hold and protect various information such as executable code, data, and the stack for a program.
- **Paging:** An optional mechanism for creating a conventional demand-paged, virtual-memory system where each segment is divided into pages, which are then stored stored in memory.
When a CPU first turns on, it wakes up in **16-bit real mode**. This is an issue, as an operating system in a 16-bit environment caps our accessible memory to a measly `1MB`. To get around this, we will have to move the hardware into **32-bit protected mode**, where our accessible memory limit is bumped up to a more appealing `4GB`. We enter protected mode by building a GDT, essentially a rulebook, for the CPU to follow. While there are various designs one can implement using these mechanisms, we will make use a very minimal and simple design called a **flat memory model**.
## The Main Role of the GDT
The x86 architecture uses the GDT to enforce two specific constraints:
1. **Segmentation:** Where things will live. We will create a descriptor in a table that tells the CPU our code for program *X* starts at base address `0x2000` and has a limit (size) of `0x5000` bytes. If the program tried to access data outside of those addresses specified by the base address plus the limit, the CPU would stop the program and trigger a crash.
2. **Privilege Levels:** Who can access this information. Intel processors have physical hardware security levels (called **rings**) numbered 0 to 3. These rings help prevent accidental or even malicious attempts to alter memory in unexpected ways. Modern operating systems tend to only use two of these levels:
	1. **Ring 0**: Kernel space, which provides absolute control over the hardware 
	2. **Ring 3**: User space, the lowest privilege level that heavily restricts direct hardware access.
## The Flat Model
The simplest memory model we can build is called the **flat or linear model.** Every segment in this memory model spans the entirety of the address space, making building the GDT and subsequent translation logic extremely simple.
In this model, our GDT will essentially say:
- The Kernel Code starts at base `0x0` and has a limit of `4GB` (Our maximum RAM capacity), and it is Ring 0.
- The Kernel Data starts at Base `0x0` and has a limit of `4GB`, and it is Ring 0.
- The User Code starts at Base `0x0` and has a limit of `4GB`, and it is Ring 3.
- The User Data starts at Base `0x0` and has a limit of `4GB`, and it is Ring 3.
- Additionally, Intel architecture strictly dictates that the very first entry in the GDT must be the **Null Descriptor**. This is a completely blank, 64-bit chunk of zeros. The Null Descriptor is used mainly for edge cases, such as uninitialized segment registers.
Effectively, everything overlaps and we bypass slicing up our memory and the translation complexity that would come with that, but we can still enforce privilege levels and access protected mode.
## How a GDT Entry is Built
In the old 16-bit Intel processors, the GDT entry utilized a 24-bit base and a 16-bit limit. When Intel upgraded to the 32-bit processor, the base needed to expand to 32 bits and 20 bits for the limit. If they made them simple, contiguous 32-bit and 20-bit segments, any operating system built for 16-bit processors would not be compatible with newer 32-bit processors as the layout of the base and limit would be different. Instead, to ensure backwards compatibility, they took the new bits and put them into the remaining gaps of the old structure. This makes the physical layout of a 32-bit layout notoriously ugly and fragmented.
Every single GDT entry is exactly 64 bits (8 bytes) longs. The hardware reads those bits as follows:
1. **The Limit - 20 Bits**
	- **Bits 0-15:** Limit Low (The first 16 bits of the limit)
	- **Bits 48-51:** Limit High (The remaining 4 bits, shoved into the middle of the entry)
2. **The Base Address - 32 Bits**
	- **Bits 16-39:** Base Low & Middle (The first 24 bits. In our code this will need to be split into 16 and 8 bits as we do not have a data type that holds 24 bits of information).
	- **Bits 56-63**: Base High (The final 8 bits, located at the very end of the 64-bit entry.)
3. **The Access Byte - 8 Bits**
	- **Bits 40-47:** This is our security level descriptor. It contains information that determines Ring level, whether the segment is executable code or readable data, and whether the segment is currently loaded into memory. Because our limit and base addresses will be identical in each of our segments, this is the only information that differentiates kernel code from user code or data.
4. **The flags - 4 Bits**
	- **Bits 52-55:** These determine the hardware parameters of the segment. The most important parameter for us here is the **Granularity Flag**. With only 20 bits to use in the limit, on the surface we can only technically access `1MB` of data per segment, which in a flat memory model means only `1MB` of total RAM is accessible. To hit 4GB, we'll set the granularity flag to `1`.  The hardware will now read data in `4KB` blocks, and thus also multiply our given limit by `4KB`. With this flag, our new accessible limit becomes exactly `4GB`.
### Building the Access Byte
- **Bit 0 - Accessed**: Set to `1` when accessed/used, starts at `0`.
- **Bit 1 - Read/Write:** `1` means code can be read and data can be read and written. (Always `1` in our case)
- **Bit 2 - Conforming/Dir**: Privilege bypass (Always `0` for us)
- **Bit 3 - Executable:** `1` for code, `0` for data
- **Bit 4 - Type**: Almost always `1` in our case. `0` is used for System/TSS designation.
- **Bit 5 and 6 - Privilege (DPL)**: `00` for Ring 0, `11` for Ring 3.
- **Bit 7 - Present:** Is the segment valid? An initialized segment will always be `1`.
Using this structure, we can calculate our binary strings and convert them into Hexadecimal for the "magic numbers" we'll need in our C code:
- **Kernel Code:** `10011010` -> `0x9A`
- **Kernel Data:** `10010010` -> `0x92`
- **User Code:** `11111010` -> `0xFA`
- **User Data:** `11110010` -> `0xF2`
### Building the Flags
- **Bit 0 - Reserved:** Always `0`.
- **Bit 1 - Long Mode**: `1` for a 64-bit environment, `0` for our 32-bit OS.
- **Bit 2 - Size:** `1` for our 32-bit protected mode OS. `0` for anything in 16-bit real mode.
- **Bit 3 - Granularity:** `1` tells hardware to read in `4KB` blocks, and to multiply the limit by `4KB`. `0` means 1 Byte blocks and using the limit directly.
Our flags will always be: `1100` -> `0xC`
Note that because the flags are only 4 bits, they share a byte with limit high. In our code, we will have to combine these two to create the byte `0xCF`.
## Installing the GDT
After setting up the basic structure of the GDT entries, we create an array to hold each entry (an entry each for Kernel Code and Data, User Space Code and Data, as well as the Null Descriptor). This array is our GDT table. As our last step, we use assembly to flush our GDT to the CPU's `GDTR` register, which will permanently contain a pointer to the physical RAM address where the array lives:
1. **Catching the C Parameter.** When C calls an Assembly function, the stack pointer (`%esp`) currently points to the Return Address (where the CPU needs to go back to when the function finishes). The actual argument we're passing (the pointer to the GDT for the `GDTR` register) is sitting exactly 4 bytes above that return address. Thus we need an instruction to grab our pointer from the stack and store it in the `%eax` register: `mov 4(%esp), %eax`
2. **Loading `GDTR`**: `lgdt (%eax)`. The GDT is now installed.
3. **Resetting the CPU.** The CPU is still using the old BIOS segment rules until we manually overwrite them with our new Kernel Data segment:
```
mov $0x10, %ax
mov %ax, %ds
mov %ax, %es
mov %ax, %fs
mov %ax, %ss
mov %ax, %gs
```
4. **Setting the Code Segment Register**. We can use `mov` to change data segment register, but Intel forbids us from using `mov` to change the Code Segment register (`%cs`). To change `%cs`, we must execute a "Far Jump". We tell the CPU: "*Jump to the label `$.flush`, but simultaneously swap the Code Segment register to `0x08` (The Kernel Code segment)".* 
If our OS doesn't crash after initialization, we've successfully setup a 32-bit Protected Mode environment.
# How Memory is Managed
## Vertical Security
The flat memory model means every segment-- Kernel Code, Kernel Data, User Code, User Data-- has a base of `0x00000000` and a limit of `4GB`. They all completely overlap.
The CPU only knows which space is running because it tracks the current privilege level using the bottom two bits of the Code Segment (`CS`) register. This is called **CPL** (Current Privilege Level).
- When the CPL is `00` (Ring 0), the CPU allows any memory access and privileged instructions (like `hlt`, `cli`, and `outb`).
- When our scheduler executes `iret` to jump to a user program, it intentionally loads a Ring 3 segment selector into `CS`. The CPL becomes `11` (Ring 3). The hardware is now structurally locked. If the user program tries to execute `cli` or read a memory address that the GDT marked as Ring 0, the CPU throws a General Protection Fault.
## Lateral Security
Because our User Data segment spans the entire 4GB of RAM, the CPU hardware considers any memory access within that 4GB perfectly legal, as long as the memory isn't flagged for Ring 0. This means if a program such as `calc.c` accidentally miscalculates a C pointer and writes data out of bounds, it might accidentally overwrite the variables belonging to another program, say `prime.c`. The CPU will not stop this, because to the CPU, both programs are authorized to be in each others space.
## The Solution?
Because the hardware isn't helping us laterally, the burden falls entirely on our OS design.
For the start of the project, we simply must be extremely careful with our pointers to stay within each programs respective memory bubble.
In the future, we will build a structure called the Process Control Block that will enable us to create "Virtual Memory" for every process that allows us to secure memory laterally.