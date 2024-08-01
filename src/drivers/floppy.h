#pragma once
#include <stdint.h>

// pass the starting address of DMA region
void fpc_init(char* dmabuf);
// IRQ handler for FPC
void fpc_irq();
// pass address of buffer, linear block address (0-511 -> 0, 512-1023 -> 1), length to read
void fpc_read(char** buf, uint32_t lba, uint16_t len);
// void floppy_write(char* buf, uint32_t lba, uint32_t sz);