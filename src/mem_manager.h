#pragma once
#include "utils.h"

extern void load_page_dir(uint32_t* pd_addr);
extern void enable_paging();

uint32_t page_dir[1024] __attribute__((aligned(4096))); // change this to page frame allocator
uint32_t page_tab[1024] __attribute__((aligned(4096)));

void init_pd()
{
    for(size_t i=0; i<1024;i++) {
        page_dir[i] = 0x2; // supervisor, write enabled, not present
    }
}

void init_pt()
{
    for(size_t i=0;i<1024;i++) {
        page_tab[i] = (i*0x1000) | 3; // supervisor, r/w, present 
    }
}

void add_entry()
{
    page_dir[0] = ((uint32_t) page_tab) | 3;
    page_dir[1023] = ((uint32_t) page_dir) | 3;
}

void load_pd(uint32_t* page_dir_addr)
{
    load_page_dir(page_dir_addr);
    enable_paging();
}

// todo
void assign_page_dirs()
{

}

void assign_page_tabs()
{

}

void map_page()
{
    // OS needs to map pages for the relevant page tables
}