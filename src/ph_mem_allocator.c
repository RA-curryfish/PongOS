#include "ph_mem_allocator.h"
#include "utils.h"

const uint16_t mem_blk_arr_size = (1<<((uint32_t)MAX_MEM_BLOCK_LAYER+1))-1; // need 2047 len array
mem_block_node_t mem_blk_arr[(1<<((uint32_t)MAX_MEM_BLOCK_LAYER+1))-1]; // can be made into bool array?
// left child = 2*i+1,  right child = 2*i+2

uint32_t get_sz_frm_lyr(uint8_t layer)
{
    return (0x1000<<((uint32_t)MAX_MEM_BLOCK_LAYER-layer));
}

// find smallest free block
mem_block_node_t* find_free_block(uint32_t size)
{
    uint16_t i;
    uint16_t best_fit_idx = 5000; // invalid at beginning
    // change to binary search
    for(i=0;i<mem_blk_arr_size && !mem_blk_arr[i].used;i++)
    {
        // if requested size is lesser than the current node, cannot allocate here
        if(get_sz_frm_lyr(mem_blk_arr[i].layer) < size) break;
        best_fit_idx = i;
    }
    // value below 0x400000 in range indicate error
    if(best_fit_idx>=mem_blk_arr_size) return (mem_block_node_t*){MEM_BASE_ADDR-1};
    return &mem_blk_arr[best_fit_idx];
}

mem_range_t* allocate_mem(uint32_t size)
{
    // mark block as used now
    mem_block_node_t* free_blk = find_free_block(size);
    free_blk->used = true;
    return &free_blk->range;
}

bool free_mem(mem_range_t* range)
{
    uint32_t size = range->end - range->start + 1;
    uint32_t low = ((uint32_t)MEM_BASE_ADDR/size)-1; // get first index of that size
    uint16_t high = low+((uint32_t)MEM_BASE_ADDR/size)-1; // get last index of that size
    uint32_t search = range->start;
    uint8_t disp = offsetof(mem_block_node_t,range);
    uint16_t idx = (uint16_t)bin_search((void*)mem_blk_arr, disp, search, low, high, sizeof(mem_block_node_t),true);
    if (idx>=mem_blk_arr_size) return false; 
    mem_blk_arr[idx].used = false;
    return true;
}

void ph_mem_initialize()
{
    uint8_t layer=0;
    uint16_t inc=1;
    for(uint16_t i=0;i<mem_blk_arr_size;)
    {
        uint16_t tmp = 0;
        uint32_t mem_base = MEM_BASE_ADDR;
        while(tmp<inc && i<mem_blk_arr_size) {
            mem_blk_arr[i].layer = layer;
            mem_blk_arr[i].used = false;
            mem_blk_arr[i].range.start = mem_base;
            mem_base += get_sz_frm_lyr(layer);
            mem_blk_arr[i].range.end = mem_base-1;
            i++;
            tmp++;
        }
        inc = inc<<1;
        layer++;
    }
}