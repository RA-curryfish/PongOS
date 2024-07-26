#pragma once
#include <stdint.h>
#include <stdbool.h>

// ~ 4MB of memory between 4MB-8MB
#define MEM_BASE_ADDR 0x400000
#define MEM_END_ADDR 0x7FFFFF
#define MAX_MEM_BLOCK_LAYER 11

typedef struct mem_range {
    uint32_t start;
    uint32_t end;
}mem_range_t;

typedef struct mem_block_node {
    uint8_t layer; // don't really need this
    mem_range_t range;
    bool used;
}mem_block_node_t;

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

void init_mem_region();
mem_range_t* allocate_mem(uint32_t size);