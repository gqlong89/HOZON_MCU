/**
 * @Title: can_spi.c
 * @author yuzhimin
 * @date Jul 21, 2017
 * @version V1.0
 */

#include <app/can_mgr/can_mgr.h>
#include <can_api.h>
#include <gpio.h>
#include <gpio_hal.h>
#include <ipc_spi.h>
#include <lpspi_hal.h>
#include <mid/can_spi/can_spi.h>
#include <mid/can_spi/mcp2515.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <port_hal.h>
#include <string.h>
#include <S32K144.h>

static void spican_irq_handler(uint32_t instance);

/*can 4 rx interrupe function*/
static void CAN_INT_ISR_4(unsigned int pin)
{
    if (GP_CAN4_nINT_PIN_IN == pin)
    {
        spican_irq_handler(LPSPICAN1);
        PORT_HAL_ClearPinIntFlagCmd(CAN4_INT_PORT, CAN4_INT_PIN);
    }
}

/*can 5 rx interrupe function*/
static void CAN_INT_ISR_5(unsigned int pin)
{
    if (GP_CAN5_nINT_PIN_IN == pin)
    {
        spican_irq_handler(LPSPICAN2);
        PORT_HAL_ClearPinIntFlagCmd(CAN5_INT_PORT, CAN5_INT_PIN);
    }
}
/*can io init function*/
static void GPIOInit(uint32_t instance)
{
    if (LPSPICAN1 == instance)
    {
        GPIO_INT_CFG gpio_int_cfg_a =
        {PORT_NUM_A, PORT_INT_LOGIC_ZERO, GP_CAN4_nINT_PIN_IN, &CAN_INT_ISR_4 };
        /*can 4 rest */
        GPIO_HAL_SetPinDirection(GP_CAN4_RESET, GP_CAN4_RESET_PIN_OUT, 1);
        GPIO_HAL_SetPins(GP_CAN4_RESET, 1 << GP_CAN4_RESET_PIN_OUT);

        gpio_isr_init(&gpio_int_cfg_a);

    }
    else if (LPSPICAN2 == instance)
    {
        GPIO_INT_CFG gpio_int_cfg_e =
        {PORT_NUM_E, PORT_INT_LOGIC_ZERO, GP_CAN5_nINT_PIN_IN, &CAN_INT_ISR_5 };

        /*can 5 rest */
        GPIO_HAL_SetPinDirection(GP_CAN5_RESET, GP_CAN5_RESET_PIN_OUT, 1);
        GPIO_HAL_SetPins(GP_CAN5_RESET, 1 << GP_CAN5_RESET_PIN_OUT);

        gpio_isr_init(&gpio_int_cfg_e);
    }
}

static void Power_On(void)
{
    /*EN3*/
    GPIO_HAL_SetPinDirection(PTB, 14, 1);
    GPIO_HAL_SetPins(PTB, 1 << 14);
}
/********************************************************************
 function:     spican_init_gpio
 description:
 input:        none
 output:       none
 return:       none
 *********************************************************************/
static void spican_init_gpio(uint32_t instance)
{
    Power_On();
    GPIOInit(instance);
    OSIF_TimeDelay(100);
}

/* function:get send maibox state
 * param:
 * instance : 1(can4) , 2 (can5)
 * num: 0, 1, 2 (index of tx mailbox)
 * perr: to store err state
 * return :true- KO ,false - wrong
 *  */
