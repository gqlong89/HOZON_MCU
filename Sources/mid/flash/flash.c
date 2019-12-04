#include <flash.h>
#include <flash_driver.h>
#include <interrupt_manager.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <s32_core_cm4.h>
#include <status.h>
#include <stdbool.h>
#include <string.h>
#include <S32K144.h>
#include <S32K144_features.h>
#include <wdg.h>

#define FLASH_TEST     0

#define FLASH_DEBUG   0

static flash_ssd_config_t flashSSDConfig;
static status_t ret;

/*!
 \brief Interrupt handler for Flash Command Complete event
 */
void CCIF_Handler(void)
{
    /* Disable Flash Command Complete interrupt */
    FTFx_FCNFG &= (~FTFx_FCNFG_CCIE_MASK);

    return;
}

/*!
 \brief Callback function for Flash operations
 */
START_FUNCTION_DEFINITION_RAMSECTION
void CCIF_Callback(void)
{
    /* Enable interrupt for Flash Command Complete */
    if ((FTFx_FCNFG & FTFx_FCNFG_CCIE_MASK) == 0u)
    {
        FTFx_FCNFG |= FTFx_FCNFG_CCIE_MASK;
    }
}
END_FUNCTION_DEFINITION_RAMSECTION

void Flash_SE(unsigned int SectorAddr)
{
    #if 0
    #ifdef S32K144_SERIES
    uint32_t ocmdr0, ocmdr1;
    ocmdr0 = MSCM->OCMDR[0u];
    ocmdr1 = MSCM->OCMDR[1u];
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    #endif /* S32K144_SERIES */
    #endif

    INT_SYS_DisableIRQGlobal();
    FLASH_DRV_EraseSector(&flashSSDConfig, SectorAddr, FLASH_SECTORE_SIZE);
    INT_SYS_EnableIRQGlobal();

    #if 0
    #ifdef S32K144_SERIES
    MSCM->OCMDR[0u] = ocmdr0;
    MSCM->OCMDR[1u] = ocmdr1;
    #endif /* S32K144_SERIES */
    #endif
}

void Flash_PageProgram(unsigned int addr, unsigned char *pBuf)
{
    #if 0
    #ifdef S32K144_SERIES
    uint32_t ocmdr0, ocmdr1;
    ocmdr0 = MSCM->OCMDR[0u];
    ocmdr1 = MSCM->OCMDR[1u];
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    #endif /* S32K144_SERIES */
    #endif

    INT_SYS_DisableIRQGlobal();
    FLASH_DRV_Program(&flashSSDConfig, addr, FLASH_PAGE_SIZE, pBuf);
    INT_SYS_EnableIRQGlobal();

    #if 0
    #ifdef S32K144_SERIES
    MSCM->OCMDR[0u] = ocmdr0;
    MSCM->OCMDR[1u] = ocmdr1;
    #endif /* S32K144_SERIES */
    #endif
}

static status_t Flash_Program(unsigned int addr, unsigned char *pBuf, uint32_t size)
{
    status_t ret;

    if ((size & (FEATURE_FLS_PF_BLOCK_WRITE_UNIT_SIZE - 1U)) != 0U)
    {
        printk(PRINTK_LEVEL_ERROR, " The size must be 8 consecutive bytes");
        return STATUS_ERROR;
    }

    #if 0
    #ifdef S32K144_SERIES
    uint32_t ocmdr0, ocmdr1;
    ocmdr0 = MSCM->OCMDR[0u];
    ocmdr1 = MSCM->OCMDR[1u];
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    #endif /* S32K144_SERIES */
    #endif

    INT_SYS_DisableIRQGlobal();
    ret = FLASH_DRV_Program(&flashSSDConfig, addr, size, pBuf);
    INT_SYS_EnableIRQGlobal();

    #if 0
    #ifdef S32K144_SERIES
    MSCM->OCMDR[0u] = ocmdr0;
    MSCM->OCMDR[1u] = ocmdr1;
    #endif /* S32K144_SERIES */
    #endif

    return ret;
}

void Flash_CopyData(unsigned int dest_addr, unsigned int Src_addr, uint32_t size)
{
    uint32_t num = 0;
    static uint32_t sectorNumber = 0;

    while (num < size)
    {
        if ((dest_addr + num) / FLASH_SECTORE_SIZE != sectorNumber)
        {
            sectorNumber = (dest_addr + num) / FLASH_SECTORE_SIZE;
            Flash_SE(sectorNumber * FLASH_SECTORE_SIZE);
        }

        if ((num + FLASH_PAGE_SIZE) <= size)
        {
            Flash_Program((dest_addr + num), (unsigned char *)(Src_addr + num),
                          FLASH_PAGE_SIZE);
        }
        else
        {
            Flash_Program((dest_addr + num), (unsigned char *)(Src_addr + num),
                          (size - num));
        }

        num += FLASH_PAGE_SIZE;
    }
}

