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

### Misc.
- Remember to keep the kernel in some kind of a busy loop otherwise it will exit and you will spend a half a day trying to figure out why interrupts aren't working :)
- 4kb = 0x1000, 64kb = 0x10000, 1mb = 0x100000, 4mb = 0x400000 


### Interrupts
There are three categories:
- CPU interrupts: The first 32 are reserved by the CPU for itself
- Hardware interrupts: Helps the CPU avoid wasting time by polling devices. The mapping depends on Programmable Interrupt Controller (PIC, eg: Intel 8259). In this case, two are chained together having 8 pins each.
- Software interrupts: The OS defines the routines to run (INT 0x..)

A table of 256 entries called Interrupt Descriptor Table needs to be set up to handle interrupts. The BIOS sets up a temporary one at 0x00, but the kernel can move it later once BIOS exits. Each entry is 8 bytes. Once this kernel is initialized, the Hardware Abstraction Layer loads the appropriate register with the IDT descriptor so the CPU knows where to find the table.

Then, ISR are initialized mapped by setting flags, ISR function addresses, etc, in each entry of the IDT. For hardware interrupts, the PIC will be set up first with ICWs. Then the 16 irqs will be registered by the ISR register handler. Then interrupts can be enabled. Now, whenever an interrupt occurs, the CPU indexes into the IDT, picks the ISR function address executes those instructions. The registered handler for the ISR will be called from here. 

Links:
- https://wiki.osdev.org/8259_PIC

### Memory
Need to use BIOS to detect memory map to figure out which physical memory addresses are available. Before booting into the kernel, the multiboot specification stores a pointer to the multiboot_info structure in %ebx. So we just need to push it on the stack before calling kernel_main. This can be retrieved as an argument to kernel_main. Then we can extract the memory maps from here. I just used the the structures from the multiboot.h file from GNU. You can also check it while booting using command mode: lsmmap

| Base     | Length  | Base Decimal | Length Decimal | Status |
| -------- | ------- | ------------ | -------------- | ------ |
| 0        | 9fc00   | 0            | 654336         | avail  |
| 9fc00    | 400     | 654336       | 1024           | res    |
| f0000    | 10000   | 983040       | 65536          | res    |
| 100000   | fee0000 | 1048576      | 267255808      | avail  |
| ffe0000  | 20000   | 2.68E+08     | 131072         | res    |
| fffc0000 | 40000   | 4.29E+09     | 262144         | res    |
|          |         |              | 268369920      |        |

Using buddy allocation, There are 11 layers with the biggest having 4MB block and the smallest, a 4KB block. The layout is the following:

| Hex addr | 400000      | 480000 | 500000 | 580000 | 600000 | 680000 | 700000 | 780000 |
| -------- | ----------- | ------ | ------ | ------ | ------ | ------ | ------ | ------ |
| 0        | 4MB         |        |        |        |        |        |        |        |
| 1        | 2MB         |        |        |        |   2MB  |        |        |        |
| 2        | 1MB         |        | 1MB    |        |   1MB  |        |    1MB |        |
| 3        | 512KB       | 512KB  | 512KB  | 512KB  | 512KB  | 512KB  | 512KB  | 512KB  |
| 4        |             |        |        |        |        |        |        |        |
| 5        |             |        |        |        |        |        |        |        |
| 6        |             |        |        |        |        |        |        |        |
| 7        |             |        |        |        |        |        |        |        |
| 8        |             |        |        |        |        |        |        |        |
| 9        |             |        |        |        |        |        |        |        |
| 10       | 4kb,4kb…... |        |        |        |        |        |        |

With the array to represent the system as follows:

| Array | 4MB | 2MB | 2MB | 1MB | 1MB | 1MB | 1MB |     | 4KB  |
| ----- | --- | --- | --- | --- | --- | --- | --- | --- | ---- |
| idx   | 0   | 1   | 2   | 3   | 4   | 5   | 6   | ….. | 2046 |

Links:
- https://wiki.osdev.org/Detecting_Memory_(x86)
- https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Boot-information-format
