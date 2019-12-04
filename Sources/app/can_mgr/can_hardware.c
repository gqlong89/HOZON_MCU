/*
 * can_hareware.c
 *
 *  Created on: 2018Äê4ÔÂ28ÈÕ
 *      Author: zhouhong
 */

#include <gpio.h>

static unsigned int num_flex_can = 0;
void can_check_flexcan_phy_num(void)
{
    if (GPIO_ReadPinInput(RD1_PORT, RD1_PORT_PIN))
    {
        num_flex_can++;
    }

    if (GPIO_ReadPinInput(RD2_PORT, RD2_PORT_PIN))
    {
        num_flex_can++;
    }

    if (GPIO_ReadPinInput(RD3_PORT, RD3_PORT_PIN))
    {
        num_flex_can++;
    }
}

unsigned int can_get_flexcan_phy_num(void)
{
    return num_flex_can;
}

