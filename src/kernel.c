#include "terminal.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

bool pid_arr[8];
uint8_t pid_ctr=1;

typedef struct process_control_block {
	uint8_t pid;
	uint32_t stack_top;
	uint16_t cr3;
	struct process_control_block* next_task;
	uint16_t state;
	// todo: add other fields
}pcb;

void create_pcb()
{

}

void switch_task(uint32_t addr)
{
	
}

void kernel_main(void) 
{
	// create PCB for kernel with pid = 1
	// pid_arr[pid_ctr] = true;
	// create_pcb();
	splash_screen();
	keyboard_handle();
}