bool Get_send_sta(uint32_t instance, uint8_t num, uint8_t *perr)
{
    instance = CAN_2_SPI(instance);
    uint8_t tmp = 0;

    if (0 == perr)
    {
        perr = 0;
    }
    else if (num > 2)
    {
        *perr = ERR_OVER;
    }
    else
    {
        *perr = ERR_NO;
    }

    tmp = mcp2515_read_register(instance, 0x30 + 0x10 * num);
    return ((tmp >> 3) & 0x01);
}
static bool dataLoad(uint32_t instance, uint8_t no)
{
    uint32_t i;
    uint32_t id, dlc;
    uint8_t tmpChar, stansrr = 0, extrtr = 0, ext = 2;
    CAN_MSG *can_buf = NULL;

    no *= 0x10;

    tmpChar = mcp2515_read_register(instance, 0x62 + no);   /*Rx 0/1 flag_low*/
    id = 0;

    if (tmpChar & (1 << 3))   /*ex IDE*/
    {
        ext = 1;
        id |= (mcp2515_read_register(instance, 0x61 + no) << 21);     /*sid<10:3>*/
        id |= ((tmpChar & 0x03) << 16);     /*eid<17:16>*/
        id |= ((tmpChar & 0xE0) << 13);     /*sid<2:0>*/
        id |= (mcp2515_read_register(instance, 0x63 + no) << 8);     /*eid<15:8>*/
        id |= mcp2515_read_register(instance, 0x64 + no);   /*eid<7:0>*/
    }
    else
    {
        ext = 0;

        if (tmpChar & (1 << 4))
        {
            stansrr = 1;    //Remote frame
        }

        id |= ((tmpChar & 0xE0) >> 5);     /*sid<2:0>*/
        id |= (mcp2515_read_register(instance, 0x61 + no) << 3);     /*sid<10:3>*/
    }

    tmpChar = mcp2515_read_register(instance, 0x65 + no);
    dlc = (tmpChar & 0x0F) % 9;

    if ((tmpChar & (1 << 6)) && (ext == 1))        /*eid + rtr*/
    {
        extrtr = 1;    //Remote frame
    }

    if (((ext == 0) && (stansrr == 1)) || ((ext == 1) && (extrtr == 1)))
    {
        return true;    //Remote frame don't recieve
    }

    if ((can_buf = (CAN_MSG *)ipc_can_handle(SPI_GET_FRAME)) != NULL)
    {
        uint32_t uptimeWhenGot, miscUptimeWhenGot, flag, check;
        uptimeWhenGot       = SysTick_GetTickCount();
        miscUptimeWhenGot   = SysTick_GetCurCounter();
        flag                = SysTick_GetCountFlag();
        check               = SysTick_GetCurCounter();

        if (flag && (check <= miscUptimeWhenGot))
        {
            uptimeWhenGot += 1;
        }

        /* Divide by 16 to make sure the miscUptime bits less than 16Bit. */
        can_buf->miscUptime = miscUptimeWhenGot >> 4;
        can_buf->uptime     = uptimeWhenGot;
        can_buf->type = 'C';
        can_buf->dlc =  dlc & 0x0F ;
        can_buf->MsgID = id | (ext ? (1u << 31) : 0);

        for (i = 0; i < dlc; i++)
        {
            can_buf->Data[i] = mcp2515_read_register(instance, 0x66 + no + i);
        }

        if (can_receive(SPI_2_CAN(instance), can_buf))
        {
            ipc_can_handle(SPI_SET_FRAME);
        }
        else
        {
            ipc_can_handle(SPI_DROP_FRAME);
        }
    }

    return true;
}
/********************************************************************
 function:     spican_irq_handler
 description:  master ready irq handler
 input:        none
 output:       none
 return:       none
 *********************************************************************/
static void spican_irq_handler(uint32_t instance)
{
    unsigned char irqFlag;
    irqFlag = mcp2515_read_register(instance, CANINTF);

    if (irqFlag & 0xA0) /* err flag */
    {
        mcp2515_bit_modify(instance, CANINTF, irqFlag, 0);

        irqFlag = mcp2515_read_register(instance, EFLG);

        if (irqFlag & (1 << 5))
        {
            can_err_set(SPI_2_CAN(instance), STATE_BUSOFF);
        }
        else
        {
            can_err_set(SPI_2_CAN(instance), STATE_ERR);
        }

        return;
    }

    if (irqFlag & 0x01) /*RX 0*/
    {
        dataLoad(instance, 0);
        mcp2515_bit_modify(instance, CANINTF, 0x01, 0);
    }

    if (irqFlag & 0x02) /*RX 1*/
    {
        dataLoad(instance, 1);
        mcp2515_bit_modify(instance, CANINTF, 0x02, 0);
    }

    can_err_set(SPI_2_CAN(instance), STATE_RECEIVE);
    can_rectime_set(SPI_2_CAN(instance));
}

