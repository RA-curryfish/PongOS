#include "hal.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "../drivers/driver_headers.h"
#include "../phy_mem_mgr.h"
#include "../libf.h"

extern void panic();

void pg_fault(registers* regs)
{
	// printf("%x",*(uint32_t*)regs->eip);
    // printf("\n%x %x %x\n", regs->eip, regs->esp, regs->ebp);
    printstr("PAGE FULT");
    panic();
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

void init_hal(uintptr_t dma_beg, uintptr_t u_mem_beg)
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
    // Initialize physical memory
    ph_mem_initialize(dma_beg,u_mem_beg);
    // Initialize terminal
	terminal_initialize();
    // init floppy driver
    fpc_init((unsigned char*)dma_beg);
}