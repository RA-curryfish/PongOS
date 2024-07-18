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
- Terminal initialied for basic CLI, need to fix small bugs

## Details
Some details of certain concepts 

### Interrupts
A table of 256 entries called Interrupt Descriptor Table needs to be set up to handle interrupts. The BIOS sets up a temp one at 0x00, but the kernel can move it later once BIOS exits. Each entry is 8 bytes. Once this kernel is initialized, the Hardware Abstraction Layer loads the appropriate register with the IDT descriptor so the CPU knows where to find the table. Then, ISR are initialized mapped by setting flags, ISR function addresses, etc,  in each entry of the IDT. Now, whenever an interrupt occurs, the CPU indexes into the IDT, picks the ISR function address executes those instructions. The registered handler for the ISR will be called from here.  
