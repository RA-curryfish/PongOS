// Hardware Abstraction Layer
#pragma once
#include<stdint.h>
void init_hal(uintptr_t dma_beg, uintptr_t u_mem_beg, void* kernel_task);