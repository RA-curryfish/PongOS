#include "terminal.h"
#include "../asm_helper.h"
#include "../libf.h"
#include "keyboard.h"
#include "../cmds.h"

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
uint8_t line_wrap=0;
const char* cmd_list[] = {
	"help",
	"clear",
	"text",
	"pong",
	"echo",
	"exit"
};
uint8_t cmd_list_size=0;

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

void toggle_cmd_offset(bool on) 
{
	cmd_offset = on? strlen(cmd_label) : 0;
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

void terminal_initialize(void* kernel_task) 
{
	terminal_row = 0;
	terminal_column = 0+cmd_offset;
	cmd_label_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN,VGA_COLOR_BLACK);
	cmd_list_size = sizeof(cmd_list)/sizeof(cmd_list[0]);
	error_label_color = vga_entry_color(VGA_COLOR_LIGHT_RED,VGA_COLOR_BLACK);
	terminal_text_color = vga_entry_color(VGA_COLOR_CYAN,VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) VGA_MEM_BASE;
	for (size_t y = 0; y < VGA_HEIGHT; y++) terminal_line_ptrs[y] = (ter_el*)(sizeof(uint16_t)*y*VGA_WIDTH+VGA_MEM_BASE);
	cmd_initialize(kernel_task);
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
	terminal_column = len;
}

void execute_cmd(const size_t cmd_num, char* args)
{	
	switch (cmd_num)
	{
		case 0:
			toggle_cmd_offset(false);
			help(cmd_list,cmd_list_size);
			toggle_cmd_offset(true);
			emit_label(cmd_label, cmd_label_color);
			break;
		case 1:
			clear_terminal();
			toggle_cmd_offset(true);
			emit_label(cmd_label, cmd_label_color);
			break;
		case 2:
			toggle_cmd_offset(false);
			clear_terminal();
			text();
			break;
		case 3:
			toggle_cmd_offset(false);
			clear_terminal();
			pong();
			break;
		case 4:
			toggle_cmd_offset(false);
			while(*args != '\0' && *args != '\n') {
				printf("%c", *args);
				args++;
			}
			printf("\n");
			toggle_cmd_offset(true);
			emit_label(cmd_label,cmd_label_color);
		default:
			break;
	}
}

char* extract_char(ter_el* elem)
{
	char* ret = '\0'; size_t i;
	for(i=0;i<VGA_WIDTH;i++) {
		if((elem+i)->c == '\0' || (elem+i)->c == '\n') break;
		ret[i] = (elem+i)->c;
	}
	ret[i==VGA_WIDTH? VGA_WIDTH-1 : i] = '\0'; 
	return ret;
}

int8_t cmd_parser(char* usr_cmd)
{
	// add code to check multiline cmd like "echo .....<nextline>..."
	for(size_t j=0;j<cmd_list_size;j++)
		if(strcmp_cmd(usr_cmd,cmd_list[j]) == true) return j;
	return -2;
}

void terminal_putchar(char c) 
{
	switch(c) {
		case '\n':
			if(cmd_offset) {
				char* usr_cmd = extract_char(terminal_line_ptrs[terminal_row]);
				usr_cmd += cmd_offset;
				int8_t cmd_num = terminal_column==cmd_offset? -1 : cmd_parser(usr_cmd); // just enter a new line if nothing is typed
				line_wrap=0;
				if (cmd_num >= 0) {
					if (terminal_row == (VGA_HEIGHT-1)) scroll_terminal();
					else terminal_row++;
					terminal_column=0;
					usr_cmd = usr_cmd + strlen(cmd_list[cmd_num]);
					if(usr_cmd[0] == ' ') usr_cmd++;
					execute_cmd(cmd_num, usr_cmd);
				}
				else {
					terminal_column = cmd_offset;

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
				terminal_column = 0;
			}
			break;
		case '\t': // todo: complete tab space implementation
			size_t rem = VGA_WIDTH-terminal_column;
			terminal_column = rem<8 ? 8-rem : terminal_column+8;
			break;
		case -2: // end key
			execute_cmd(1,"\0");
			toggle_cmd_offset(true);
			break;
		default:
			terminal_putcharat(c, terminal_text_color, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) {
				terminal_column = 0;
				++line_wrap;
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
	if(cmd_offset==0) { // text mode
		if(terminal_column == 0) {
			if(terminal_row != 0) {
				--terminal_row;
				terminal_column=VGA_WIDTH;
			}
		}
		else {
			--terminal_column;
		}
	}
	else { // cmd mode
		if(!line_wrap) {
			// if(terminal_column == cmd_offset) {
			// 	if(terminal_row == 0); // do nothing
			// }
			if(terminal_column > cmd_offset) {
				--terminal_column;
			}
		}
		else {
			if(terminal_column == 0) {
				if(terminal_row != 0) {
					--terminal_row;
					line_wrap = line_wrap>0? line_wrap-1 : 0;
					terminal_column=VGA_WIDTH;
				}
			}
			else {
				--terminal_column;
			}
		}
	}
	
	terminal_putcharat(' ',terminal_text_color,terminal_column,terminal_row);
	update_cursor(terminal_column, terminal_row);
}

void splash_screen()
{
	/* Initialize terminal interface */
	disable_cursor();
	clear_terminal();
	toggle_cmd_offset(false);
	printstr("\n\n\n\n\n\n\n\n\n\n");
	printstr("\t\t*****************************************\n");
	printstr("\t\t*\t\tPong OS\t\t*\n");
	printstr("\t\t*****************************************\n");
	
	while(true) if (inb(KBD_STATUS_PORT)&1) break; // yes, it is funny
	inb(KBD_DATA_PORT);
	
	toggle_cmd_offset(true);
	clear_terminal();
	emit_label(cmd_label,cmd_label_color);
	enable_cursor(0,15);
}