static int Flash_Write_AutoEraseSector(uint32_t _uiWriteAddr, uint8_t *_pBuf,
                                       uint32_t _NumByteToWrite)
{
    uint8_t g_tQSpiBuf[4 * 1024];
    //uint8_t g_tQSpiBuf[256];
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    //uint16_t i, j;
    uint16_t i;
    uint8_t *_pStr = g_tQSpiBuf;

    secpos = _uiWriteAddr / 4096;
    secoff = _uiWriteAddr % 4096;
    secremain = 4096 - secoff;

    /* if the data is in one sector */
    if (_NumByteToWrite <= secremain)
    {
        secremain = _NumByteToWrite;
    }

    while (1)
    {
        //Flash_SE( TEMP_IMG_START_ADDR );
        //Flash_CopyData( TEMP_IMG_START_ADDR, ( secpos * 4096 ), 4 * 1024 );

        Flash_ReadBuffer(_pStr, (secpos * 4096), 4096);

        /* the data expect to write is different from the data saved in the secctor, save it */
        /*if( 0
            != memcmp( (void *) _pBuf, (unsigned char *) ( TEMP_IMG_START_ADDR + secoff ),
                secremain ) )*/
        if (0 != memcmp((void *)_pBuf,  _pStr + secoff, secremain))
        {
            /* check whether it is need to erase flash */
            for (i = 0; i < secremain; i++)
            {
                if (((unsigned char *)(secpos * 4096))[secoff + i] != 0XFF)
                {
                    break;
                }
            }

            if (i < secremain)
            {
                Flash_SE(secpos * 4096);
            }

            #if 0
            j = 0;

            while (j < 16)
            {
                Flash_ReadBuffer(_pStr, (TEMP_IMG_START_ADDR + (j * 256)), 256);

                if ((j * 256 <= secoff) && ((j + 1) * 256 > secoff))
                {
                    if ((j + 1) * 256 >= (secoff + secremain))
                    {
                        for (i = 0; i < secremain; i++)
                        {
                            _pStr[i + secoff - j * 256] = _pBuf[i];
                        }
                    }
                    else
                    {
                        for (i = 0; i < ((j + 1) * 256 - secoff); i++)
                        {
                            _pStr[i + secoff - j * 256] = _pBuf[i];
                        }
                    }
                }
                else if ((j * 256 < (secoff + secremain))
                         && ((j + 1) * 256 >= (secoff + secremain)))
                {
                    for (i = 0; i < ((secoff + secremain) - (j * 256)); i++)
                    {
                        _pStr[i] = _pBuf[i];
                    }
                }
                else if ((j * 256 > secoff) && ((j + 1) * 256 < (secoff + secremain)))
                {
                    for (i = 0; i <= 255; i++)
                    {
                        _pStr[i] = _pBuf[i];
                    }
                }

                if (Flash_Program(((secpos * 4096) + j * 256), _pStr, 256) != STATUS_SUCCESS)
                {
                    return FLASH_FAIL;
                }

                j++;
            }

            driverWDGFeed();
            #endif
            #if 1

            for (i = 0; i < secremain; i++)
            {
                _pStr[i + secoff] = _pBuf[i];
            }

            driverWDGFeed();

            if (Flash_Program((secpos * 4096), _pStr, 4096) != STATUS_SUCCESS)
            {
                return FLASH_FAIL;
            }

            #endif
        }

        if (_NumByteToWrite == secremain)
        {
            break;
        }
        else
        {
            secpos++;
            secoff = 0;

            _pBuf += secremain;
            _uiWriteAddr += secremain;
            _NumByteToWrite -= secremain;

            if (_NumByteToWrite > 4096)
            {
                secremain = 4096;
            }
            else
            {
                secremain = _NumByteToWrite;
            }
        }
    }

    return FLASH_SUCCESS;
}

