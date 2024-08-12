.altmacro
.macro PUSH_EADX1
    push %eax
    push %ebx
    push %ecx
    push %edx
.endm

.macro POP_EDAX1
    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
.endm

.macro VGA_PRINT_STRING1 s
    LOCAL loop, end
    PUSH_EADX1
    mov \s, %ecx
    mov vga_current_line, %eax
    mov $0, %edx
    mov $25, %ebx
    div %ebx
    mov %edx, %eax
    mov $160, %edx
    mul %edx
    lea 0xb8000(%eax), %edx
    mov $0x0f, %ah
    loop:
        mov (%ecx), %al
        cmp $0, %al
        je end
        mov %ax, (%edx)
        add $1, %ecx
        add $2, %edx
        jmp loop
    end:
        incl vga_current_line
        POP_EDAX1
.endm

.macro HEX_NIBBLE1 reg
    LOCAL letter, end
    cmp $10, \reg
    jae letter
    add $'0, \reg
    jmp end
    letter:
        add $0x37, \reg
    end:
.endm

.macro HEX1 c
    mov \c, %al
    mov \c, %ah
    shr $4, %al
    HEX_NIBBLE1 <%al>
    and $0x0F, %ah
    HEX_NIBBLE1 <%ah>
.endm

.macro VGA_PRINT_HEX_41 in=<%eax>
    LOCAL loop
    PUSH_EADX1
    mov \in, %ecx
    push $0
    mov $2, %ebx
    loop:
        HEX1 <%cl>
        mov %ax, %dx
        shl $16, %edx
        HEX1 <%ch>
        mov %ax, %dx
        push %edx
        shr $16, %ecx
        dec %ebx
        cmp $0, %ebx
        jne loop
        mov %esp, %edx
        VGA_PRINT_STRING1 <%edx>
        add $12, %esp
        POP_EDAX1
.endm


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
