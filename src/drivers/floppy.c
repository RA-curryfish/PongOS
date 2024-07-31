#include "floppy.h"
#include "../asm_helper.h"
#include <stdbool.h>
#include "../drivers/terminal.h"
#define SECTORS_PER_TRACK 18
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

void floppy_wait(uint8_t bit, bool set)
{
    while(true) {
        uint8_t msr = inb(MAIN_STATUS_REGISTER);
        if(set) if(bit & msr) return; // bit should be 1 
        else if(!(bit & msr)) return; // bit should be 0
    }
}

void lba_to_chs(uint32_t lba, chs_t* chs)
{
    chs->cyl    = lba / (2 * SECTORS_PER_TRACK);
    chs->head   = ((lba % (2 * SECTORS_PER_TRACK)) / SECTORS_PER_TRACK);
    chs->sector = ((lba % (2 * SECTORS_PER_TRACK)) % SECTORS_PER_TRACK + 1);
}

void floppy_send_cmd(enum data_cmd cmd, enum floppy_regs reg) 
{ 
    floppy_wait(1<<7, true);
    outb(reg, cmd); 
} 

uint8_t floppy_read_result(enum floppy_regs reg)
{
    floppy_wait(1<<7, true);
    return inb(reg);
}

void floppy_irq()
{
    printchar('y');
}

void floppy_init()
{   
    // enable perpendicular mode
    floppy_send_cmd(CMD_PERPENDICULAR_MODE, DATA_FIFO);
    floppy_send_cmd(1<<2, DATA_FIFO); // drive 0 enable
    floppy_send_cmd(0x03, DATARATE_SELECT_REGISTER); // use 2.88M floppy
}

void floppy_seek(uint8_t cyl)
{
    if(!floppy_state) floppy_motor(true); // turn on motor if not already

    floppy_send_cmd(CMD_SEEK, DATA_FIFO);
    floppy_send_cmd(0<<2|0, DATA_FIFO); // head 0, drive 0
    floppy_send_cmd(cyl, DATA_FIFO);
    floppy_wait(1<<0, false); // check disk active bit to be unset?
    
    // send sense interrupt cmd
    floppy_send_cmd(CMD_SENSE_INTERRUPT, DATA_FIFO);
    do {
        uint8_t st0 = floppy_read_result(DATA_FIFO);
        uint8_t cylinder = floppy_read_result(DATA_FIFO);
        if (cyl==cylinder) break;
    } while (true); // should idealy not run more than ocne

    floppy_motor(false); //turn off motor
}

void floppy_read(void* buf, uint32_t lba, uint32_t sz)
{    
    // need to add retries
    // seek stuff 
    chs_t chs;
    lba_to_chs(lba, &chs);
    floppy_seek(chs.cyl);
    
    // issue std r/w data cmd
    floppy_send_cmd(CMD_READ_DATA|MT|MFM,DATA_FIFO);
    floppy_send_cmd((0<<2)|0, DATA_FIFO); // head num, drive num
    floppy_send_cmd(chs.cyl, DATA_FIFO); //cyl
    floppy_send_cmd(0, DATA_FIFO); //head
    floppy_send_cmd(chs.sector, DATA_FIFO); // count starts from 1
    floppy_send_cmd(2, DATA_FIFO);
    floppy_send_cmd(SECTORS_PER_TRACK, DATA_FIFO); // end of tracks
    floppy_send_cmd(0x1B, DATA_FIFO); //GAP size
    floppy_send_cmd(0xFF, DATA_FIFO);
    floppy_wait(1<<7, true);

    // status, ending chs, bytes per sec vals
    uint8_t st0,st1,st2, cyl_r, hd_r, sec_r, bps;
    st0 = floppy_read_result(DATA_FIFO);
    st1 = floppy_read_result(DATA_FIFO);
    st2 = floppy_read_result(DATA_FIFO);

    cyl_r = floppy_read_result(DATA_FIFO);
    hd_r = floppy_read_result(DATA_FIFO);
    sec_r = floppy_read_result(DATA_FIFO);
    bps = floppy_read_result(DATA_FIFO); // should be 2??
}