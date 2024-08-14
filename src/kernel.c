#include <stdbool.h>
#include "x86/hal.h"
#include "boot/multiboot.h"
#include "drivers/vfs.h"
#include "drivers/terminal.h"
#include "libf.h"
#include "phy_mem_mgr.h"
#include "load_proc.h"
#include "task.h"

#ifdef __linux__
#error "using linux??"
#endif

#ifndef __i386__
#error "not using x86 compiler??"
#endif

pcb* cur_task;

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

uint8_t get_free_mem_region(memory_info_t* mem_info, uint8_t cnt)
{
	for(uint8_t i=cnt;i<mem_info->count;i++) if(mem_info->regions[i].type == 1) return i;
	return mem_info->count+1;
}

void kernel_bsy_loop()
{
	// busy loop
	while(true){}
}

void foo()
{
	printf("testing\n");
	switch_task(cur_task, cur_task->next_task);

	printf("wicked game\n");
	switch_task(cur_task, cur_task->next_task);

	printf("until the ashes of eden fall\n");
	switch_task(cur_task, cur_task->next_task);
}

void kernel_main(uintptr_t heap_end, uintptr_t heap_begin, unsigned long* mbt) 
{
	multiboot_info_t *mbi = (multiboot_info_t *)mbt;
	k_heap_initialize(heap_begin, heap_end);
	memory_info_t* mem_info = (memory_info_t*)ph_malloc(sizeof(memory_info_t));
	mem_info->count = 0;
	load_mem_info(mem_info,mbi); // use this info to store user memory being and end
		
	uint8_t dma_reg = get_free_mem_region(mem_info,1); // get first free region for DMA after 1MB
	if(dma_reg>mem_info->count) printf("No free regions\n");
	uintptr_t dma_beg = mem_info->regions[dma_reg].addr;
	uintptr_t u_mem_beg = dma_beg+0x300000; // hardcoding for now
	
	init_hal(dma_beg, u_mem_beg); // pass memory bounds for phy mem
	
	ph_free((uintptr_t)mem_info);

	splash_screen();

	pcb* kernel_task = (pcb*)ph_malloc(sizeof(pcb));
	cur_task = kernel_task;
	init_kernel_task((uint32_t*)&cur_task, kernel_task);
	// file_t* f = (file_t*)ph_malloc(sizeof(file_t));
	// f->type = DEVICE;
	// load(f);
	// ph_free((uintptr_t)f);

	pcb* task = (pcb*)ph_malloc(sizeof(pcb));
	create_task(task,kernel_task,0,foo);
	
	kernel_task->next_task = task;
	
	switch_task(kernel_task, kernel_task->next_task);

	printf("i just knew too much\n");
	switch_task(kernel_task, kernel_task->next_task);
	
	printf("no iiiiiii dont wanna fall\n");
	switch_task(kernel_task, kernel_task->next_task);
	
	printf("stayyy with me dont let mee goooooo\n");

	kernel_bsy_loop();
}