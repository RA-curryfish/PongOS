#pragma once
#include "utils.h"

extern void load_page_dir(uint32_t* pd_addr);

uint32_t* allocate_page()
{
    // todo: find out a free 4kB slot and return its aligned address
    uint32_t* page __attribute__((aligned(4096)));
    return page;
}

uint32_t* init_pd()
{
    uint32_t* page_dir = allocate_page();    
    for(size_t i=0; i<1024;i++) {
        page_dir[i] = 0x2; // supervisor, write enabled, not present
    }
    return page_dir;
}

uint32_t* init_pt()
{
    uint32_t* page_tab = allocate_page();
    // for(size_t i=0;i<1024;i++) {
    //     page_tab[i] = (i*0x1000) | 3; // supervisor, r/w, present 
    //     // todo: call add_pt_entry????
    // }
    return page_tab;
}

void add_pd_entry(uint32_t* page_dir, size_t entry, uint32_t val)
{
    if(entry<=1023){
        val = 0xFFFFF000 & val; // lower 12 bits to be cleared
        val = 0b00100111 | val; // lower 8 bits flags set
        page_dir[entry] = val;
    }
}

void add_pt_entry(uint32_t* page_tab, size_t entry, uint32_t val)
{
    if(entry<=1023) {
        val = 0xFFFF000 & val; 
        val = val | 3; 
        page_tab[entry] = val;
    }
    // VGA_PRINT_HEX_4(page_tab);
}

void load_pd(uint32_t* page_dir)
{
    load_page_dir(page_dir);
}