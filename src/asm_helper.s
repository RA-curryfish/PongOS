.text
.global load_page_dir
.type load_page_dir, @function
load_page_dir:
    push %ebp /* prologue */
    mov %esp, %ebp
    mov 8(%ebp), %eax /* arg1 -> skip first two slots of 4 bytes each*/
    mov %eax, %cr3
    mov %ebp, %esp /* epilogue */
    pop %ebp
    ret

.global load_idt
.type load_idt, @function
load_idt:
    push %ebp
    mov %esp, %ebp
    mov 8(%ebp), %eax
    lidt (%eax)
    mov %ebp, %esp
    pop %ebp
    ret

.global panic
.type panic, @function
panic:
    cli
    hlt

.global enable_interrupts
enable_interrupts:
    sti
    ret

.global disable_interrupts
disable_interrupts:
    cli
    ret
