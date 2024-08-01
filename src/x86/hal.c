#include "hal.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "../drivers/driver_headers.h"
#include "../ph_mem_allocator.h"

void pg_fault()
{
	printstr("PAGE FULT");
	// handle adding page to the page table etc here
	// uintptr_t* ptr = ph_malloc();
	// if ((get_bitmap(1)&(1<<1)) == 0) printstr("suc");
	// // 001 000 000 --> 4MB
	// printchar(*(char*)ptr);
	
	// ph_free(ptr);

}

void register_interrupts()
{
	irq_register_handler(0,timer_handle);
	irq_register_handler(1,keyboard_handle);
    irq_register_handler(6, fpc_irq);
	isr_register_handler(0x0E,pg_fault);
}

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

    register_interrupts();

	terminal_initialize();

    // Initialize Physical memory
    ph_mem_initialize();

    // init floppy driver
    fpc_init((char*)DMA_BEGIN);
}