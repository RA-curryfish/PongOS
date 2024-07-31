#include "floppy.h"
#include "../asm_helper.h"
#include <stdbool.h>
#define SECTORS_PER_TRACK 18

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

static volatile floppy_state=0;

void floppy_motor(bool switch_on)
{
    if(switch_on) {
        if(floppy_state==0) {
            outb(DIGITAL_OUTPUT_REGISTER, 0x1c);
            for(uint16_t i=0; i<10000; i++) iowait();
        }
        floppy_state=1;
    }
    else {
        if(floppy_state=1) {
            outb(DIGITAL_OUTPUT_REGISTER, 0xc);
            for(uint16_t i=0; i<10000; i++) iowait();
        }
        floppy_state=0;
    }
}

void floppy_wait()
{
    while(true) {
        uint8_t msr = inb(MAIN_STATUS_REGISTER);
        if(0b10000000 & msr) return;
    }
}

void lba_to_chs(uint32_t lba, chs_t* chs)
{
    chs->cyl    = lba / (2 * SECTORS_PER_TRACK);
    chs->head   = ((lba % (2 * SECTORS_PER_TRACK)) / SECTORS_PER_TRACK);
    chs->sector = ((lba % (2 * SECTORS_PER_TRACK)) % SECTORS_PER_TRACK + 1);
}

void floppy_cmd(enum data_cmd cmd, enum floppy_regs reg) 
{ 
    floppy_wait();
    outb(reg, cmd); 
} 

void floppy_init()
{   
    // turn on motor 0, enable IRQs?, set normal op
    floppy_motor(true);

    // enable perpendicular mode
    floppy_cmd(CMD_PERPENDICULAR_MODE, DATA_FIFO);
    floppy_cmd(1<<2, DATA_FIFO); // drive 0 enable
    floppy_cmd(0x03, DATARATE_SELECT_REGISTER); // use 2.88M floppy
}

void floppy_seek()
{
    outb(DATA_FIFO, CMD_SEEK);
}

void floppy_read(void* buf, uint32_t dev_loc, uint32_t sz)
{    
    // seek stuff 
    if(!floppy_state) floppy_motor(true);
    floppy_seek();
    chs_t chs;
    lba_to_chs(dev_loc, &chs);


    // issue sense interrupt cmd

    // issue std r/w data cmd

    // wait for IRQ6 to determine when controller wants data to be read
    // or check RQM bit in MSR
}