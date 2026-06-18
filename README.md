# Casino OS Lottery Scheduler
A minimal, 32-bit x86 operating system featuring a preemptive lottery scheduler. Casino OS is built from the ground up to demonstrate the process of making a computer run with software multitasking, interrupt handling, and probabilistic process scheduling.
## Directory
* `boot/` - The assembly code that actually starts the OS when you turn the power on. 
* `cpu/` - This code tells the Intel processor how our memory is divided up and how to handle crashes or hardware signals.
* `drivers/` - The code that talks directly to physical hardware chips (the screen, the keyboard, and the internal clock).
* `libc/` - Our custom, mini version of standard C tools (like copying text or printing numbers) since we can't use the normal C library.
* `kernel/` - The main brain of the OS. This contains the lottery scheduler that actually switches the computer back and forth between different programs. 
* `user/` - The user space programs a person would run on the OS (like a calculator).
## Prerequisites 
To build and run Casino OS, you need a Linux/Unix environment with: 
* **GNU Make**
* **GCC / GNU Assembler** (Configured to compile 32-bit code) 
* **QEMU** (Specifically `qemu-system-i386` to emulate the computer)
## Build & Execution Instructions 
We use a standard Makefile to compile everything.

**Compiling instructions:**
```bash 
make       # Compile the OS only
make run   # Compile and start the OS
make clean # Remove old compiled files
```