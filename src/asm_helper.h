#pragma once
#include <stdint.h>
#include <stddef.h>

const uint16_t kbd_data_port = 0x60;
const uint16_t kbd_status_port = 0x64;

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