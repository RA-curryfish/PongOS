#include "load_proc.h"
#include "phy_mem_mgr.h"
#include "libf.h"
#include "task.h"

void load_txt()
{

}

void load_elf()
{

}

void load(file_t* f, vas_t* vas)
{
	// f is either text file or executable
    if(f->type == DEVICE ) { // add exe?
        // uint32_t* pd = (uint32_t*)get_new_pd();
        // uint32_t* pt = (uint32_t*)get_new_pt(); // in a loop maybe?
    
        // pd[0] = (uint32_t)pt | 7; // supervisor, r/w, present
        
        vas->code_begin = (uint8_t*)ph_frame_alloc(); // 0x400000
        vas->global_begin = (uint8_t*)ph_frame_alloc(); // 0x401
        vas->heap_begin = (uint8_t*)ph_frame_alloc(); // 0x402
        vas->stack_begin = (uint8_t*)ph_frame_alloc()+0x1000; // 0x404, because stack grows downwards
        // pt[0] = (uint32_t)frame0 | 7;
        // pt[1] = (uint32_t)frame1 | 7;
        // pt[2] = (uint32_t)frame2 | 7;
        // pt[3] = (uint32_t)frame3 | 7;

        // code that enters and exits a functio
        // *((uint8_t*)vas->code_begin) = 0x55;
        // *(((uint8_t*)vas->code_begin)+1) = 0x89;
        // *(((uint8_t*)vas->code_begin)+2) = 0xe5;
        // *(((uint8_t*)vas->code_begin)+3) = 0xc9;
        // *(((uint8_t*)vas->code_begin)+4) = 0xc3;

        // code to infinite loop
        // *(uint8_t*)vas->code_begin = 0xe9;
        // *(((uint8_t*)vas->code_begin)+1) = 0x00;
        // *(((uint8_t*)vas->code_begin)+2) = 0x40;
        // *(((uint8_t*)vas->code_begin)+3) = 0x00;
        // *(((uint8_t*)vas->code_begin)+4) = 0x00;
        
        // copy device contents in allocated frames
        unsigned char* buf = vas->code_begin;  // convert pt[0], etc into phy addr
        const uint16_t buf_len=512;
        int bytes_read=0, total_bytes_read=0x1000;
        uint16_t idx=0;
        open(f); 
        do {
            bytes_read = read(f,buf,idx++,buf_len);  // reads raw bytes      
            if(bytes_read<0) printf("PANIC\n");
            total_bytes_read -= bytes_read;
            buf += buf_len; // ideally change frames after 4KB
        } while(total_bytes_read>0);
        
        *(unsigned char*)0x400009 = '\x40';
        uint32_t ptr = printf;
        uint32_t disp = ptr - 0x400010;
        unsigned char* call_inst = (unsigned char*)&disp;
        *(unsigned char*)0x40000c = call_inst[0];
        *(unsigned char*)0x40000d = call_inst[1];
        *(unsigned char*)0x40000e = call_inst[2];
        *(unsigned char*)0x40000f = call_inst[3];

        // for(int i=0x400000;i<=0x400020;i++){
        //     // if(*(unsigned char*)i != 0x00)
        //         printf("%x ", *(unsigned char*)i);
        // }
    }
    else if(f->type == FILE) {
        // char* buf = (char*)virt_page_alloc(); // return a vritual address here
        // char* buf_start = buf;
        // const uint16_t buf_len=512; // size of each block in floppy
        // uint16_t idx=0; // logical block address
        // size_t bytes_read=0, total_bytes_read=4096;
        // open(f);
        // do {
        //     bytes_read = read(f,buf,idx++,buf_len);  // reads raw bytes      
        //     if(bytes_read<0) printf("PANIC\n");
        //     total_bytes_read -= bytes_read;
        //     buf += buf_len; // contiguous allocation in virtual space
        // } while(total_bytes_read>0);
        // for(uint16_t i=0;i<804;i++)
        //     printf("%c",buf_start[i]);

        // typedef void (*fptr)(void);
        // fptr ptr = (fptr)buf;
        // ptr();
    }
}