#if 0
int Flash_WriteImage(unsigned char *pBuffer, unsigned int WriteAddr, unsigned int WriteNum)
{
    //Flash_SE( TEMP_IMG_START_ADDR );
    //Flash_Program(TEMP_IMG_START_ADDR, pBuffer, 4096);
    #if 1
    uint8_t g_tQSpiBuf[256];
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *_pStr = g_tQSpiBuf;

    secpos = WriteAddr / 4096;
    secoff = WriteAddr % 4096;
    secremain = 4096 - secoff;

    Flash_ReadBuffer(_pStr, WriteAddr, WriteNum);

    if (0 != memcmp((void *)pBuffer,  _pStr, WriteNum))
    {
        /* check whether it is need to erase flash */
        for (i = 0; i < secremain; i++)
        {
            if (((unsigned char *)(secpos * 4096))[secoff + i] != 0XFF)
            {
                break;
            }
        }

        if (i < secremain)
        {
            Flash_SE(secpos * 4096);
        }

        if (Flash_Program(WriteAddr, pBuffer, WriteNum) != STATUS_SUCCESS)
        {
            return FLASH_FAIL;
        }

    }

    return FLASH_SUCCESS;
    #endif
}
#endif

int Flash_WriteBuffer(unsigned char *pBuffer, unsigned int WriteAddr, unsigned int WriteNum)
{
    return Flash_Write_AutoEraseSector(WriteAddr, pBuffer, WriteNum);
}

int Flash_WriteBufferDirect(unsigned char *pBuffer, unsigned int WriteAddr, unsigned int WriteNum)
{
    return  Flash_Program(WriteAddr, pBuffer, WriteNum);
}

void Flash_ReadBuffer(unsigned char *pBuffer, unsigned int ReadAddr, unsigned int ReadNum)
{
    memcpy(pBuffer, (unsigned char *) ReadAddr, ReadNum);
}

void Flash_ReadPage(uint32_t *pBuffer, uint32_t ReadAddr, uint32_t ReadNum)
{
    memcpy(pBuffer, (uint32_t *) ReadAddr, ReadNum);
}

int flash_real_read(unsigned int address, unsigned char *data, unsigned int len)
{
    Flash_ReadBuffer((unsigned char *) data, address, len);

    return FLASH_SUCCESS;
}

unsigned int flash_checksum_read(unsigned int addr, unsigned char *buf, unsigned int num)
{
    unsigned int i;
    unsigned char cs = 0;

    Flash_ReadBuffer(buf, addr, num);

    for (i = 0; i < (num - 1); i++)
    {
        cs += buf[i];
    }

    if (buf[num - 1] != cs)
    {
        printk(PRINTK_LEVEL_DEBUG, "Data is corruptted on address (0x%x)! ", addr);
        return 1;
    }

    return 0;
}

unsigned int flash_checksum_write(unsigned int addr, unsigned char *buf, unsigned int num)
{
    unsigned int i;
    unsigned char cs = 0;

    for (i = 0; i < (num - 1); i++)
    {
        cs += buf[i];
    }

    buf[num - 1] = cs;

    Flash_WriteBuffer(buf, addr, num);

    printk(PRINTK_LEVEL_DEBUG, "flash_checksum_write on address (0x%x)! ", addr);

    return 0;
}

void Flash_EEPROM_WriteBuffer(unsigned char *pBuffer, unsigned int WriteAddr,
                              unsigned int WriteNum)
{
    #if 0
    #ifdef S32K144_SERIES
    uint32_t ocmdr0, ocmdr1;
    ocmdr0 = MSCM->OCMDR[0u];
    ocmdr1 = MSCM->OCMDR[1u];
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    #endif /* S32K144_SERIES */
    #endif

    FLASH_DRV_EEEWrite(&flashSSDConfig, WriteAddr, WriteNum, pBuffer);

    #if 0
    #ifdef S32K144_SERIES
    MSCM->OCMDR[0u] = ocmdr0;
    MSCM->OCMDR[1u] = ocmdr1;
    #endif /* S32K144_SERIES */
    #endif
}

void Flash_EEPROM_ReadBuffer(unsigned char *pBuffer, unsigned int ReadAddr, unsigned int ReadNum)
{
    memcpy(pBuffer, (unsigned char *) ReadAddr, ReadNum);
}

unsigned int flash_eeprom_checksum_read(unsigned int addr, unsigned char *buf, unsigned int num)
{
    unsigned int i;
    unsigned char cs = 0;

    Flash_EEPROM_ReadBuffer(buf, addr, num);

    for (i = 0; i < (num - 1); i++)
    {
        cs += buf[i];
    }

    if (buf[num - 1] != cs)
    {
        printk(PRINTK_LEVEL_LOG, "Data is corruptted on address (0x%x)! ", addr);
        return 1;
    }

    return 0;
}

unsigned int flash_eeprom_checksum_write(unsigned int addr, unsigned char *buf, unsigned int num)
{
    unsigned int i;
    unsigned char cs = 0;

    for (i = 0; i < (num - 1); i++)
    {
        cs += buf[i];
    }

    buf[num - 1] = cs;

    Flash_EEPROM_WriteBuffer(buf, addr, num);

    printk(PRINTK_LEVEL_DEBUG, "flash_eeprom_checksum_write on address (0x%x)! ", addr);

    return 0;
}

