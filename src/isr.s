# Interrup Service Routine
# All int. jmp to a common label, and then handled by C code
.code32

extern isr_handler_func

.macro ISR_NOERRCODE n
.global ISR\n
ISR\n:
    push 0 # push dummy err code
    push \n # push interrupt num
    jmp isr_common
.endm

.macro ISR_ERRCODE n
.global ISR\n
ISR\n:
    # cpu pushes dummy err code
    push \n # push interrupt num
    jmp isr_common
.endm

%include "isrs_gen.inc"

isr_common:
    pusha
    
    xor %eax, %eax
    mov %ds, %ax # ds is 16 bit
    push %eax

    mov $DATA_SEG, %ax # switch to kernel data segment 
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    push %esp # pass esp as argument for isr handler to access stuff
    call isr_handler
    add $4, %esp # clear stack, equivalent to pop

    pop %eax # revert back to state before "kernel data segment"
    mov %ax, %ds # replace vals to old
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    popa # restore gen. registers
    add $8, %esp # clear stack of error code, and int. num
    iret # special return, pops cs, eip, eflags, ss, esp


