# PongOS
Building my version of the BareBonesOS, a minimal OS built for the x86 architecture. Making the game 'Pong' on it, and coding in assembly and freestanding C (C without stdlib), and learning concepts in computer architecture, operating systems, linkers/loaders and so on.

## Milestones
- Creating a file system
- Enabling and using paging for applications (WIP)
- Basic terminal to type in commands and interact with the OS (fixing small bugs)
- Launching applications (WIP - text app, Pong app)
- Refactoring code (WIP)
- .... 

## Progress
- GDT set up (null, kernel code, kernel data segments)
- Paging enabled with first 4MB being identity mapped
- Switched to protected mode
- IDT set up to handle interrupts
- Terminal looking thing made

## Details
Some details of certain concepts 

### Interrupts
There are three categories:
- CPU interrupts: The first 32 are reserved by the CPU for itself
- Hardware interrupts: Helps the CPU avoid wasting time by polling devices. The mapping depends on Programmable Interrupt Controller (PIC, eg: Intel 8259). In this case, two are chained together having 8 pins each.
- Software interrupts: The OS defines the routines to run (INT 0x..)

A table of 256 entries called Interrupt Descriptor Table needs to be set up to handle interrupts. The BIOS sets up a temporary one at 0x00, but the kernel can move it later once BIOS exits. Each entry is 8 bytes. Once this kernel is initialized, the Hardware Abstraction Layer loads the appropriate register with the IDT descriptor so the CPU knows where to find the table.

Then, ISR are initialized mapped by setting flags, ISR function addresses, etc, in each entry of the IDT. For hardware interrupts, the PIC will be set up first with ICWs. Then the 16 irqs will be registered by the ISR register handler. Then interrupts can be enabled. Now, whenever an interrupt occurs, the CPU indexes into the IDT, picks the ISR function address executes those instructions. The registered handler for the ISR will be called from here.

Links:
- https://wiki.osdev.org/8259_PIC