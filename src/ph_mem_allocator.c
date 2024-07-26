#include "ph_mem_allocator.h"

uint8_t* mem_base;
mem_block_node_t mem_blk_arr[(1<<(MAX_MEM_BLOCK_LAYER+1))-1]; // can be made into bool array?
// left child = 2*i+1
// right child = 2*i+2

void init_mem_region()
{
    mem_base = MEM_BASE_ADDR;
}

uint32_t* allocate_mem()
{
    return find_free_block();
}

uint32_t* find_free_block()
{

}