#pragma once
#include <stddef.h>
#include <stdint.h>
#define VGA_MEM_BASE 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

typedef struct terminal_element {
	unsigned char c; // first 8 bits
	unsigned char color; // last 8 bits
} ter_el;

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

void disable_cursor();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void update_cursor(int x, int y);
void toggle_cmd_offset();
void clear_line(size_t row);
void clear_terminal();
void terminal_initialize();
void scroll_terminal();
void terminal_putcharat(char c, uint8_t color, size_t x, size_t y);
void emit_label(const char *label, uint8_t color);
char* extract_char(ter_el* elem);
int8_t cmd_parser(size_t row);
void execute_cmd(const size_t cmd_num);
void terminal_putchar(char c);
void printchar(char data);
void printstr(const char *data);
void backspace();
void splash_screen();