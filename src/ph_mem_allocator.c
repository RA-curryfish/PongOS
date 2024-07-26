#include "ph_mem_allocator.h"

uint8_t* mem_base;
const uint16_t mem_blk_arr_size = (1<<(MAX_MEM_BLOCK_LAYER))-1;
mem_block_node_t mem_blk_arr[mem_blk_arr_size]; // can be made into bool array?
// left child = 2*i+1
// right child = 2*i+2

void init_mem_region()
{
    mem_base = MEM_BASE_ADDR;
    uint8_t layer=0;
    uint16_t inc=1;
    for(uint16_t i=0;i<mem_blk_arr_size;)
    {
        uint16_t tmp = 0;
        while(tmp<inc) {
            mem_blk_arr[i++].layer = layer;
            mem_blk_arr[i++].used = false;
        }
        
        inc = inc<<1;
        layer++;
    }
}

uint32_t* allocate_mem()
{
    return find_free_block(10);
}

uint32_t get_sz_frm_lyr(uint8_t layer)
{
    return (0x1000<<(MAX_MEM_BLOCK_LAYER-layer-1));
}

// find smallest free block
uint32_t* find_free_block(uint32_t size)
{
    uint16_t i;
    bool allocate_fail;
    for(i=0;i<mem_blk_arr_size && !mem_blk_arr[i].used;i++)
    {
        // if requested size is lesser than the current node, cannot allocate page
        if(get_sz_frm_lyr(mem_blk_arr[i].layer) < size) {
            allocate_fail=true;
            break;
        }
        //.....
    }
}