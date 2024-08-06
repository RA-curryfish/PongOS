#include "ph_mem_allocator.h"
#include "libf.h"
#define MEM_BASE_ADDR U_MEM_BASE
#define MEM_END_ADDR 0x800000
// const uint16_t mem_blk_arr_size = (1<<((uint32_t)MAX_MEM_BLOCK_LAYER+1))-1; // need 2047 len array
// mem_block_node_t mem_blk_arr[(1<<((uint32_t)MAX_MEM_BLOCK_LAYER+1))-1]; // can be made into bool array?
// // left child = 2*i+1,  right child = 2*i+2

// uint32_t get_sz_frm_lyr(uint8_t layer)
// {
//     return (0x1000<<((uint32_t)MAX_MEM_BLOCK_LAYER-layer));
// }

// // find smallest free block
// mem_block_node_t* find_free_block(uint32_t size)
// {
//     uint16_t i;
//     uint16_t best_fit_idx = 5000; // invalid at beginning
//     // change to binary search
//     for(i=0;i<mem_blk_arr_size && !mem_blk_arr[i].used;i++)
//     {
//         // if requested size is lesser than the current node, cannot allocate here
//         if(get_sz_frm_lyr(mem_blk_arr[i].layer) < size) break;
//         best_fit_idx = i;
//     }
//     // value below 0x400000 in range indicate error
//     if(best_fit_idx>=mem_blk_arr_size) return (mem_block_node_t*){MEM_BASE_ADDR-1};
//     return &mem_blk_arr[best_fit_idx];
// }

// mem_range_t* allocate_mem(uint32_t size)
// {
//     // mark block as used now
//     mem_block_node_t* free_blk = find_free_block(size);
//     free_blk->used = true;
//     return &free_blk->range;
// }

// bool free_mem(mem_range_t* range)
// {
//     uint32_t size = range->end - range->start + 1;
//     uint32_t low = ((uint32_t)MEM_BASE_ADDR/size)-1; // get first index of that size
//     uint16_t high = low+((uint32_t)MEM_BASE_ADDR/size)-1; // get last index of that size
//     uint32_t search = range->start;
//     uint8_t disp = offsetof(mem_block_node_t,range);
//     uint16_t idx = (uint16_t)bin_search((void*)mem_blk_arr, disp, search, low, high, sizeof(mem_block_node_t),true);
//     if (idx>=mem_blk_arr_size) return false; 
//     mem_blk_arr[idx].used = false;
//     return true;
// }

// void ph_mem_initialize()
// {
//     uint8_t layer=0;
//     uint16_t inc=1;
//     for(uint16_t i=0;i<mem_blk_arr_size;)
//     {
//         uint16_t tmp = 0;
//         uint32_t mem_base = MEM_BASE_ADDR;
//         while(tmp<inc && i<mem_blk_arr_size) {
//             mem_blk_arr[i].layer = layer;
//             mem_blk_arr[i].used = false;
//             mem_blk_arr[i].range.start = mem_base;
//             mem_base += get_sz_frm_lyr(layer);
//             mem_blk_arr[i].range.end = mem_base-1;
//             i++;
//             tmp++;
//         }
//         inc = inc<<1;
//         layer++;
//     }
// }

const uint8_t mem_bitmap_sz = (uint32_t)(
        (((uint32_t)MEM_END_ADDR-(uint32_t)MEM_BASE_ADDR)/(uint32_t)FRAME_SIZE)
        /8); 
uint8_t mem_bitmap[(uint32_t)(
        (((uint32_t)MEM_END_ADDR-(uint32_t)MEM_BASE_ADDR)/(uint32_t)FRAME_SIZE)
        /8)];
static uintptr_t HEAP_BEGIN;
static uintptr_t HEAP_END;
static size_t HDR_SIZE;
static size_t FTR_SIZE;
static heap_hdr_mdata_t* free_head;

void ph_page_alloc()
{
    uintptr_t* ret = NULL;
    uint8_t frame_num = 0;
    bool found = false;
    uint16_t i;
    for(i =0;i<mem_bitmap_sz;i++)
    {
        if((mem_bitmap[i]&(1<<frame_num)) == 0) {
            found = true;
            break;
        }
        if(frame_num==7) frame_num=255;
        frame_num++;
    }
    if(!found) return ret;
    mem_bitmap[i] = mem_bitmap[i] | (1<<frame_num);
    ret = (uintptr_t*)(MEM_BASE_ADDR + (8*i + frame_num)*FRAME_SIZE);
    return ret;
}

