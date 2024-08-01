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
.skip 16384 # 16 KiB
.skip 16384 # 16 KiB
.skip 16384 # 16 KiB
stack_top:

.section .data
.equ page_directory, __end_align_4k
.equ page_table, __end_align_4k+0x1000
.equ app_page_table, __end_align_4k+0x2000

.section .text
.include "boot/asm_macros.inc" # weird path because make is in diff path
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp
	pushl %ebx # pointer to multiboot info
	
	# Set up PD and PTs
	SETUP_PD
	mov $0, %eax
	SETUP_PT <$page_table>
	mov $0x400, %eax
	SETUP_PT <$app_page_table>

	# enable paging
	mov $page_directory, %eax
	mov %eax, %cr3
    ENABLE_PAGING
	
	# setup GDT and jump to protected mode
	PROTECTED_MODE
	
	call kernel_main

	cli
1:	hlt
	jmp 1b

.size _start, . - _start
