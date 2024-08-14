# PongOS
Building my version of the BareBonesOS, a minimal OS built for the x86 architecture. Making the game 'Pong' on it, and coding in assembly and freestanding C (C without stdlib), and learning concepts in computer architecture, operating systems, linkers/loaders and so on.

## Milestones
- Create a binary, store on floppy, and then run it 
- Create a file system parser (FAT16?) and read from floppy
- Launching applications (WIP - text app, Pong app)
- Basic terminal to type in commands and interact with the OS (fixing small bugs)
- Handle phy and virtual memory management (can improve)
- ....

## Progress
- GDT set up (null, kernel code, kernel data segments), protected mode entered
- Paging enabled with 0-4MB being identity mapped, 1MB-2MB DMA, 4MB-32MB for user space
- IDT set up to handle interrupts (hardware, KB, etc)
- A terminal pops up, can type in the cmd line but can't use arrow keys
- Floppy controller set up and skeleton VFS created
- A simple kernel heap allocator created
- Can load data from floppy to memory and jump to that location (running a binary)

## Building
- You need to build a cross compiler for x86 arch. Download GCC and binutils and build them in a safe directory (away from system stuff)
- https://wiki.osdev.org/GCC_Cross-Compiler shows how to do this
- You may have to change the paths according/in the packer.sh script to make it run
- The script is pretty self-explanatory. Once the set up is done accordingly, you can simply run it to produce an iso
- To run this iso you need qemu, you can see how it is run in the script. Since this is a work in progress, giving too less of memory may cause the OS to crash

## Details
Some details of certain concepts 

### Misc.
- Remember to keep the kernel in some kind of a busy loop otherwise it will exit and you will spend a half a day trying to figure out why interrupts aren't working :)
- 4kb = 0x1000, 64kb = 0x10000, 1mb = 0x100000, 4mb = 0x400000
- Be careful while using uint (or use big enough uints) as loop variables as they loop back and won't terminate loops
- Keep pointer type in mind while incrementing (char* +4 is equivalent to int* +1)

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
| 100000   | 3ee0000 | 1048576      | 65929216       | avail  |
| 3fe0000  | 20000   | 66977792     | 131072         | res    |
| fffc0000 | 40000   | 4.29E+09     | 262144         | res    |
|          |         |              | 67043328       |        |

(switched to using a simple bitmap for now, have not fully implemented the things below)

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

DMA is between 1MB-2MB. Kernel lies at 2MB. Since kernel lies in identity mapped region, memory allocs in kernel will be with physical memory. User memory will lie at 4MB till 32MB. Memory allocs in userland will be virtual, with the upper part of VAS mapped to kernel.
The address space is arranged something like this: (need to move kernel PD and PTs either behind stack_bottom or ahead of stack_top else it's gonna bite me in the butt later ----> there were no issues in the first place, considered old vals of PD,PT)

|           |            | 1MB    | 2MB       |      |             |             |           |               |             |            | Kernel PD | 1024 entries | PT1 1024 entries | PT2 1024 entries | First user frame |    | 8MB    |
| --------- | ---------- | ------ | --------- | ---- | ----------- | ----------- | --------- | ------------- | ----------- | ---------- | --------- | ------------ | ---------------- | ---------------- | ---------------- | -- | ------ |
| Addresses | 0          | 100000 | 200000    | …    | 203F00      | 206000      | 20E000    | 20E000        | 215FCC      | 216000     | 217000    | 217004       | 218000           | 219000           | 400000           | …. | 800000 |
| Values    | bios stuff | DMA    | multiboot | text | kernel_main | kheap_begin | kheap_end | kstack_bottom | kstack_var1 | kstack_top | 218027    | 219027       | 3                | 403              |                  |    |        |

In x86, cr3 register has PD address. The virtual address is 32 bits with bits 31-22 are PD entry, 21-12 are PT entry, 11-0 are offset into the page (4KB). Thus, accessing, the virtual address to access physical address 4MB would be (0000000001)(0000000000)(000000000000).

Links:
- https://wiki.osdev.org/Detecting_Memory_(x86)
- https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Boot-information-format

### Floppy Controller
Using DMA at ~1MB to transfer data. The floppy image needs to be in the right format (see the commands in the flopper.sh file). Documentation is a mess and confusing to figure out order of steps, timing, etc.
no  further comments ;_;

Links:
- https://www.ardent-tool.com/floppy/Floppy_Programming.html
- https://forum.osdev.org/viewtopic.php?t=13538
- https://wiki.osdev.org/Floppy_Disk_Controller#st2

### Multitasking
The code for context switch (CS) needs to be in assembly to prevent the C compiler from adding function pro/epilogue. Some registers are saved by caller, the rest need to be saved by the CS. A new stack needs to be set up before CSing. The %esp will now point at the top of this new stack. You need to manually push values on stack that would be popped off by the CS code after changing the stack. Also, if you want to call another function (before calling the task function), add the second return address as well and any other parameter the function takes on the stack too. For example, my stack looks like this: 

| Meaning | Address | Value  | Name       |
| ------- | ------- | ------ | ---------- |
| esp     | 3fe4    | 216FBC | ebp val    |
|         | 3fe8    | 0      | edi        |
|         | 3fec    | 207034 | esi        |
|         | 3ff0    | 207034 | ebx        |
| ret1    | 3ff4    | 200BB0 | exch_tasks |
| ret2    | 3ff8    | 207034 | func       |
| param1  | 3ffc    | 2043f0 | task       |

'func' is the entry function of the task, 'exch_tasks' updates the current task to the new task and takes the parameter 'task'. esp is the top of stack when CS occurs. There can be another function after 'task' that is a general wind down function.