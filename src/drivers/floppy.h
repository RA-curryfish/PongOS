#pragma once
#include <stdint.h>

void fpc_init(char* dmabuf);
void fpc_irq();
void fpc_read(char* buf, uint32_t lda);
// void floppy_write(char* buf, uint32_t lda, uint32_t sz);