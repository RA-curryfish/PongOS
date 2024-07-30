#include "isr.h"
#include "idt.h"
#include <stddef.h>
#include "../drivers/terminal.h"

isr_handler isr_handlers[256];

// static const char* const exceptions[] = {
//     "Divide by zero error",
//     "Debug",
//     "Non-maskable Interrupt",
//     "Breakpoint",
//     "Overflow",
//     "Bound Range Exceeded",
//     "Invalid Opcode",
//     "Device Not Available",
//     "Double Fault",
//     "Coprocessor Segment Overrun",
//     "Invalid TSS",
//     "Segment Not Present",
//     "Stack-Segment Fault",
//     "General Protection Fault",
//     "Page Fault",
//     "",
//     "x87 Floating-Point Exception",
//     "Alignment Check",
//     "Machine Check",
//     "SIMD Floating-Point Exception",
//     "Virtualization Exception",
//     "Control Protection Exception ",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "",
//     "Hypervisor Injection Exception",
//     "VMM Communication Exception",
//     "Security Exception",
//     ""
// };

void isr_initialize_gates();
extern void panic();

void isr_initialize()
{
    isr_initialize_gates();
    for (uint16_t i=0;i<256;i++)
        idt_enable_gate(i);
    idt_disable_gate(0x80);
}

void __attribute__((cdecl)) isr_handler_func(registers* regs)
{
    if(isr_handlers[regs->interrupt] != NULL) 
        isr_handlers[regs->interrupt](regs); // call isr with regs as param
    else if(regs->interrupt >= 32)
        printstr("UNHANDleD ISR\n");
    else {
        printstr("PANIC");
        panic();
    }
}

void isr_register_handler(int interrupt, isr_handler handler)
{
    isr_handlers[interrupt] = handler;
    idt_enable_gate(interrupt);
}