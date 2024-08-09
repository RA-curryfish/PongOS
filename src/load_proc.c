#include "load_proc.h"
#include "virt_mem_mgr.h"

// static uint16_t pd_idx=1;
extern void load_page_dir(uint32_t* pd);
extern uint32_t* get_cur_pd(); 
void load_txt()
{

}

void load_elf()
{

}

void load(file_t* f)
{
	// f is either text file or executable
    if(f->type == DEVICE ) { // add exe?
        uint32_t* pd = get_new_pd();
        uintptr_t pt = (uintptr_t)get_new_pt(); // in a loop maybe?
        pd[0] = pt | 7; // supervisor, r/w, present
        
        void* vas_start = 0x0;
        char* buf=vas_start;const uint16_t buf_len=512; // size of each block in floppy
        open(f);
    }
    if(f->type == FILE) {
        char* buf = (char*)virt_page_alloc(); // return a vritual address here
        char* buf_start = buf;
        const uint16_t buf_len=512; // size of each block in floppy
        uint16_t idx=0; // logical block address
        size_t bytes_read=0, total_bytes_read=4096;
        open(f);
        do {
            bytes_read = read(f,buf,idx++,buf_len);  // reads raw bytes      
            if(bytes_read<0) printf("PANIC\n");
            total_bytes_read -= bytes_read;
            buf += buf_len; // contiguous allocation in virtual space
        } while(total_bytes_read>0);
        for(uint16_t i=0;i<804;i++)
            printf("%c",buf_start[i]);

        // typedef void (*fptr)(void);
        // fptr ptr = (fptr)buf;
        // ptr();
    }
}