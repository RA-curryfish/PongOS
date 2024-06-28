#pragma once
#include "utils.h"

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

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
uint16_t cursor;

void clear_terminal()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	clear_terminal();
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putcharat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
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
	if (terminal_row == 0 && terminal_column == 0)
		clear_terminal(); // todo: scroll instead of clear screen
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
}
