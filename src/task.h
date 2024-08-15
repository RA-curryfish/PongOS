#pragma once
#include <stdint.h>

typedef enum process_state {
    STOPPED=0,
    RUNNING,
    WAIING,
}state;

typedef struct process_control_block {
    uint32_t* kernel_sp;
    uint32_t* pd;
    struct process_control_block* next_task;
    state task_state;
} pcb;

void init_kernel_task(uint32_t* cur_task_addr, pcb* k_task);
void create_task(pcb* new_task,pcb* next_task,uint32_t* pd, void(*func)(), uint8_t* stack_begin);
extern void __attribute__((cdecl)) switch_task(pcb* old, pcb* new);