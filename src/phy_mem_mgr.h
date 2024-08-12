#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "./drivers/vfs.h"
#define FRAME_SIZE 0x1000

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

typedef struct heap_hdr_mdata {
    size_t size; //exclude hdr and ftr size
    bool free;
    struct heap_hdr_mdata* next;
    struct heap_hdr_mdata* prev;
} heap_hdr_mdata_t;

typedef struct heap_ftr_mdata {
    struct heap_hdr_mdata* cur_hdr;
} heap_ftr_mdata_t;

// Bitmap allocation
// 1024 4KB pages -> 128 bytes
// 2nd page in 1st byte -> (8*1 + 2)
uintptr_t ph_frame_alloc();
void ph_page_free(uintptr_t frame);
void k_heap_initialize(uintptr_t heap_beg, uintptr_t heap_end);
void ph_mem_initialize(uintptr_t dma_beg, uintptr_t u_mem_beg);
void* get_new_pd();
void* get_new_pt();
// heap malloc for the kernel 
void* ph_malloc(size_t sz);
// frees single 4KB page
void ph_free(uintptr_t ptr);
// for testing
// uint8_t get_bitmap(uint8_t idx);