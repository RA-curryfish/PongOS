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
    return NULL;
}

void free(void* ptr)
{

}
