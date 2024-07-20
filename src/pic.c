#include "pic.h"
#include "asm_helper.h"

#define PIC1_COMMAND_PORT 0x20
#define PIC2_COMMAND_PORT 0xA0
#define PIC1_DATA_PORT 0x21
#define PIC2_DATA_PORT 0xA1

#define ICW1 0x11 // [IC4ExpectedFlag|0|0|0|InitFlag|0|0|0]
#define PIC1_ICW3 0x4 // indicate pic1 has a slave at IRQ2 (0x04)
#define PIC2_ICW3 0x2 // indicate pic2 is a cascade (0x02)
#define ICW4 0x01 // [x86Mode|0|0|0|0|0|0|0]
#define PIC_CMD_END_OF_INTERRUPT 0x20
#define PIC_CMD_READ_IRR 0x0A // Interrupt request register: tells which IRQs are raised
#define PIC_CMD_READ_ISR 0x0B // In-service register: tells the IRQs being serviced by CPU

void pic_init(uint8_t offpic1, uint8_t offpic2)
{
    // ICW1 - init controllers
    outb(PIC1_COMMAND_PORT, ICW1);
    iowait();
    outb(PIC2_COMMAND_PORT, ICW1);
    iowait();

    // ICW2 - offsets
    outb(PIC1_DATA_PORT, offpic1);
    iowait();
    outb(PIC2_DATA_PORT, offpic2);
    iowait();

    // ICW3 - some other stuff lol
    outb(PIC1_DATA_PORT, PIC1_ICW3);
    iowait();
    outb(PIC2_DATA_PORT, PIC2_ICW3);
    iowait();

    // ICW4 - more other stuff
    outb(PIC1_DATA_PORT, ICW4);
    iowait();
    outb(PIC2_DATA_PORT, ICW4);
    iowait();

    // Clear data ports
    outb(PIC1_DATA_PORT, 0);
    iowait();
    outb(PIC2_DATA_PORT, 0);
    iowait();
}

void pic_end_interrupt(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
    outb(PIC1_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
}

void pic_disable()
{
    outb(PIC1_DATA_PORT, 0xFF);
    iowait();
    outb(PIC2_DATA_PORT, 0xFF);
    iowait();
}

// to ignore a particular request line
void pic_mask(uint8_t irq)
{
    uint8_t port;

    if(irq < 8) port = PIC1_DATA_PORT;
    else {
        port = PIC2_DATA_PORT;
        irq -= 8;
    }

    uint8_t mask = inb(port);
    outb(port, mask | (1 << irq));
}

void pic_unmask(uint8_t irq)
{
    uint8_t port;

    if (irq < 8) port = PIC1_DATA_PORT;
    else {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = inb(port);
    outb(port,  mask & ~(1 << irq));
}

// lower 8 bits from PIC1, upper from PIC2
uint16_t pic_read_reg(uint8_t cmd)
{
    outb(PIC1_COMMAND_PORT, cmd);
    outb(PIC2_COMMAND_PORT, cmd);
    return ((uint16_t)inb(PIC1_COMMAND_PORT)) | (((uint16_t)inb(PIC2_COMMAND_PORT)) << 8);
}

uint16_t pic_read_req_reg()
{
   return pic_read_reg(PIC_CMD_READ_IRR);
}

uint16_t pic_read_serv_reg()
{
   return pic_read_reg(PIC_CMD_READ_ISR);
}

