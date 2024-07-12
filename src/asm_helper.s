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

.macro PUSH_EADX
    push %eax
    push %ebx
    push %ecx
    push %edx
.endm

.macro POP_EDAX
    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
.endm

.macro VGA_PRINT_STRING s
    LOCAL loop, end
    PUSH_EADX
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
        POP_EDAX
.endm

.macro HEX_NIBBLE reg
    LOCAL letter, end
    cmp $10, \reg
    jae letter
    add $'0, \reg
    jmp end
    letter:
        add $0x37, \reg
    end:
.endm

.macro HEX c
    mov \c, %al
    mov \c, %ah
    shr $4, %al
    HEX_NIBBLE <%al>
    and $0x0F, %ah
    HEX_NIBBLE <%ah>
.endm

.macro VGA_PRINT_HEX_4 in=<%eax>
    LOCAL loop
    PUSH_EADX
    mov \in, %ecx
    push $0
    mov $2, %ebx
    loop:
        HEX <%cl>
        mov %ax, %dx
        shl $16, %edx
        HEX <%ch>
        mov %ax, %dx
        push %edx
        shr $16, %ecx
        dec %ebx
        cmp $0, %ebx
        jne loop
        mov %esp, %edx
        VGA_PRINT_STRING <%edx>
        add $12, %esp
        POP_EDAX
.endm


/*
paging_setup:
    .equ page_dir, __end_align_4k
    .equ page_tab, __end_align_4k + 0x1000

     store address of a page table into first entry of page dir 
       zero out some bits??? necessary??
        eax  = 1 0001 0000 0000 0000 = 0x11000
        ax     = 0001 0000 0000 0000 =  0x1000
        0xF000 = 1111 0000 0000 0000

            0001 0000 0000 0000
        &   1111 0000 0000 0000
        ax= 0001 0000 0000 0000
        eax remains same

       set flags for first byte, PS bit unset means PT exists with 4kb page
    mov $page_tab, %eax
    and $0xF000, %ax
    mov %eax, $page_dir
    mov $0b00100111, %al
    mov %al, page_directory

    /* store 1024 mappings of pages in the page table
    mov $0, %eax
    mov $page_table, %ebx
    
    page_setup_start:
        cmp $0x400, %eax
        je page_setup_end
        /* Top 20 address bits.
        mov %eax, %edx
        shl $12, %edx
        /*
        Set flag bits 0-7. We only set to 1:
        -   bit 0: Page present
        -   bit 1: Page is writable.
            Might work without this as the permission also depends on CR0.WP.

        mov $0b00000011, %dl
        /* Zero flag bits 8-11
        and $0xF0, %dh
        mov %edx, (%ebx)
        inc %eax
        add $4, %ebx
        jmp page_setup_start
    page_setup_end:
*/

; .data
; .global page_dir
; .equ page_dir, __end_align_4k
; .global page_tab
; .equ page_tab, __end_align_4k+0x1000

.text
.global load_page_dir
.type load_page_dir, @function
load_page_dir:
    push %ebp /* prologue */
    mov %esp, %ebp
    mov 8(%esp), %eax /* arg1 -> skip first two slots of 4 bytes each*/
    mov %eax, %cr3
    mov %ebp, %esp /* epilogue */
    pop %ebp
    ret

.global enable_paging
.type enable_paging, @function
enable_paging:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret

.global tst_paging
.type tst_paging, @function
tst_paging:
    push %ebp
    mov %esp, %ebp
