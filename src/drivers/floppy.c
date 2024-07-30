#include "floppy.h"
#define SECTORS_PER_TRACK 18

enum floppy_regs
{
   STATUS_REGISTER_A                = 0x3F0, // read-only
   STATUS_REGISTER_B                = 0x3F1, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4, // read-only
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

void lba_to_chs(uint32_t lba, chs_t* chs)
{
    chs->cyl    = lba / (2 * SECTORS_PER_TRACK);
    chs->head   = ((lba % (2 * SECTORS_PER_TRACK)) / SECTORS_PER_TRACK);
    chs->sector = ((lba % (2 * SECTORS_PER_TRACK)) % SECTORS_PER_TRACK + 1);
}

// uint8_t floppy_init()
// {
//     int flp_cmos;

//     /* Before anything, validate the floppy controller */
//     if (!flp_valid())
//     {
//         kernel_warning("No enchanted floppy controller detected. "
//                        "Floppy initialization aborted.");
//         return -1;
//     }

//     /* Check the drive */
//     flp_cmos = cmos_get_flp_status();
//     if (!flp_cmos)
//     {
//         kernel_warning("No floppy drive detected");
//         return -1;
//     }
    // else
    // {
    //     /* Determine the drive number */
    //     if (CMOS_DISKETTE_TYPE_DRIVE0(flp_cmos) == CMOS_DISKETTE_1M44)
    //     {
    //         flp.drive_nr = 0;
    //         flp.dor_select_reg = DOR_SEL_0;
    //         flp.dor_motor_reg = DOR_MOTOR_0;
    //         flp.msr_busy_bit = MSR_BUSY_0;
    //     }
    //     else if (CMOS_DISKETTE_TYPE_DRIVE1(flp_cmos) == CMOS_DISKETTE_1M44)
    //     {
    //         flp.drive_nr = 1;
    //         flp.dor_select_reg = DOR_SEL_1;
    //         flp.dor_motor_reg = DOR_MOTOR_1;
    //         flp.msr_busy_bit = MSR_BUSY_1;
    //     }
    //     else
    //     {
    //         kernel_warning("No suitable floppy drive detected");
    //         return -1;
    //     }
    // }

    // dma_struct_init(&flp.dma, 2);
    // dma_reg_channel(&flp.dma, SECTORS_PER_TRACK * 512);
    
    // ctrl_disable();
    // ctrl_enable();
    // ctrl_reset();
    // set_motor_on(WAIT_MOTOR_SPIN);
    // flp_recalibrate();
    // set_motor_off(NO_WAIT_MOTOR_SPIN);

//     return 0;
// }