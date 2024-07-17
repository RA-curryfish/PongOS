.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data
.equ page_directory, __end_align_4k
.equ page_table, __end_align_4k+0x1000

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp

	# identity mapped first 4MB of RAM
	# set first pde
	mov $page_table, %eax
    and $0xF000, %ax
    mov %eax, page_directory
    mov $0b00100111, %al
    mov %al, page_directory
	
	# set 1024 ptes
	mov $0, %eax
    mov $page_table, %ebx
	page_setup_start:
		cmp $0x400, %eax
		je page_setup_end
		mov %eax, %edx
		shl $12, %edx
		mov $0b00000011, %dl
		and $0xF0, %dh
		mov %edx, (%ebx)
		inc %eax
		add $4, %ebx
		jmp page_setup_start
	page_setup_end:

/*	mov $0x1235, %eax
    mov %eax, 0x500
	VGA_PRINT_HEX_4 0x500
	/*# map page 0 to 0x1000
	mov page_table, %eax
    or $0x00001000, %eax
    mov %eax, page_table*/

	# enable paging
	mov $page_directory, %eax
	mov %eax, %cr3
    ENABLE_PAGING
	
	# setup GDT and jump to protected mode
	PROTECTED_MODE

	/*mov $0x6666, %eax
	mov %eax, 0
	VGA_PRINT_HEX_4 0x0
	# disable paging
	mov %cr0, %eax
    and $0x7FFFFFFF, %eax
    mov  %eax, %cr0
	VGA_PRINT_HEX_4 0x0
	VGA_PRINT_HEX_4 0x1000*/
	
	call kernel_main

	cli
1:	hlt
	jmp 1b

.size _start, . - _start
