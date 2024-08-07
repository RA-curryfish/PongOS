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

static uintptr_t DMA_BEGIN;
static uintptr_t DMA_END;
static uintptr_t HEAP_BEGIN;
static uintptr_t HEAP_END;
static uintptr_t UMEM_BEGIN;
static uintptr_t UMEM_END;
static uint8_t UMEM_BITMAP_SZ;
static uint8_t UMEM_BITMAP[128]; //max size of bitmap is 128 -> 128*8 = 1024 pages
static size_t HDR_SIZE = sizeof(heap_hdr_mdata_t);
static size_t FTR_SIZE = sizeof(heap_ftr_mdata_t);
static heap_hdr_mdata_t* free_head;
// 0000 0000 01 00 0000 0000 0000 0000 0000
//      00     4      00          000       ===> 0x400000

uintptr_t ph_page_alloc()
{
    uintptr_t ret = (uintptr_t)NULL;
    uint8_t frame_num = 0;
    bool found = false;
    uint16_t i;
    for(i =0;i<UMEM_BITMAP_SZ;i++)
    {
        if((UMEM_BITMAP[i]&(1<<frame_num)) == 0) {
            found = true;
            break;
        }
        if(frame_num==7) frame_num=255;
        frame_num++;
    }
    if(!found) return ret;
    UMEM_BITMAP[i] = UMEM_BITMAP[i] | (1<<frame_num);
    ret = (uintptr_t)(UMEM_BEGIN + (8*i + frame_num)*FRAME_SIZE);
    // ideally should also map this page in the page table of the app
    return ret;
}
// 
// void ph_page_free(uintptr_t ptr)
// {
//     uint16_t frame_num = (uint16_t)((uint16_t*)ptr-(uint16_t*)MEM_BASE_ADDR)/FRAME_SIZE;
//     uint16_t idx = frame_num/8;
//     frame_num = frame_num%8;
//     mem_bitmap[idx] = mem_bitmap[idx] & ~(1<<frame_num);
// }
// 
// uint8_t get_bitmap(uint8_t idx)
// {
//     return mem_bitmap[idx];
// }

void* ph_malloc(size_t sz)
{
    heap_hdr_mdata_t* tmp = free_head;
    while(tmp && (void*)((void*)tmp+sz+HDR_SIZE+FTR_SIZE)<HEAP_END) { // go thru free list
        if(tmp->size < sz) {
            tmp = tmp->next;
            continue;
        }
        else if(tmp->size > sz) { // split block
            heap_hdr_mdata_t* new_free = (void*)tmp+HDR_SIZE+sz+FTR_SIZE;
            new_free->free = true;
            new_free->prev=tmp->prev;
            new_free->next=tmp->next;
            new_free->size=tmp->size-sz-HDR_SIZE-FTR_SIZE;
            heap_ftr_mdata_t* ftr=(void*)tmp+tmp->size;
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
        heap_ftr_mdata_t* ftr = (void*)tmp+HDR_SIZE+sz;
        ftr->cur_hdr = tmp;

        return (void*)((void*)tmp+HDR_SIZE);
    }
    return NULL;
}

void ph_free(uintptr_t ptr)
{
    if(!ptr || ptr>HEAP_END || ptr<HEAP_BEGIN) return;
    
    heap_hdr_mdata_t* hdr = (void*)ptr-HDR_SIZE;
    hdr->free = true;

    // combine consecutive free slots
    heap_ftr_mdata_t* prev_ftr = (void*)hdr-FTR_SIZE;
    heap_hdr_mdata_t* prev_hdr = prev_ftr->cur_hdr;
    while(prev_hdr && prev_hdr>=free_head && hdr && prev_hdr->free) {
        // prev chnk is free, combine chnks
        prev_hdr->next = hdr->next;
        prev_hdr->prev = hdr->prev;
        heap_ftr_mdata_t* ftr = (void*)hdr+HDR_SIZE+hdr->size;
        ftr->cur_hdr = prev_hdr;
        prev_hdr->size += hdr->size+HDR_SIZE+FTR_SIZE;
        
        // go backwards
        hdr=prev_hdr;
        prev_ftr=(void*)hdr-FTR_SIZE;
        prev_hdr=prev_ftr->cur_hdr;
    }

    if((uintptr_t)hdr==HEAP_BEGIN || hdr<free_head) { // reset the free head
        hdr->next=free_head;
        free_head=hdr;
        return;
    }
    // else
    // add the chnk between two frees
    heap_hdr_mdata_t* tmp = free_head;
    while(tmp) {
        if(tmp<hdr && tmp->next>hdr) { // hdr exists between
            hdr->prev = tmp;
            hdr->next = tmp->next;

            tmp->next = hdr;
            hdr->next->prev = hdr;
            break;
        }
        tmp = tmp->next;
    }
}

void k_heap_initialize(uintptr_t heap_beg, uintptr_t heap_end)
{
    memset((void*)heap_beg,'\0',(size_t)(heap_end-heap_beg));
    FTR_SIZE = sizeof(heap_ftr_mdata_t);
    
    // assign first free slot as entire heap
    HEAP_BEGIN = heap_beg; HEAP_END = heap_end;
    heap_hdr_mdata_t* hdr = (uintptr_t)HEAP_BEGIN; hdr->next = NULL; hdr->prev = NULL;
    hdr->free = true;
    hdr->size = (size_t)(heap_end-heap_beg-HDR_SIZE-FTR_SIZE);
    heap_ftr_mdata_t* ftr= (uintptr_t)(HEAP_END - FTR_SIZE);
    ftr->cur_hdr = hdr;
    free_head = hdr;
}

void ph_mem_initialize(uintptr_t dma_beg, uintptr_t u_mem_beg)
{
    DMA_BEGIN = dma_beg; DMA_END=DMA_BEGIN+0x100000-1; //1MB of DMA
    UMEM_BEGIN = u_mem_beg; UMEM_END=UMEM_BEGIN+0x2000000-1; //32MB of userspace
    memset((void*)DMA_BEGIN, '\0',(size_t)FRAME_SIZE*16);
    memset((void*)UMEM_BEGIN, '\0',(size_t)FRAME_SIZE*16);
    UMEM_BITMAP_SZ = (uint32_t)(
        (((uint32_t)UMEM_END-(uint32_t)UMEM_BEGIN)/(uint32_t)FRAME_SIZE)
        /8); 
}

void load_file(file_t* f)
{
	char* buf = (char*)ph_page_alloc();
    uint16_t buf_len=FRAME_SIZE;
	open(f);
	read(f,buf,0,buf_len);
    printf(buf);

    typedef void (*fptr)(void);
    fptr ptr = (fptr)buf;
    ptr();
    printf("done\n");
}