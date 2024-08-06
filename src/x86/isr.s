# Interrup Service Routine
# All int. jmp to a common label, and then handled by C code
.code32

.macro isr_noerrorcode c
.global ISR\c
ISR\c:
    push $0 # dummy err code
    push $\c # push interrupt num
    jmp isr_common
.endm

.macro isr_errorcode c
.global ISR\c
ISR\c:
    # cpu pushes err code
    push $\c # push interrupt num
    jmp isr_common
.endm

.section .text
# Include all ISRs which expand into functions by the above macros
.include "src/x86/isrs_gen.inc"

# Common part of the ISR
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
    call isr_handler_func
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
