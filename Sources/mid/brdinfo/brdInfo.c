/*
 *  sample info from bootloader
 *
 *  BTL VER: btlver BTL.T4.06[A-2]\Feb 26 2016\18:03:37
 *  HW  VER: hwver IN.X15003M.S4
 *  BOM VER: bomver bom0001
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include "flash.h"

#define BTL_VER_STR_LOCATION (BTLV_ADDR)
#define HW_VER_STR_LOCATION  (HWV_ADDR)
#define BOM_VER_STR_LOCATION (BOM_ADDR)

static unsigned int s_BtlVerValid = 0;
static unsigned int s_HwVerValid = 0;
static unsigned int s_BomVerValid = 0;

/* 0: success 1: fail */
static unsigned int check_btl_ver_format(void)
{
    if (memcmp((void *) BTL_VER_STR_LOCATION, (void *) "btlver", 6) != 0)
    {
        return 1;
    }

    if (strlen((char *) BTL_VER_STR_LOCATION) >= 64)
    {
        return 1;
    }

    return 0;
}

static unsigned int check_hw_ver_format(void)
{
    if (memcmp((void *) HW_VER_STR_LOCATION, (void *) "hwver", 5) != 0)
    {
        return 1;
    }

    if (strlen((char *) HW_VER_STR_LOCATION) >= 32)
    {
        return 1;
    }

    return 0;
}

static unsigned int check_bom_ver_format(void)
{
    if (memcmp((void *) BOM_VER_STR_LOCATION, (void *) "bomver", 6) != 0)
    {
        return 1;
    }

    if (strlen((char *) BOM_VER_STR_LOCATION) >= 32)
    {
        return 1;
    }

    return 0;
}

void brd_info_init(void)
{
    if (check_btl_ver_format() == 0)
    {
        s_BtlVerValid = 1;
        //shellPrintf(" BTL VER: %s\r\n", (char *)BTL_VER_STR_LOCATION);
    }

    if (check_hw_ver_format() == 0)
    {
        s_HwVerValid = 1;
        //shellPrintf(" HW  VER: %s\r\n", (char *)HW_VER_STR_LOCATION);
    }

    if (check_bom_ver_format() == 0)
    {
        s_BomVerValid = 1;
        //shellPrintf(" BOM VER: %s\r\n", (char *)BOM_VER_STR_LOCATION);
    }
}

const char *brd_get_btlVerStr(void)
{
    if (s_BtlVerValid == 1)
    {
        return (char *)(BTL_VER_STR_LOCATION + 7);
    }
    else
    {
        return "unknown";
    }
}

const char *brd_get_hwVerStr(void)
{
    if (s_HwVerValid == 1)
    {
        return (char *)(HW_VER_STR_LOCATION + 6);
    }
    else
    {
        return (char *) 0;
    }
}

const char *brd_get_bomVerStr(void)
{
    if (s_BomVerValid == 1)
    {
        return (char *)(BOM_VER_STR_LOCATION + 7);
    }
    else
    {
        return (char *) 0;
    }
}

/* example: x15006, x15003, without leading 'x' or 'X' */
unsigned int brd_get_projectCode(void)
{
    unsigned int code = 0;
    unsigned int pStart = 0;
    unsigned int pEnd = 0;
    char prjCode[12];

    if (s_HwVerValid == 1)
    {
        memset(prjCode, 0, sizeof(prjCode));

        pStart = (unsigned int) strstr((char *) HW_VER_STR_LOCATION, "IN.");
        pEnd = (unsigned int) strstr((char *)(pStart + 3), ".");

        if ((pStart == 0) || (pEnd == 0))
        {
            return 0;
        }

        if ((pEnd > pStart) && ((pEnd - pStart) >= 5))
        {
            memcpy(prjCode, (unsigned char *)(pStart + 4), 5);
            sscanf(prjCode, "%u", &code);
        }

        //shellPrintf(" prj code: %u \r\n", code);
        return code;
    }
    else
    {
        return 0;
    }
}

/* example: S0, S1, S2 without leading 's' or 'S' */
unsigned int brd_get_boardRevision(void)
{
    unsigned int rev = 0;
    unsigned int pStart = 0;
    unsigned int pEnd = 0;
    char revision[12];

    if (s_HwVerValid == 1)
    {
        memset(revision, 0, sizeof(revision));

        pStart = (unsigned int) strstr((char *) HW_VER_STR_LOCATION, "IN.");
        pEnd = (unsigned int) strstr((char *)(pStart + 3), ".");

        if ((pStart == 0) || (pEnd == 0))
        {
            return 0;
        }

        memcpy(revision, (unsigned char *)(pEnd + 2), 1);
        sscanf(revision, "%u", &rev);

        //shellPrintf(" board revision: %u \r\n", rev);
        return rev;
    }
    else
    {
        return 0;
    }
}

/* 硬件明细信息的版本,如:IN.X15005.P1明细表.xls对应产品部分配的bom0001 */
unsigned int brd_get_bomRevision(void)
{
    unsigned int rev = 0;
    unsigned int pStart = 0;
    char revision[12];

    if (s_BomVerValid == 1)
    {
        memset(revision, 0, sizeof(revision));

        pStart = (unsigned int) strstr((char *) BOM_VER_STR_LOCATION, "bom");
        pStart += 1;
        pStart = (unsigned int) strstr((char *) pStart, "bom");

        if (pStart == 0)
        {
            return 0;
        }

        memcpy(revision, (unsigned char *)(pStart + 3), 4);
        sscanf(revision, "%u", &rev);

        //shellPrintf(" bom  revision: %u \r\n", rev);
        return rev;
    }
    else
    {
        return 0;
    }
}

