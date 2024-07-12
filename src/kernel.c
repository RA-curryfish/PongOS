#include "terminal.h"
#include "kbd.h"
#include "mem_manager.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

void splash_screen()
{
	/* Initialize terminal interface */
	disable_cursor();
	clear_terminal();
	terminal_initialize();
	printstr("\n\n\n\n\n\n\n\n\n\n");
	printstr("\t\t*****************************************\n");
	printstr("\t\t*\t\tPong OS\t\t*\n");
	printstr("\t\t*****************************************\n");
	
	while(true)
	{
		if (inb(status_port)&1) break;
	}

	inb(data_port);
	toggle_cmd_offset();
	clear_terminal();
	emit_label(cmd_label,cmd_label_color);
	enable_cursor(0,15);
}

void keyboard_handle()
{
	uint8_t c = 0;
	uint8_t s = 0;
	do {
		s = inb(status_port);
		if (((s&1)==1)) {
			c = inb(data_port);
			if (c>=0x80) { continue;}
			else if (kbd[c] != -1 && kbd[c] != -2 && kbd[c] != -3) { // not backspace, end or capslock
				if (capslock) /*(kbd[c] >= 97 && kbd[c] <= 122 && (capslock || shift))*/ printchar(caps_kbd[c]);
				else printchar(kbd[c]);
			}
			else if (kbd[c] == -1) backspace();
			else if (kbd[c] == -3) capslock = !capslock;
			// else if (kbd[c] == -4) shift = true; // todo: fix shift
			// else if (kbd[c] == -5) shift = 0;
			else if (c == 0x2A || c == 0x36) shift = true;
		}
	} while(kbd[c] != -2);
}

void kernel_main(void) 
{
	// init_pd();
	// init_pt();
	// add_entry();
	// load_pd(page_dir);
	splash_screen();
	keyboard_handle();
}