uint8_t spican_baud(uint32_t instance, uint32_t baud)
{
    uint8_t ret = 0;
    instance = CAN_2_SPI(instance);
    ret |= mcp2515_write_register(instance, CANCTRL, 0x82);   /* enter conf mode */

    switch (baud)
    {
        case 50000:
            ret |= mcp2515_write_register(instance, CNF1, 0x09);   /*(SJW-1)<<6 + BRP*/
            ret |= mcp2515_write_register(instance, CNF2,
                                          0xB6);   /*BTLMODE<<7 + ASM<<6 + (PS1-1)<<3 + (seg-1)<<0*/
            ret |= mcp2515_write_register(instance, CNF3, 0x04);   /*(PS2-1)<<0*/
            break;

        case 100000:
            ret |= mcp2515_write_register(instance, CNF1, 0x04);
            ret |= mcp2515_write_register(instance, CNF2, 0xBD);
            ret |= mcp2515_write_register(instance, CNF3, 0x04);
            break;

        case 125000:
            ret |= mcp2515_write_register(instance, CNF1, 0x03);
            ret |= mcp2515_write_register(instance, CNF2, 0xBD);
            ret |= mcp2515_write_register(instance, CNF3, 0x04);
            break;

        case 250000:
            ret |= mcp2515_write_register(instance, CNF1, 0x01);
            ret |= mcp2515_write_register(instance, CNF2, 0xBD);
            ret |= mcp2515_write_register(instance, CNF3, 0x04);
            break;

        case 1000000:
            ret |= mcp2515_write_register(instance, CNF1, 0x00);
            ret |= mcp2515_write_register(instance, CNF2, 0xA0);
            ret |= mcp2515_write_register(instance, CNF3, 0x02);
            break;

        case 500000:
        default:
            ret |= mcp2515_write_register(instance, CNF1, 0x00);
            ret |= mcp2515_write_register(instance, CNF2, 0xBD);
            ret |= mcp2515_write_register(instance, CNF3, 0x04);
            break;
    }

    ret |= mcp2515_write_register(instance, CANCTRL, 0x00);   /* enter normal mode */
    return ret;
}
/* function:init SPI CAN
 * param:
 * instance : 1(can4) , 2 (can5)
 * baud: 5k, 100k, 250k ,1m (Baud rate)
 *
 *  */
uint32_t spican_init(uint32_t instance, uint32_t baud)
{
    instance = CAN_2_SPI(instance);
    mcp2515_init(instance);
    spican_init_gpio(instance);
    /* configure the MCP2510 */
    printk(PRINTK_LEVEL_DEBUG, "SPI CAN control: %02X",
           mcp2515_read_register(instance, CANCTRL));
    spican_baud(SPI_2_CAN(instance), baud);
    printk(PRINTK_LEVEL_DEBUG, "After configuration: %02X",
           mcp2515_read_register(instance, CANCTRL));

    mcp2515_write_register(instance, 0x60, 0x64);   /*rx buff0 close filter  and  open guncun*/
    mcp2515_write_register(instance, 0x70, 0x60);  /*rx buff1 close filter*/
    mcp2515_write_register(instance, CANINTE, 0x03 | 0x80);   /* enable  err + Rx IRQ */

    return 0;
}

/* function:deinit SPI CAN
 * param:
 * instance : 1(can4) , 2 (can5) , 1+2 (can4 +can5)
 *
 *  */
bool spican_deinit(uint32_t instance)
{
    instance = CAN_2_SPI(instance);

    if ((instance > 2) || (instance < 1))
    {
        return false;
    }

    if (LPSPICAN1 & instance)
    {
        /*can 4 rest */
        GPIO_HAL_ClearPins(GP_CAN4_RESET, 1 << GP_CAN4_RESET_PIN_OUT);
        /*spi disable*/
        LPSPI_HAL_Disable(LPSPI1);
    }

    if (LPSPICAN2 & instance)
    {
        /*can 5 rest */
        GPIO_HAL_ClearPins(GP_CAN5_RESET, 1 << GP_CAN5_RESET_PIN_OUT);
        /*spi disable*/
        LPSPI_HAL_Disable(LPSPI2);
    }

    return true;
}

