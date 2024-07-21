#include "terminal.h"
#include "hal.h"
#include "irq.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

void timer(registers* regs)
{
	// timer IRQ
}

void kernel_main(void) 
{
	splash_screen();
	init_hal();
	irq_register_handler(0,timer);
	irq_register_handler(1,keyboard_handle);
	
	// busy loop
	while(true){
		
	}
}