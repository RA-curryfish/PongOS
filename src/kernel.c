#include <stdbool.h>
#include "drivers/driver_headers.h"
#include "hal.h"
#include "irq.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

void register_interrupts()
{
	irq_register_handler(0,timer_handle);
	irq_register_handler(1,keyboard_handle);
}

void kernel_main(void) 
{
	splash_screen();
	init_hal();
	register_interrupts();	
	// busy loop
	while(true){}
}