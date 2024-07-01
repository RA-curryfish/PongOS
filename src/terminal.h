#pragma once
#include "utils.h"
#define VGA_MEM_BASE 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// void terminal_putchar(char c);
// void printchar(char c);

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
uint16_t* terminal_line_ptrs[VGA_HEIGHT];

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc  | (uint16_t) color << 8;
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void clear_line(size_t row)
{
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		terminal_buffer[(row*VGA_WIDTH)+x] = vga_entry(' ', terminal_color);
	}
}

void clear_terminal()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		clear_line(y);
		// for (size_t x = 0; x < VGA_WIDTH; x++) {
		// 	const size_t index = y * VGA_WIDTH + x;
		// 	terminal_buffer[index] = vga_entry(' ', terminal_color);
		// }
	}
}

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) VGA_MEM_BASE;
	clear_terminal();
	for (size_t y = 0; y < VGA_HEIGHT; y++) terminal_line_ptrs[y] = (uint16_t*)(sizeof(uint16_t)*y*VGA_WIDTH+VGA_MEM_BASE);
}

void scroll_terminal()
{
	uint16_t terminal_line[VGA_WIDTH+1];
	terminal_line[VGA_WIDTH] = 0;
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		memcpy((void*)terminal_line,(void*)terminal_line_ptrs[y],sizeof(uint16_t)*VGA_WIDTH);
		clear_line(y-1);
		memcpy((void*)terminal_line_ptrs[y-1],(void*)terminal_line,sizeof(uint16_t)*VGA_WIDTH);
	}
	clear_line(VGA_HEIGHT-1);
}

void terminal_putcharat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x; // x is width, y is height
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	switch(c) 
	{
		case '\n':
			terminal_column = 0;
			terminal_row = (terminal_row == (VGA_HEIGHT-1)) ? 0 : terminal_row+1;
			break;
		case '\t':
			size_t rem = VGA_WIDTH-terminal_column;
			terminal_column = rem<8 ? 8-rem : terminal_column+8;
			break;
		default:
			terminal_putcharat(c, terminal_color, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) 
			{
				terminal_column = 0;
				if (++terminal_row == VGA_HEIGHT)
					terminal_row = 0;
			}
	}
	update_cursor(terminal_column, terminal_row);
	if (terminal_row == 0 && terminal_column == 0)
		scroll_terminal();
}

void printchar(char data) 
{
	terminal_putchar(data);
}

void printstr(const char *data)
{
	size_t size = strlen(data);
	for (size_t i = 0; i < size; i++)
		printchar(data[i]);
}

void backspace()
{
	if(terminal_column == 0) {
		if(terminal_row == 0); // do nothing
		else {
			terminal_column = VGA_WIDTH;
			--terminal_row;
		}
	}
	else {
		--terminal_column;
	}
	terminal_putcharat(' ',terminal_color,terminal_column,terminal_row);
	update_cursor(terminal_column, terminal_row);
}
