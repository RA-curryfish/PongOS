#include "utils.h"
#include "print.h"
#include "kbd.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

bool capslock = false;
bool shift = false;

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
//	terminal_setcolor(VGA_COLOR_CYAN);
	
	int8_t c = 0;
	uint8_t port = 0x60;
	uint8_t cnt = 0;
	do {
		if(inb(port) != (char)c) {
			c = inb(port);
			if (kbd[c] != -1 && kbd[c] != -2 && kbd[c] != -3) { //&& kbd[c] != '\n'){    // not backspace, end or capslock
				if ((capslock && !shift) || (!capslock && shift)) { printchar(caps_kbd[c]); }
				//if (kbd[c] >= 97 && kbd[c] <= 122 && ((capslock && !shift) || (!capslock && shift))) {printchar(caps_kbd[c]); }
				else { printchar(kbd[c]); }
			}
			else if (kbd[c] == -1) {
				backspace();
			}
			else if (kbd[c] == -3) {
				capslock = !capslock;
			}
			//printchar(cnt);
		}
		else {
			if (kbd[c] == -4) shift = 1;
			else if (kbd[c] == -5) shift = 0;
//			printstr("hlo\n");
		}
	}while(kbd[c] != -2);

	//print("**********************************************\n");
	//print("\t\tPong OS\n");
	//print("**********************************************\n");
}
