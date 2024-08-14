.include "src/boot/asm_macros2.inc"
.text
/*.global load_page_dir
.type load_page_dir, @function
load_page_dir:
    push %ebp # prologue
    mov %esp, %ebp
    mov 8(%ebp), %eax # arg1 -> skip first two slots of 4 bytes each
    mov %eax, %cr3
    mov %ebp, %esp # epilogue
    pop %ebp
    ret
.global get_cur_pd
.type get_cur_pd, @function
get_cur_pd:
    push %ebp
    mov %esp, %ebp
    mov %cr3, %eax
    push %eax # caller saved??
    mov %ebp, %esp
    pop %ebp
    ret
*/
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

.global switch_task
switch_task:
    # eax,ecx,edx caller saved
    # eip is saved by 'call' inst

    push %ebx
    push %esi
    push %edi
    push %ebp
    
    mov 20(%esp), %edi # PCB of old task
    movl %esp, (%edi) # save sp to PCB of old task
    
    mov 24(%esp), %esi # PCB of new task
    
    mov (%esi), %esp # load new sp
    # mov 4(%esi), %eax # load new PD

    # mov %eax, %cr3 # load new VAS

    pop %ebp
    pop %edi
    pop %esi
    pop %ebx

    ret
