.altmacro 
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
