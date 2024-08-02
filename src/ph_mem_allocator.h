#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// User app memory pool at 4MB-32MB
#define U_MEM_BASE 0x400000
#define  U_MEM_END 0x1FFFFFF
#define FRAME_SIZE 0x1000
// DMA in identity mapped region
#define DMA_BEGIN 0x100000
#define DMA_END 0x200000

// typedef struct mem_range {
//     uint32_t start;
//     uint32_t end;
// }mem_range_t;

// typedef struct mem_block_node {
//     uint8_t layer; // don't really need this
//     mem_range_t range;
//     bool used;
// }mem_block_node_t;

/* 11 layers of memory block sizes
0 - 4MB
1 - 2MB 2MB
2 - 1MB 1MB 1MB 1MB
3 - 512KB 512KB 512KB 512KB 512KB 512KB 512KB 512KB
4 - 256KB
5 - 128KB
6 - 64KB
7 - 32KB
8 - 16KB
9 - 8KB
10 - 4kB
*/ 

// mem_range_t* allocate_mem(uint32_t size);
// bool free_mem(mem_range_t* range);

typedef struct heap_free_ll {
    uintptr_t cur;
    size_t size; // can guess from next and used->next?
    size_t prev_size;
    struct heap_free_ll* next;
    struct heap_used_ll* prev; //??
} heap_free_ll_t;

typedef struct heap_used_ll {
    uintptr_t cur;
    struct heap_used_ll* next;
    struct heap_used_ll* prev;
} heap_used_ll_t;

// Bitmap allocation
// 1024 4KB pages -> 128 bytes
// 2nd page in 1st byte -> (8*1 + 2)

void ph_mem_initialize(uintptr_t heap_beg, uintptr_t heap_end);
// heap malloc for the kernel 
void* ph_malloc(size_t sz);
// frees single 4KB page
void ph_free(uintptr_t ptr);
// for testing
uint8_t get_bitmap(uint8_t idx);