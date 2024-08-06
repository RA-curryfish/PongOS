#include <stdbool.h>
#include "x86/hal.h"
#include "boot/multiboot.h"
#include "drivers/vfs.h"
#include "drivers/terminal.h"
#include "libf.h"
#include "ph_mem_allocator.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

typedef struct memory_info {
	uint8_t count;
	multiboot_memory_map_t regions[16];
} memory_info_t;

void load_mem_info(memory_info_t* mem_info, multiboot_info_t* mbi)
{
	if (CHECK_FLAG (mbi->flags, 6)) {
		multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)mbi->mmap_addr;
		for (;(unsigned long)mmap < (mbi->mmap_addr+mbi->mmap_length);mem_info->count++) {
			mem_info->regions[mem_info->count].addr = mmap->addr;
			mem_info->regions[mem_info->count].len = mmap->len;
			mem_info->regions[mem_info->count].size = mmap->size;
			mem_info->regions[mem_info->count].type = mmap->type;
			mmap = (multiboot_memory_map_t *)((unsigned long)mmap+ mmap->size + sizeof(mmap->size));
		}
	}
}

void load_binary()
{
	// unsigned char* buf = ""; // read this buffer from a file in the HDD/Floppy?
}

void kernel_main(uintptr_t heap_end, uintptr_t heap_begin, uintptr_t stack_top, uintptr_t stack_bottom, unsigned long* mbt) 
{
	multiboot_info_t *mbi = (multiboot_info_t *)mbt;
	ph_mem_initialize(heap_begin,heap_end);
	
	memory_info_t* mem_info = (memory_info_t*)ph_malloc(sizeof(memory_info_t));
	mem_info->count = 0;
	load_mem_info(mem_info,mbi); // use this info to store user memory being and end
	
	init_hal(mem_info); // pass memory bounds for phy mem
	// ph_free(mem_info);
	
	splash_screen();

	for(uint8_t i=0;i<1;i++) {
		printf("region: %d\n",i+1);
		printf("addr: %x\n",mem_info->regions[i].addr);
		printf("len: %x\n",mem_info->regions[i].len);
		printf("size: %x\n",mem_info->regions[i].size);
		printf("type: %d\n",mem_info->regions[i].type);
	}
	
	// char* s = (char*)ph_malloc(10);
	// printf("s addr: %x\n", s);
	// char* s1 = (char*)ph_malloc(20);
	// printf("s1 addr: %x\n", s1);
	// ph_free(s);
	// char* s2 = (char*)ph_malloc(10);
	// printf("s2 addr: %x\n", s2);
	// ph_free(s2);
	// s2 = (char*)ph_malloc(20);
	// printf("s2 addr: %x\n", s2);
	// char* buf; uint16_t buf_len=512;
	// file_t* f; f->type = DEVICE;
	// printf("%x", f);
	// printf("%x", &f->type);
	// open(f);
	// read(f,&buf,1,buf_len);
	// for(uint16_t i=0;i<buf_len;i++) printchar(*(buf+i));

	// busy loop
	while(true){}
}