void ph_page_free(uintptr_t ptr)
{
    uint16_t frame_num = (uint16_t)((uint16_t*)ptr-(uint16_t*)MEM_BASE_ADDR)/FRAME_SIZE;
    uint16_t idx = frame_num/8;
    frame_num = frame_num%8;
    mem_bitmap[idx] = mem_bitmap[idx] & ~(1<<frame_num);
}

uint8_t get_bitmap(uint8_t idx)
{
    return mem_bitmap[idx];
}

void* ph_malloc(size_t sz)
{
    heap_hdr_mdata_t* tmp = free_head;
    while(tmp && (tmp+sz+HDR_SIZE+FTR_SIZE)<HEAP_END) { // go thru free list
        if(tmp->size < sz) continue;
        else if(tmp->size > sz) { // split block
            heap_hdr_mdata_t* new_free = tmp+HDR_SIZE+sz+FTR_SIZE;
            new_free->free = true;
            new_free->prev=tmp->prev;
            new_free->next=tmp->next;
            new_free->size=tmp->size-sz-HDR_SIZE-FTR_SIZE;
            heap_ftr_mdata_t* ftr=tmp+tmp->size;
            ftr->cur_hdr=new_free;
            
            tmp->size = sz;

            if(tmp==free_head) free_head=new_free; // shift head to next free block
        }
        else { // else exact fit
            if(tmp==free_head) free_head=tmp->next;
        }
        tmp->free=false;
        if(tmp->prev) tmp->prev->next = tmp->next;
        if(tmp->next) tmp->next->prev = tmp->prev;
        tmp->next=NULL;tmp->prev=NULL;
        heap_ftr_mdata_t* ftr = tmp+HDR_SIZE+sz;
        ftr->cur_hdr = tmp;
        return tmp;
    }
    return NULL;
}

void ph_free(uintptr_t ptr)
{
    if(!ptr || ptr>HEAP_END || ptr<HEAP_BEGIN) return;
    
    heap_hdr_mdata_t* hdr = ptr-HDR_SIZE;
    hdr->free = true;

    if(hdr==HEAP_BEGIN) { // reset the free head
        hdr->next=free_head;
        free_head=hdr;
        return;
    }
    // else
    heap_ftr_mdata_t* prev_ftr = hdr-FTR_SIZE;
    heap_hdr_mdata_t* prev_hdr = prev_ftr->cur_hdr;
    while(prev_hdr && hdr && prev_hdr)

    // combine consecutive free slots
    // heap_ftr_mdata_t* prev_ftr = hdr-FTR_SIZE;
    // heap_hdr_mdata_t* prev_hdr = prev_ftr->cur_hdr;
    // while(prev_hdr && hdr && prev_hdr->free) {
    //     // prev chnk is free, reassign prev chnk's attributes to cur chnk
    //     prev_hdr->next = hdr->next;
    //     prev_hdr->prev = hdr->prev;
    //     prev_hdr->size += hdr->size+HDR_SIZE+FTR_SIZE;
    //     prev_hdr+HDR_SIZE+


    //     prev_ftr=prev_hdr-FTR_SIZE;
    //     hdr=prev_hdr;
    //     prev_hdr=prev_ftr->cur_hdr;
    // }

    // find prev slot that is free
    // find next slot that is free
    // add the chnk between them
}

void ph_mem_initialize(uintptr_t heap_beg, uintptr_t heap_end)
{
    memset((void*)DMA_BEGIN, '\0',(size_t)FRAME_SIZE);
    memset((void*)heap_beg,'\0',(size_t)(heap_end-heap_beg));
    HDR_SIZE = sizeof(heap_hdr_mdata_t);
    FTR_SIZE = sizeof(heap_ftr_mdata_t);
    
    // assign first free slot as entire heap
    HEAP_BEGIN = heap_beg; HEAP_END = heap_end;
    heap_hdr_mdata_t* hdr = HEAP_BEGIN; hdr->next = NULL; hdr->prev = NULL;
    hdr->free = true;
    hdr->size = (size_t)(heap_end-heap_beg-HDR_SIZE-FTR_SIZE);
    heap_ftr_mdata_t* ftr= HEAP_END - FTR_SIZE;
    ftr->cur_hdr = hdr;
    free_head = hdr;
}