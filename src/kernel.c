#include <stdbool.h>
#include "drivers/driver_headers.h"
#include "hal.h"
#include "irq.h"
#include "multiboot.h"
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

void pg_fault()
{
	printstr("PAGE FULT");
	// handle adding page to the page table etc here
	// uintptr_t* ptr = ph_malloc();
	// if ((get_bitmap(1)&(1<<1)) == 0) printstr("suc");
	// // 001 000 000 --> 4MB
	// printchar(*(char*)ptr);
	
	// ph_free(ptr);

}

void register_interrupts()
{
	irq_register_handler(0,timer_handle);
	irq_register_handler(1,keyboard_handle);
	isr_register_handler(0x0E,pg_fault);
}

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

void kernel_main(unsigned long* mbt) 
{
	multiboot_info_t *mbi = (multiboot_info_t *)mbt;
	memory_info_t* mem_info;
	mem_info->count = 0;
	load_mem_info(mem_info,mbi);
	
	init_hal(); // pass memory bounds for phy mem
	register_interrupts();	
	splash_screen();

	// busy loop
	while(true){}
}