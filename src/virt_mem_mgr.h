#pragma once
#include <stdint.h>
#define VIRT_MEM_BEGIN 0x0
#define VIRT_MEM_END 0x400000 //4MB
#define PAGE_SIZE 0x1000

typedef struct virtual_addr_space {
    uint8_t* code_begin; //1 page each
    uint8_t* global_begin; 
    uint8_t* heap_begin;
    uint8_t* stack_begin;
} vas_t;
void* get_new_pt();
void* malloc();
void free(void* ptr);
// allocates pages sequentially
// void* virt_page_alloc(void* pt, uint16_t entry);
void virt_page_free(void* page);
void* get_new_pd();