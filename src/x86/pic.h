#pragma once
#include<stdint.h>
// Programmable Interrupt Controller
// PIC1 and PIC2 chained together

void pic_init(uint8_t offpic1, uint8_t offpic2);
void pic_disable();
void pic_mask(uint8_t irq);
void pic_unmask(uint8_t irq);
void pic_end_interrupt(uint8_t irq);
uint16_t pic_read_reg(uint8_t cmd);
uint16_t pic_read_req_reg();
uint16_t pic_read_serv_reg();