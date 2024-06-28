#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

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

size_t strlen(const char *s)
{
	size_t len=0;
	while(s[len++]);
	return len-1;
}

void memcpy(void *dest, void *source, size_t len)
{
	char *d = (char*)dest;
	char *s = (char*)source;
	while(len)
	{
		*(d+len) = *(s+len);
		len--;
	}
}
