#include "floppy.h"
#include "../asm_helper.h"
#include <stdbool.h>
#include "../drivers/terminal.h"
#define SECTORS_PER_TRACK 18
#define RQM 0x80
#define NDMA 0x20
#define MT 0x80 // multi track mode
#define MFM 0x40 // magnetic shenannigan

enum floppy_regs
{
   STATUS_REGISTER_A                = 0x3F0, // read-only
   STATUS_REGISTER_B                = 0x3F1, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4, // read-only, check busy bitflag b4 r/w
   DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
   DATA_FIFO                        = 0x3F5,
   DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
   CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum data_cmd
{
    CMD_READ_TRACK =        0x02,  /* generates IRQ6 */
    CMD_SET_PARAM =         0x03,  /* set drive parameters */
    CMD_DRIVE_STATUS =      0x04,
    CMD_WRITE_DATA =        0x05,  /* write data to disk */
    CMD_READ_DATA =         0x06,  /* read data from disk */
    CMD_RECALIBRATE =       0x07,  /* seek to cylinder 0 */
    CMD_SENSE_INTERRUPT =   0x08,  /* ack IRQ6, get status of last cmd */
    CMD_WRITE_DELETED_DATA= 0x09,
    CMD_READ_ID =           0x0A,  /* generatess IRQ6 */
    CMD_READ_DELETED_DATA = 0x0C,
    CMD_FORMAT_TRACK =      0x0D,
    CMD_SEEK =              0x0F,  /* seek both heads to cylinder X */
    CMD_VERSION =           0x10,  /* used on init */
    CMD_SCAN_EQUAL =        0x11,
    CMD_PERPENDICULAR_MODE= 0x12,  /* used on init */
    CMD_CONFIGURE =         0x13,  /* set controller parameters */
    CMD_LOCK =              0x14,  /* protect controller parameters from reset */
    CMD_VERIFY =            0x16,
    CMD_SCAN_LOW_OR_EQUAL = 0x19,
    CMD_SCAN_HIGH_OR_EQUAL= 0x1D,

    /* When read FIFO register, this value indicates that
     * an invalid command was given on the previous write */
    CMD_ERROR = 0x80
};

typedef struct chs {
    uint8_t cyl;
    uint8_t head;
    uint8_t sector;
}chs_t;

static volatile uint8_t floppy_state=0;
static volatile bool floppy_irq_done = false;

#define DMA_LEN 0x4800
// set DMA in a 1:1 mapped area :) (maybe 1MB-2MB) 
static const char* floppy_dmabuf = 0x100000;
void floppy_dma_init(bool rw)
{
    union { 
        unsigned char b[4]; // 4 bytes 
        unsigned long l;    // 1 long = 32-bit 
    } a, c; // address and count 

    a.l = (unsigned) floppy_dmabuf; 
    c.l = (unsigned) DMA_LEN - 1; // -1 because of DMA counting 

    // check that address is at most 24-bits (under 16MB) 
    // check that count is at most 16-bits (DMA limit) 
    // check that if we add count and address we don't get a carry 
    // (DMA can't deal with such a carry, this is the 64k boundary limit) 
    if((a.l >> 24) || (c.l >> 16) || (((a.l&0xffff)+c.l)>>16)) { 
        printstr("floppy_dma_init: static buffer problem\n"); 
    } 

    unsigned char mode; 
    // 01:0:0:01:10 = single/inc/no-auto/to-mem/chan2 
    if(rw) mode = 0x46;
    else mode = 0x4a;
    // 01:0:0:10:10 = single/inc/no-auto/from-mem/chan2 

    outb(0x0a, 0x06);   // mask chan 2 

    outb(0x0c, 0xff);   // reset flip-flop 
    outb(0x04, a.b[0]); //  - address low byte 
    outb(0x04, a.b[1]); //  - address high byte 

    outb(0x81, a.b[2]); // external page register 

    outb(0x0c, 0xff);   // reset flip-flop 
    outb(0x05, c.b[0]); //  - count low byte 
    outb(0x05, c.b[1]); //  - count high byte 

    outb(0x0b, mode);   // set mode (see above) 

    outb(0x0a, 0x02);   // unmask chan 2 
}

void floppy_motor(bool switch_on)
{
    if(switch_on) {
        if(floppy_state==0) {
            outb(DIGITAL_OUTPUT_REGISTER, 0x1c);
            for(uint16_t i=0; i<60000; i++) iowait();
        }
        floppy_state=1;
    }
    else {
        if(floppy_state==1) {
            outb(DIGITAL_OUTPUT_REGISTER, 0xc);
            for(uint16_t i=0; i<60000; i++) iowait();
        }
        floppy_state=0;
    }
}

void floppy_wait(uint8_t bit, bool set)
{
    while(true) {
        uint8_t msr = inb(MAIN_STATUS_REGISTER);
        if(set) {if(bit & msr) return;} // bit should be 1 
        else {if(!(bit & msr)) return;} // bit should be 0
    }
}

void lba_to_chs(uint32_t lba, chs_t* chs)
{
    chs->cyl    = lba / (2 * SECTORS_PER_TRACK);
    chs->head   = ((lba % (2 * SECTORS_PER_TRACK)) / SECTORS_PER_TRACK);
    chs->sector = ((lba % (2 * SECTORS_PER_TRACK)) % SECTORS_PER_TRACK + 1);
}

void floppy_send_cmd(uint8_t cmd, uint16_t reg) 
{ 
    floppy_wait(RQM, true);
    outb(reg, cmd);
    floppy_wait(RQM, true);
} 

uint8_t floppy_read_result(uint16_t reg)
{
    floppy_wait(RQM, true);
    return inb(reg);
    floppy_wait(RQM, true);
}

void floppy_irq()
{
    printstr("IRQ6\n");
    floppy_irq_done=true;
}

void floppy_irq_wait()
{
    while(!floppy_irq_done);
    // floppy_irq_done=false;
}

void floppy_fetch_res(uint8_t* st0, uint8_t* cyl)
{
    floppy_send_cmd(CMD_SENSE_INTERRUPT, DATA_FIFO);
    *st0=floppy_read_result(DATA_FIFO);
    *cyl=floppy_read_result(DATA_FIFO);
}

void floppy_reset()
{
    outb(DIGITAL_OUTPUT_REGISTER,0);
    outb(DIGITAL_OUTPUT_REGISTER,0x0c); // 0b00001100
}

void floppy_configure()
{
    // specify command
    floppy_send_cmd(0,CONFIGURATION_CONTROL_REGISTER); // set datarate
    floppy_send_cmd(CMD_SET_PARAM, DATA_FIFO);
    floppy_send_cmd(0xdf, DATA_FIFO); // steprate, unload time
    floppy_send_cmd(0x2, DATA_FIFO); // load time, enable DMA
    printstr("specify done\n");

    // floppy_send_cmd(CMD_CONFIGURE, DATA_FIFO);
    // floppy_send_cmd(0, DATA_FIFO);
    // floppy_send_cmd((1<<6|0<<5|1<<4|0b1000), DATA_FIFO); //imp seek, !fifo_disable, polling_mode_disable, threshold 
    // floppy_send_cmd(0, DATA_FIFO);
    // no result, no irq
}

void floppy_init()
{   
    printstr("f Init\n");
    floppy_reset();
    floppy_irq_wait();
    floppy_configure();

    // recalibrate
    floppy_motor(true);
    floppy_send_cmd(CMD_RECALIBRATE, DATA_FIFO);
    floppy_send_cmd(0,DATA_FIFO);
    floppy_read_result(DATA_FIFO);
    if(floppy_read_result(DATA_FIFO)!=0) printstr("ERROR");
    floppy_motor(false);

    // lock settings
    floppy_send_cmd(CMD_LOCK, DATA_FIFO);
    printstr("f Init end\n");
}

void floppy_seek(chs_t chs)
{
    if(!floppy_state) floppy_motor(true); // turn on motor if not already
    printstr("f seek\n");
    floppy_irq_done=false;
    printstr("0");
    floppy_send_cmd(CMD_SEEK, DATA_FIFO);
    floppy_send_cmd(chs.head<<2, DATA_FIFO); // head 0, drive 0
    floppy_send_cmd(chs.cyl, DATA_FIFO);
    printstr("1");
    // floppy_irq_wait(); //??
    // floppy_wait(1<<0, false); // check disk active bit to be unset?
    floppy_irq_done=false;
    
    // send sense interrupt
    uint8_t st0,cyl;
    floppy_fetch_res(&st0,&cyl);
    printstr("2");
    if(cyl==chs.cyl) {
        floppy_motor(false); //turn off motor
        printstr("seek end\n");
        return;
    }
    // do {
    //     uint8_t st0 = floppy_read_result(DATA_FIFO);
    //     uint8_t cylinder = floppy_read_result(DATA_FIFO);
    //     if (cyl==cylinder) break;
    // } while (true); // should idealy not run more than ocne
    printstr("ERROR SEEK\n");
}

void floppy_read(char* buf, uint32_t lba)
{    
    // need to add retries
    // seek stuff 
    chs_t chs;
    lba_to_chs(lba, &chs);
    floppy_seek(chs); // needed??
    // chs.head=1;
    // floppy_seek(chs);
    // chs.head=0;
    
    floppy_motor(true);
    floppy_dma_init(0);
    printstr("f read\n");
    
    for(uint8_t i=0;i<250;i++) iowait();

    // issue std r/w data cmd
    floppy_irq_done=false;
    floppy_send_cmd(CMD_READ_DATA|MT|MFM, DATA_FIFO);
    floppy_send_cmd((chs.head<<2)|0, DATA_FIFO); // head num, drive num
    floppy_send_cmd(chs.cyl, DATA_FIFO); //cyl
    floppy_send_cmd(chs.head, DATA_FIFO); //head
    floppy_send_cmd(chs.sector, DATA_FIFO); // count starts from 1
    floppy_send_cmd(0x2, DATA_FIFO); //512bytes per sector
    floppy_send_cmd(SECTORS_PER_TRACK, DATA_FIFO); // end of tracks
    floppy_send_cmd(0x1B, DATA_FIFO); //GAP size
    floppy_send_cmd(0xFF, DATA_FIFO);
    floppy_irq_wait();
    floppy_irq_done=false;
    
    printstr("f read issued\n");

    // status, ending chs, bytes per sec vals
    uint8_t st0,st1,st2, cyl_r, hd_r, sec_r, bps;
    st0 = floppy_read_result(DATA_FIFO);
    st1 = floppy_read_result(DATA_FIFO);
    st2 = floppy_read_result(DATA_FIFO);

    cyl_r = floppy_read_result(DATA_FIFO);
    hd_r = floppy_read_result(DATA_FIFO);
    sec_r = floppy_read_result(DATA_FIFO);
    bps = floppy_read_result(DATA_FIFO); // should be 2??
    printchar(st0+'0');
    printchar(st1+'0');
    printchar(st2+'0');
    printchar(cyl_r+'0');
    printchar(hd_r+'0');
    printchar(sec_r+'0');
    printchar(bps+'0');
    int error = 0; 

        if(st0 & 0xC0) { 
            static const char * status[] = 
            { 0, "error", "invalid command", "drive not ready" };
            printstr("floppy_do_sector: status = ");
            printstr(status[st0>>6]);
            error = 1; 
        } 
        if(st1 & 0x80) { 
            printstr("floppy_do_sector: end of cylinder\n"); 
            error = 1; 
        } 
        if(st0 & 0x08) { 
            printstr("floppy_do_sector: drive not ready\n"); 
            error = 1; 
        } 
        if(st1 & 0x20) { 
            printstr("floppy_do_sector: CRC error\n"); 
            error = 1; 
        } 
        if(st1 & 0x10) { 
            printstr("floppy_do_sector: controller timeout\n"); 
            error = 1; 
        } 
        if(st1 & 0x04) { 
            printstr("floppy_do_sector: no data found\n"); 
            error = 1; 
        } 
        if((st1|st2) & 0x01) { 
            printstr("floppy_do_sector: no address mark found\n"); 
            error = 1; 
        } 
        if(st2 & 0x40) { 
            printstr("floppy_do_sector: deleted address mark\n"); 
            error = 1; 
        } 
        if(st2 & 0x20) { 
            printstr("floppy_do_sector: CRC error in data\n"); 
            error = 1; 
        } 
        if(st2 & 0x10) { 
            printstr("floppy_do_sector: wrong cylinder\n"); 
            error = 1; 
        } 
        if(st2 & 0x04) { 
            printstr("floppy_do_sector: uPD765 sector not found\n"); 
            error = 1; 
        } 
        if(st2 & 0x02) { 
            printstr("floppy_do_sector: bad cylinder\n"); 
            error = 1; 
        } 
        if(bps != 0x2) { 
            printstr("floppy_do_sector: wanted 512B/sector, got ");
            error = 1; 
        } 
        if(st1 & 0x02) { 
            printstr("floppy_do_sector: not writable\n"); 
            error = 2; 
        } 
}

// void floppy_write(char* buf, uint32_t dev_loc, uint32_t sz)
// {

// }