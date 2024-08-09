#include "virt_mem_mgr.h"
#include "phy_mem_mgr.h"

// void* virt_page_alloc(void* pt, uint16_t entry)
// {
//     if(entry<=1023) {
//         uintptr_t frame = ph_frame_alloc();
//         return (void*)(VIRT_MEM_BEGIN + entry*PAGE_SIZE);
//     }
// }

void virt_page_free(void* page) // needed??
{

}

void* malloc()
{

}

void free(void* ptr)
{

}

void* get_new_pd()
{
    return (void*)ph_frame_alloc();
}

void* get_new_pt()
{
    uint32_t* pt = (uint32_t*)ph_frame_alloc(); // allocate single page table, 4mb size VAS
    for(uint16_t i=0;i<1024;i++) {
        pt[i]= ph_frame_alloc() | 7; // returns 4KB aligned frame; user,r/w,present
    }
    return (void*)pt;
}