#include "hal.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "ph_mem_allocator.h"

void init_hal()
{
    // Loads the IDT into register using idt desciptor val
    // idt_descriptor -> struct {sizeof(256*idt_entries), ptr to idt[256]}
    idt_initialize();
    
    // Initialize gates -> Fill each IDT entry by idt_set_gate
    // -> enable every gate (sst IDT_FLAG_PRESENT)
    isr_initialize();

    // Initialize PIC -> register 16 ISRs (32-47) with IRQ handler
    // -> enable interrupts
    irq_initialize();

    // Initialize Physical memory
    ph_mem_initialize();
}