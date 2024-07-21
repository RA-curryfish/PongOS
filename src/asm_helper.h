#pragma once
#include <stdint.h>
#include <stddef.h>

static inline void outb(uint16_t port, char val)
{
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t v;
	asm volatile("inb %w1,%0": "=a" (v) : "Nd" (port));
	return v;
}
// Operations on the PIC are slow, so waste cycles?
static inline void iowait()
{
	outb(0x80, 0);
}