#include "ph_mem_allocator.h"
#include "utils.h"

const uint16_t mem_blk_arr_size = (1<<(MAX_MEM_BLOCK_LAYER));
mem_block_node_t mem_blk_arr[(1<<(MAX_MEM_BLOCK_LAYER))]; // can be made into bool array?
// left child = 2*i+1
// right child = 2*i+2

uint32_t get_sz_frm_lyr(uint8_t layer)
{
    return (0x1000<<(MAX_MEM_BLOCK_LAYER-layer-1));
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
    if(best_fit_idx>=mem_blk_arr_size) return (mem_block_node_t*){0};
        printchar(mem_blk_arr[2047].layer+'0');
    if(best_fit_idx == 2046)
        printchar('o');
    return &mem_blk_arr[best_fit_idx];
}

mem_range_t* allocate_mem(uint32_t size)
{
    // mark block as used now
    mem_block_node_t* free_blk = find_free_block(size);
    free_blk->used = true;
    return &free_blk->range;
}

void free_mem(mem_range_t* range)
{
    // binary search on start range
    uint32_t search = range->start;
    uint16_t low = 0, high = mem_blk_arr_size-1;
    uint8_t disp = offsetof(mem_block_node_t,range);
    // &(mem_blk_arr[0].range.start) - &(mem_blk_arr[0]);
    uint16_t idx = (uint16_t)bin_search((void*)mem_blk_arr, disp, search, low, high, sizeof(mem_block_node_t),true);
    mem_blk_arr[idx].used = false;
}

void init_mem_region()
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