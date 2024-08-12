.altmacro 
.macro PROTECTED_MODE
    .equ CODE_SEG,8
    .equ DATA_SEG, 16

    lgdt gdt_descriptor
    mov %cr0, %eax
    orl 0x1, %eax
    mov %eax, %cr0

    ljmp $CODE_SEG, $prot_mode

    gdt_start:
    gdt_null:
        .long 0
        .long 0
    gdt_code:
        .word 0xffff
        .word 0x0
        .byte 0x0
        .byte 0b10011010
        .byte 0b11001111
        .byte 0x0
    gdt_data:
        .word 0xffff
        .word 0x0
        .byte 0x0
        .byte 0b10010010
        .byte 0b11001111
        .byte 0x0
    gdt_end:
    gdt_descriptor:
        .word gdt_end - gdt_start
        .long gdt_start
    vga_current_line:
        .long 0 
    .code32
    prot_mode:
    mov $DATA_SEG, %ax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs
        mov %ax, %ss
.endm

.macro ENABLE_PAGING
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
.endm

.macro SETUP_PD
    mov $page_directory, %ebx
    
	# first PT identity mapped 0-4MB
    mov $page_table, %eax
    and $0xF000, %ax
    mov %eax, (%ebx)
    mov $0b00100111, %al
    mov %al, (%ebx)

    # second PT mapped 4MB-8MB
	mov $app_page_table, %eax
	and $0xF000, %ax
	mov %eax, 4(%ebx)
	mov $0b00100111, %al
	mov %al, 4(%ebx)

    # PD mapped to itself
    mov %ebx, %eax
    and $0xF000, %ax
    mov %eax, 4092(%ebx)
    mov $0b00100111, %al
    mov %al, 4092(%ebx)
.endm

.macro SETUP_PT pt
    # set 1024 ptes
	LOCAL page_setup_start, page_setup_end
    mov $0, %ecx
    mov \pt, %ebx
	page_setup_start:
		cmp $0x400, %ecx
		je page_setup_end
		mov %eax, %edx
		shl $12, %edx
		mov $0b00000011, %dl
		and $0xF0, %dh
		mov %edx, (%ebx)
		inc %ecx
        inc %eax
		add $4, %ebx
		jmp page_setup_start
	page_setup_end:
.endm
