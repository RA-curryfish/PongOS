#pragma once
#include <stdint.h>

void floppy_init();
void floppy_irq();
// reads one sector?
void floppy_read(char* buf, uint32_t lda);
void floppy_write(char* buf, uint32_t lda, uint32_t sz);