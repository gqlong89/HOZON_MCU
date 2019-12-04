#ifndef _FLASH_H_
#define _FLASH_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLASH_PAGE_SIZE             (256U)
#define FLASH_SECTORE_SIZE          (4096U)
#define FLASH_SIZE                  (0x80000U)        /* 512kB*/
#define check_sum_size              (1)
#define FLASH_START_ADDR            (0x0U)
#define FLASH_END_ADDR              (FLASH_START_ADDR + FLASH_SIZE)

/* ------------------------------Flash Map begin------------------------------------------  */
/* Bootloader image */
#define BTL_IMG_START_ADDR          (0x0U)
#define BTL_IMG_START_SECTOR        (0)
#define BTL_IMG_LENGTH              (64 * 1024)

/* Bootloader parameter (not used)*/
#define BTL_PARA_START_ADDR         (BTL_IMG_START_ADDR + BTL_IMG_LENGTH)
#define BTL_PARA_START_SECTOR       ((BTL_PARA_START_ADDR - BTL_IMG_START_ADDR)/FLASH_SECTORE_SIZE)
#define BTL_PARA_LENGTH             (4 * 1024)

/*APP image*/
#define APP_IMG0_START_ADDR         (BTL_PARA_START_ADDR + BTL_PARA_LENGTH)
#define APP_IMG0_START_SECTOR       ((APP_IMG0_START_ADDR - BTL_IMG_START_ADDR)/FLASH_SECTORE_SIZE)
#define APP_IMG0_LENGTH             (192 * 1024)

#define FLASH_EEPROM_START_ADDR     (0x14000000U)

#define APP_PARA_START_ADDR         (FLASH_EEPROM_START_ADDR)
#define APP_PARA_LENGTH             (3 * 1024)

#define DEV_PARA_START_ADDR         (FLASH_EEPROM_START_ADDR + APP_PARA_LENGTH)
#define DEV_PARA_LENGTH             (1 * 1024)

#define UPGRADE_FLAG_RAM_ADDR       (DEV_PARA_START_ADDR)
#define UPGRADE_FLAG_RAM_USED       (0xaaaaaaaa)          // request upgrade from mpu
/* ------------------------------Flash Map end------------------------------------------  */

/* ------------------------------RAM Map begin------------------------------------------  */
#define BOARD_INFO_START            (0x20006F00)
#define BTLV_ADDR                   (BOARD_INFO_START)
#define HWV_ADDR                    (BTLV_ADDR + 64)
#define BOM_ADDR                    (HWV_ADDR + 32)
#define BOARD_INFO_END              (BOM_ADDR + 32)

#define WAKEUPSOURCE_ADDR_START     (0x20006F80)

#define RESET_FLAG_RAM_ADDR         (0x20006F90)
#define RESET_FLAG_RAM_INIT         (0xaaaaaaaa)          // reboot wait a moment 
#define RESET_FLAG_RAM_USED         (0x55555555)          // reboot not wait

#define DBG_TRACE_MEM_START         (0x20006FA0)
#define DBG_TRACE_MEM_LEN           (0x10)

//#define UPGRADE_FLAG_RAM_ADDR       (0x20006FB0)
//#define UPGRADE_FLAG_RAM_USED       (0xaa)          // request upgrade from mpu
#define UPGRADE_COUNT_RAM_ADDR      (0x20006FB8)
/* ------------------------------RAM Map end------------------------------------------  */

#include <sys/_stdint.h>

enum
{
    FLASH_SUCCESS = 1,
    FLASH_FAIL,
    FLASH_ERR_UNALIGNED,
    FLASH_ERR_INVAL_ADDR
};

void Flash_SE(unsigned int SectorAddr);

void Flash_ReadBuffer(unsigned char *pBuffer, unsigned int ReadAddr, unsigned int ReadNum);

int Flash_WriteBuffer(unsigned char *pBuffer, unsigned int ReadAddr, unsigned int ReadNum);

int Flash_WriteBufferDirect(unsigned char *pBuffer, unsigned int WriteAddr, unsigned int WriteNum);

void flash_init(void);

void flash_test(void);

void flash_eeprom_test(void);

unsigned int flash_checksum_read(unsigned int addr, unsigned char *buf, unsigned int num);

unsigned int flash_checksum_write(unsigned int addr, unsigned char *buf, unsigned int num);

void Flash_EEPROM_WriteBuffer(unsigned char *pBuffer, unsigned int WriteAddr,
                              unsigned int WriteNum);

void Flash_EEPROM_ReadBuffer(unsigned char *pBuffer, unsigned int ReadAddr, unsigned int ReadNum);

unsigned int flash_eeprom_checksum_read(unsigned int addr, unsigned char *buf, unsigned int num);

unsigned int flash_eeprom_checksum_write(unsigned int addr, unsigned char *buf, unsigned int num);

void Flash_CopyData(unsigned int dest_addr, unsigned int Src_addr, uint32_t size);

#if 0
    int Flash_WriteImage(unsigned char *pBuffer, unsigned int WriteAddr, unsigned int WriteNum);
#endif

#endif