/* function: send data on CAN  , 8 data length default
 * param:
 * instance : 1(can4) or 2(can5)
 * bufferPtr: pointer to sending data
 * num: 0 , 1, 2 (index of tx mailbox)
 * */
uint8_t spican_send(uint32_t instance, unsigned char *bufferPtr, uint8_t num)
{
    uint32_t *srcPtr = (uint32_t *) bufferPtr;
    unsigned char canStat = 0, res = 0;

    instance = CAN_2_SPI(instance);
    canStat = mcp2515_read_register(instance, 0x1E);   /*can state register*/

    if ((canStat & 0xE0) != 0x00)    /*work mode , 000 - normal*/
    {
        printk(PRINTK_LEVEL_DEBUG, "reseting mcp2515 to  baud (canstat:%02x)", canStat);
        return ERR_MOD;
    }

    if (mcp2515_read_register(instance, EFLG) & (1 << 5))      /*err flag register , bus off */
    {
        printk(PRINTK_LEVEL_DEBUG, "Bus error!");
        return ERR_BUS_OFF;
    }

    /* try to get the idle buffer */
    if (mcp2515_read_register(instance,
                              0x30 + 0x10 * num) & (1 << 3))      /*tx buff control register , has send request*/
    {
        return ERR_BUSY;
    }

    if (srcPtr[0] & (1u << 31))    /*e/s<31> , dlc<19:16>*/
    {
        /*id s<28:18>e<17:0>*/
        res |= mcp2515_write_register(instance, 0x31 + num * 0x10, srcPtr[1] >> 21);   /*sid <10:3>*/
        res |= mcp2515_write_register(instance, 0x34 + num * 0x10, srcPtr[1]);   /*eid <7:0>*/
        res |= mcp2515_write_register(instance, 0x33 + num * 0x10, srcPtr[1] >> 8);   /*eid <15:8>*/
        res |= mcp2515_write_register(instance, 0x32 + num * 0x10,  /*sid (7-5)<2:0> , eid (1-0)<17:16>*/
                                      ((srcPtr[1] >> 16) & 0x03) | 0x08 | ((srcPtr[1] >> 13) & 0xE0));          /*ex id*/
    }
    else
    {
        res |= mcp2515_write_register(instance, 0x31 + num * 0x10, srcPtr[1] >> 3);
        res |= mcp2515_write_register(instance, 0x32 + num * 0x10, (srcPtr[1] << 5) & 0xE0);
    }

    res |= mcp2515_write_register(instance, 0x35 + num * 0x10, (srcPtr[0] >> 16) & 0x0F);     /* DLC */

    res |= mcp2515_write_register(instance, 0x36 + num * 0x10, srcPtr[2]);   /* Data */
    res |= mcp2515_write_register(instance, 0x37 + num * 0x10, srcPtr[2] >> 8);
    res |= mcp2515_write_register(instance, 0x38 + num * 0x10, srcPtr[2] >> 16);
    res |= mcp2515_write_register(instance, 0x39 + num * 0x10, srcPtr[2] >> 24);
    res |= mcp2515_write_register(instance, 0x3A + num * 0x10, srcPtr[3]);
    res |= mcp2515_write_register(instance, 0x3B + num * 0x10, srcPtr[3] >> 8);
    res |= mcp2515_write_register(instance, 0x3C + num * 0x10, srcPtr[3] >> 16);
    res |= mcp2515_write_register(instance, 0x3D + num * 0x10, srcPtr[3] >> 24);
    /* mcp2515_write_registerRST( no ); */
    res |= mcp2515_write_register(instance, 0x30 + num * 0x10,
                                  1 << 3);   /*tx control_reg , tx request*/

    if (0 == res)
    {
        can_err_set(SPI_2_CAN(instance), STATE_SEND);
    }

    return res;
}

