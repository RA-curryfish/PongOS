#pragma once
#include <stdint.h>

typedef enum process_state {
STOPPED,
RUNNING,
WAIING,
}state;

typedef struct process_control_block {
    uint32_t* kernel_sp;
    uint32_t* pd;
    struct process_control_block* next_task;
    state task_state;
} pcb;

void init_kernel_task(pcb* block);
void create_task(pcb* block,uint32_t* pd);