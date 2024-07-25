#include <stdbool.h>
#include "drivers/driver_headers.h"
#include "hal.h"
#include "irq.h"
#include "multiboot.h"

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

void register_interrupts()
{
	irq_register_handler(0,timer_handle);
	irq_register_handler(1,keyboard_handle);
}

void kernel_main(unsigned long* mbt, unsigned int magic) 
{
	multiboot_info_t *mbi = (multiboot_info_t *)mbt;
	memory_info_t* mem_info;
	mem_info->count = 0;
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
	
	init_hal();
	register_interrupts();	
	splash_screen();
	// busy loop
	while(true){}
}