#include "terminal.h"
#include "hal.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

void timer()
{
	// timer IRQ
}

void kb()
{
	// KEYBOARD HANDLERRRR
}

void kernel_main(void) 
{
	splash_screen();
	init_hal();
	irq_register_handler(0,timer);
	irq_register_handler(1,kb);
	keyboard_handle();
}