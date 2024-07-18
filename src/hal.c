#include "hal.h"
#include "idt.h"
#include "isr.h"

void init_hal()
{
    // Loads the IDT into register using idt desciptor val
    // idt_descriptor -> struct {sizeof(256*idt_entries), ptr to idt[256]}
    idt_initialize();
    
    // Initialize gates -> Fill each IDT entry by idt_set_gate
    // -> enable every gate (sst IDT_FLAG_PRESENT)
    // -> disable int 0x80 for now...
    isr_initialize();
}