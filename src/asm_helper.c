#include "asm_helper.h"
const uint16_t unused_port = 0x80;

void iowait()
{
	outb(unused_port, 0);
}