#if 0
void flash_test(void)
{
    int i;
    unsigned char buff_test[256];

    for (i = 0; i < 256; i++)
    {
        buff_test[i] = 0X55;
    }

    //Flash_SE(VEHI_FAULT_START_ADDR);
    for (i = 0; i < 5000;)
    {
        Flash_WriteBuffer(buff_test, VEHI_FAULT_START_ADDR + i, 256);
        i += 256;
    }
}

void flash_eeprom_test(void)
{
    int i;
    unsigned char buff_test1[1024];
    unsigned char buff_test2[1024];
    int t1, t2, t3;

    for (i = 0; i < 1024; i++)
    {
        buff_test1[i] = i % 255;
    }

    t1 = OSIF_GetMilliseconds();

    Flash_EEPROM_WriteBuffer(buff_test1, 0x14000800, 1024);

    t2 = OSIF_GetMilliseconds();

    Flash_EEPROM_ReadBuffer(buff_test2, 0x14000800, 1024);

    t3 = OSIF_GetMilliseconds();

    for (i = 0; i < 1024; i++)
    {
        if (buff_test1[i] != buff_test2[i])
        {
            printk(PRINTK_LEVEL_ERROR, "flash_eeprom_test ERROR (%d)! ", i);
            return ;
        }
    }

    printk(PRINTK_LEVEL_ERROR, "flash_eeprom_test t1: (%d), t2: (%d), t3: (%d)", t1, t2, t3);
    printk(PRINTK_LEVEL_ERROR, "flash_eeprom_test write 1K time (%d)! ", (t2 - t1));
    printk(PRINTK_LEVEL_ERROR, "flash_eeprom_test read 1K time (%d)! ", (t3 - t2));
}
#endif

status_t flash_eeprom_init(void)
{
    flash_user_config_t flash_InitConfig =
    {
        .PFlashBase  = 0x00000000U,
        .PFlashSize  = 0x00080000U,
        .DFlashBase  = 0x10000000U,
        .EERAMBase   = 0x14000000U,
        /* If using callback, any code reachable from this function must not be placed in a Flash block targeted for a program/erase operation.*/
        .CallBack    = NULL_CALLBACK
    };

    if (flashSSDConfig.EEESize == 0u)
    {
        /* Configure FlexRAM as EEPROM and FlexNVM as EEPROM backup region,
         * DEFlashPartition will be failed if the IFR region isn't blank.
         * Refer to the device document for valid EEPROM Data Size Code
         * and FlexNVM Partition Code. For example on S32K144:
         * - EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
         * - DEPartitionCode = 0x08u: EEPROM backup size = 64 Kbytes */
        ret = FLASH_DRV_DEFlashPartition(&flashSSDConfig, 0x02u, 0x08u, 0x0u, false);

        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }
        else
        {
            /* Re-initialize the driver to update the new EEPROM configuration */
            ret = FLASH_DRV_Init(&flash_InitConfig, &flashSSDConfig);

            if (ret != STATUS_SUCCESS)
            {
                return ret;
            }

            /* Make FlexRAM available for EEPROM */
            ret = FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);

            if (ret != STATUS_SUCCESS)
            {
                return ret;
            }
        }
    }
    else /* FLexRAM is already configured as EEPROM */
    {
        /* Make FlexRAM available for EEPROM, make sure that FlexNVM and FlexRAM
         * are already partitioned successfully before */
        ret = FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);

        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }
    }

    return ret;
}

void flash_init(void)
{
    flash_user_config_t flash_InitConfig =
    {
        .PFlashBase  = 0x00000000U,
        .PFlashSize  = 0x00080000U,
        .DFlashBase  = 0x10000000U,
        .EERAMBase   = 0x14000000U,
        /* If using callback, any code reachable from this function must not be placed in a Flash block targeted for a program/erase operation.*/
        .CallBack    = NULL_CALLBACK
    };

    #if 0
    /* Disable cache to ensure that all flash operations will take effect instantly,
     * this is device dependent */
    #ifdef S32K144_SERIES
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[2u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[3u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    #endif /* S32K144_SERIES */
    #endif

    /* Install interrupt for Flash Command Complete event */
    INT_SYS_InstallHandler(FTFC_IRQn, CCIF_Handler, (isr_t *) 0);
    INT_SYS_EnableIRQ(FTFC_IRQn);

    /* Enable global interrupt */
    INT_SYS_EnableIRQGlobal();

    FLASH_DRV_Init(&flash_InitConfig, &flashSSDConfig);

    flash_eeprom_init();
}

