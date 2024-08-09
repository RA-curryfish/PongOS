#pragma once
#define VIRT_MEM_BEGIN 0x0
#define VIRT_MEM_END 0x400000 //4MB
#define PAGE_SIZE 0x1000

void* get_new_pt();
void* malloc();
void free(void* ptr);
// allocates pages sequentially
// void* virt_page_alloc(void* pt, uint16_t entry);
void virt_page_free(void* page);
void* get_new_pd();