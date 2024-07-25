#include "irq.h"
#include "pic.h"
#include "drivers/terminal.h"

#define PIC_OFFSET 0x20 // start at 32 in the Interrupt table

irq_handler irq_handlers[16];
extern uint8_t __attribute__((cdecl)) enable_interrupts();
extern uint8_t __attribute__((cdecl)) disable_interrupts();

void irq_handler_func(registers* regs)
{
    uint8_t irq = regs->interrupt - PIC_OFFSET;

    // uint8_t pic_isr = pic_read_serv_reg();
    // uint8_t pic_irr = pic_read_req_reg();

    if(irq_handlers[irq] != NULL)
        irq_handlers[irq](regs);
    else
        printstr("unhandled IRQ\n");
    
    pic_end_interrupt(irq);    
}

void irq_initialize()
{
    pic_init(PIC_OFFSET, PIC_OFFSET+8);

    for(uint8_t i=0; i<16; i++) 
        isr_register_handler(PIC_OFFSET+i, irq_handler_func);

    enable_interrupts();
}

void irq_register_handler(uint8_t irq, irq_handler handler)
{
    irq_handlers[irq] = handler;
}

