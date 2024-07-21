#include "terminal.h"
#include "kbd.h"
#include "asm_helper.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_text_color;
uint8_t cmd_label_color;
uint8_t error_label_color;
uint16_t* terminal_buffer;
ter_el* terminal_line_ptrs[VGA_HEIGHT];
const char* error_cmd_label = "unkown command";
char* cmd_label = "cmd:";
uint8_t cmd_offset = 0;
const char* cmd_list[] = {
	"pong",
	"text",
	"exit"
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc  | (uint16_t) color << 8;
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

void toggle_cmd_offset() 
{
	cmd_offset = cmd_offset == 0? strlen(cmd_label) : 0;
}

void clear_line(size_t row)
{
	for (size_t x = 0; x < VGA_WIDTH; x++) terminal_buffer[(row*VGA_WIDTH)+x] = vga_entry(' ', terminal_text_color);
}

void clear_terminal()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		clear_line(y);
	}
	terminal_row =0; terminal_column = 0+cmd_offset;
	update_cursor(terminal_column,terminal_row);
}

void terminal_initialize() 
{
	terminal_row = 0;
	terminal_column = 0+cmd_offset;
	cmd_label_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN,VGA_COLOR_BLACK);
	error_label_color = vga_entry_color(VGA_COLOR_LIGHT_RED,VGA_COLOR_BLACK);
	terminal_text_color = vga_entry_color(VGA_COLOR_CYAN,VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) VGA_MEM_BASE;
	for (size_t y = 0; y < VGA_HEIGHT; y++) terminal_line_ptrs[y] = (ter_el*)(sizeof(uint16_t)*y*VGA_WIDTH+VGA_MEM_BASE);
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

void emit_label(const char *label, uint8_t color)
{
	size_t len = strlen(label);
	for(size_t i=0;i<len;i++) terminal_putcharat(label[i],color,i,terminal_row);
}

char* extract_char(ter_el* elem)
{
	char* ret = '\0'; size_t i;
	for(i=0;i<VGA_WIDTH;i++) {
		if((elem+i)->c == ' ' || (elem+i)->c == '\0' || (elem+i)->c == '\n') break;
		ret[i] = (elem+i)->c;
	}
	ret[i==VGA_WIDTH? VGA_WIDTH-1 : i] = '\0'; 
	return ret;
}

int8_t cmd_parser(size_t row)
{
	char* usr_cmd = extract_char(terminal_line_ptrs[row]);
	for(size_t j=0;j<3;j++) // todo: remove hardcoding
		if(strcmp_cmd(usr_cmd+cmd_offset,cmd_list[j]) == true) return j;
	return -2;
}

void execute_cmd(const size_t cmd_num)
{
	terminal_putcharat('$',cmd_label_color,terminal_column,terminal_row);
	
	switch (cmd_num)
	{
	case 0:
		break;
	default:
		break;
	}
}

void terminal_putchar(char c) 
{
	switch(c) {
		case '\n':
			int8_t cmd_num = terminal_column==cmd_offset? -1 : cmd_parser(terminal_row);
			terminal_column = 0+cmd_offset;
			if  (cmd_offset) {
				if (cmd_num >= 0) {
					execute_cmd(cmd_num);
				}
				else {
					if (cmd_num < -1) {
						if (terminal_row == (VGA_HEIGHT-1)) scroll_terminal();
						else terminal_row++;
						emit_label(error_cmd_label, error_label_color);	
					}
					if (terminal_row == (VGA_HEIGHT-1)) scroll_terminal();
					else terminal_row++;
					emit_label(cmd_label, cmd_label_color);
				}
			}
			else {
				if (terminal_row == (VGA_HEIGHT-1)) scroll_terminal();
				else terminal_row++;
			}
			break;
		case '\t': // todo: complete tab space implementation
			size_t rem = VGA_WIDTH-terminal_column;
			terminal_column = rem<8 ? 8-rem : terminal_column+8;
			break;
		default:
			terminal_putcharat(c, terminal_text_color, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) {
				terminal_column = 0;
				if (terminal_row == (VGA_HEIGHT-1)) scroll_terminal();
				else terminal_row++;
			}
	}
	update_cursor(terminal_column, terminal_row);
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
	if(terminal_column == cmd_offset) {
		if(terminal_row == 0); // do nothing
		else { // todo: fix backspace when command goes to the next line
			// terminal_column = VGA_WIDTH; 
			// --terminal_row;
		}
	}
	else {
		--terminal_column;
	}
	terminal_putcharat(' ',terminal_text_color,terminal_column,terminal_row);
	update_cursor(terminal_column, terminal_row);
}

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
		if (inb(kbd_status_port)&1) break;
	}

	inb(kbd_data_port);
	toggle_cmd_offset();
	clear_terminal();
	emit_label(cmd_label,cmd_label_color);
	enable_cursor(0,15);
}

void keyboard_handle()
{		
	uint8_t c = 0;
	uint8_t s = 0;
	// s = inb(kbd_status_port);
	// if (((s&1)==1)) {
	c = inb(kbd_data_port);
	if (c>=0x80) { }
	else if (kbd[c] != -1 && kbd[c] != -2 && kbd[c] != -3) { // not backspace, end or capslock
		if (capslock) /*(kbd[c] >= 97 && kbd[c] <= 122 && (capslock || shift))*/ printchar(caps_kbd[c]);
		else printchar(kbd[c]);
	}
	else if (kbd[c] == -1) backspace();
	else if (kbd[c] == -3) capslock = !capslock;
	// else if (kbd[c] == -4) shift = true; // todo: fix shift
	// else if (kbd[c] == -5) shift = 0;
	else if (c == 0x2A || c == 0x36) shift = true;
	// }
	// while(kbd[c] != -2);
}