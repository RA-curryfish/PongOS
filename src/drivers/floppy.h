#pragma once
#include <stdint.h>
#include <stddef.h>

// pass the starting address of DMA region
void fpc_init(char* dmabuf);
// IRQ handler for FPC
void fpc_irq();
/* pass address of buffer, linear block address (0-511 -> 0, 512-1023 -> 1), length to read
   returns num of bytes read
*/ 
int fpc_read(char** buf, size_t lba, size_t len);
int fpc_write(char* buf, size_t lba, size_t len);