#include "terminal.h"
#include "hal.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

void kernel_main(void) 
{
	splash_screen();
	init_hal();
	// __asm("int $0x21");
	keyboard_handle();
}