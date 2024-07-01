#include "utils.h"
#include "terminal.h"
#include "kbd.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

#define INT_DISABLE 0
#define INT_ENABLE  0x200
#define PIC1 0x20
#define PIC2 0xA0
#define ICW1 0x11
#define ICW4 0x01

void init_pics(int pic1, int pic2)
{
   /* send ICW1 */
	outb(PIC1, ICW1);
	outb(PIC2, ICW1);

   /* send ICW2 */
	outb(PIC1 + 1, pic1);   
	outb(PIC2 + 1, pic2);   

   /* send ICW3 */
	outb(PIC1 + 1, 4);   
	outb(PIC2 + 1, 2);

   /* send ICW4 */
	outb(PIC1 + 1, ICW4);
	outb(PIC2 + 1, ICW4);

   /* disable all IRQs */
	outb(PIC1 + 1, 0xFF);
}

bool capslock = false; // todo: query for capslock state
bool shift = false;

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	printstr("**********************************************\n");
	printstr("\t\tPong OS\n");
	printstr("**********************************************\n");
	// init_pics(0x20, 0x28);
	
	uint8_t c = 0;
	uint8_t s = 0;
	uint16_t data_port = 0x60;
	uint16_t status_port = 0x64;
	do {
		s = inb(status_port);
		if (((s&1)==1)) {
			c = inb(data_port);
			if (c>=0x80) { continue;}
			else if (kbd[c] != -1 && kbd[c] != -2 && kbd[c] != -3) { // not backspace, end or capslock
				if (kbd[c] >= 97 && kbd[c] <= 122 && (capslock || shift)) { printchar(caps_kbd[c]); }
				else { printchar(kbd[c]); }
			}
			else if (kbd[c] == -1) backspace();
			else if (kbd[c] == -3) capslock = !capslock;
			// else if (kbd[c] == -4) shift = true; // todo: fix shift
			// else if (kbd[c] == -5) shift = 0;
			else if (c == 0x2A || c == 0x36) shift = true;
		}
	}while(kbd[c] != -2);
}