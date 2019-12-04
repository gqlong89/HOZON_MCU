/**
 * @Title: ipc_spi.c
 * @author yuzhimin
 * @date Jul 25, 2017
 * @version V1.0
 */

#include <can_api.h>
#include <edma.h>
#include <gpio.h>
#include <gpio_hal.h>
#include <ipc_spi.h>
#include <lpspi_hal.h>
#include <lpspi_shared_function.h>
#include <lpspi_slave_driver.h>
#include <mid/ipc_spi/ipc_spi_list.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/msg/msg_parse.h>
#include <osif.h>
#include <ring_buffer.h>
#include <status.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_stdint.h>
#include <S32K144.h>
#include <timer.h>

#define IPC_SPI_TX_BUFSIZE      (IPC_FRAME_LENGTH)
#define IPC_SPI_TX_TIMEOUT      (2U)

static uint8_t ipcspi_txbuf[IPC_SPI_TX_BUFSIZE];
static ring_buffer ipcspi_txring;

static IPC_FRAME_T *spi_txframe = NULL;
static IPC_FRAME_T *spi_rxframe = NULL;
static uint8_t spi_dummy[2][IPC_SPI_FRAME_LEN];

/*! @brief State structure for LPSPI0 */
static lpspi_state_t lpspiIPCState;
static int8_t ipc_timer_handle;
static int8_t ipc_can_timer;
static uint8_t ipc_task = INVALID_TASK_NUMBER;
static uint8_t ipc_enable = 1U;


static void ipc_can_timeout(int8_t handle);
/*********************************************************************
 function:     ipc_tranfer_start
 description:  start to tranfer one frame
 input:        none
 output:       none
 return:       none
 *********************************************************************/
static void ipc_tranfer_start(void)
{
    uint64_t ipc_tranfer_start_time;
    uint8_t *sendBuffer = NULL;
    uint8_t *recvBuffer = NULL;

    if (!ipc_enable)
    {
        printk(PRINTK_LEVEL_ERROR, "ipc disable");
        return;
    }

    ipc_tranfer_start_time = driverGetUptime();
    GPIO_HAL_WritePin(GP_4G_SRDY_3V3, GP_4G_SRDY_3V3_PIN_OUT, 0U);
    unsigned int cnt = 0;

    if (spi_txframe)
    {
        ipc_put_freeframe(spi_txframe);
        spi_txframe = NULL;
    }

    if (spi_rxframe)
    {
        recvBuffer = spi_rxframe->pack;
    }
    else
    {
        spi_rxframe = ipc_get_freeframe(SPI_RCV_FRAME);

        if (spi_rxframe)
        {
            recvBuffer = spi_rxframe->pack;
        }
        else
        {
            recvBuffer = spi_dummy[0];
            irq_cnt[14]++;
        }
    }

    spi_txframe = ipc_get_sendframe();
    sendBuffer = (spi_txframe != NULL) ? (spi_txframe->pack) : (spi_dummy[1]);

    while (LPSPI_DRV_SlaveTransfer(IPC_SPI_INSTANCE, sendBuffer, recvBuffer,
                                   IPC_SPI_FRAME_LEN) != STATUS_SUCCESS)
    {
        cnt++;

        if (driverGetUptime() - ipc_tranfer_start_time >= IPC_SPI_TX_TIMEOUT)
        {
            printk(PRINTK_LEVEL_ERROR, "ipc tranfer timeout");
            break;
        }
    }

    irq_cnt[15] = MAX(irq_cnt[15], cnt);

    GPIO_HAL_WritePin(GP_4G_SRDY_3V3, GP_4G_SRDY_3V3_PIN_OUT, 1U);
    ltimer_start(ipc_timer_handle, IPC_SRDY_TIMEOUT);
}

/*********************************************************************
 function:     ipc_tranfer_end
 description:  tranfer one frame completely
 input:        none
 output:       none
 return:       none
 *********************************************************************/
static inline void ipc_tranfer_end(void)
{
    ltimer_stop(ipc_timer_handle);
}

/*********************************************************************
 function:     ipc_tranfer_end
 description:  tranfer one frame completely
 input:        none
 output:       none
 return:       none
 *********************************************************************/
static inline void ipc_tranfering(void)
{
    if (spi_txframe)
    {
        ipc_put_freeframe(spi_txframe);
        spi_txframe = NULL;
    }
}

/****************************************************************
 * function:      ipc_callback
 * description:   spi tansfer result callback,
 *                it is called in spi interrupt context
 * input:         uint32_t instance, SPI register;
 *                void *param, user data
 * output:        none
 * return:        0 indicates success;
 *                others indicates failed
 *****************************************************************/
static void ipc_callback(uint32_t instance, void *param)
{
    UNUSED(param);

    unsigned int msg_len;
    IPC_MSG_HDR *msg_hdr;

    ipc_tranfering();

    if (ipc_task == INVALID_TASK_NUMBER)
    {
        (void) LPSPI_DRV_SlaveAbortTransfer(instance);
        return;
    }

    if (LPSPI_TRANSMIT_FAIL == lpspiIPCState.status)
    {
        printk(PRINTK_LEVEL_ERROR, "SPI transmit Failed !");
        return;
    }

    if (LPSPI_RECEIVE_FAIL == lpspiIPCState.status)
    {
        printk(PRINTK_LEVEL_ERROR, "SPI receive Failed !");
        return;
    }

    if (LPSPI_DMA_ERROR == lpspiIPCState.status)
    {
        printk(PRINTK_LEVEL_ERROR, "SPI DMA Error!");
        edma_chn_reinit(EDMA_CHN_LPSPI0_RX);
        edma_chn_reinit(EDMA_CHN_LPSPI0_TX);
        return;
    }

    if (LPSPI_TRANSFER_CMP == lpspiIPCState.status)
    {
        //printk(PRINTK_LEVEL_DEBUG, "transfer data successfully");
        msg_hdr = (IPC_MSG_HDR *) spi_rxframe;

        if (spi_rxframe == (IPC_MSG_HDR *)spi_dummy[0])
        {
            spi_rxframe = NULL;
            irq_cnt[17]++;
        }
        else
        {
            if (msg_hdr->flag == 0x5a)
            {
                msg_len = msg_hdr->len;

                if (msg_len > IPC_SPI_FRAME_LEN - IPC_SPI_HDR || !msg_len)
                {
                    msg_hdr->flag = 0x0;  // clean head flag of the recv frame
                    msg_hdr->len = 0;
                    //printk(PRINTK_LEVEL_ERROR, "Invalid data length,len:%u", msg_len);
                }
                else
                {
                    ipc_put_recvframe(spi_rxframe);
                    spi_rxframe = NULL;
                    kernelSetTaskEvent(ipc_task, TASK_SYS_EVENT_READ);
                }
            }
        }

        ipc_tranfer_end();
        ipc_tranfer_start();

        if (!rb_empty(&ipcspi_txring))
        {
            kernelSetTaskEvent(ipc_task, TASK_SYS_EVENT_SEND);
        }
    }
    else
    {
        printk(PRINTK_LEVEL_ERROR, "unexpected result:%u", lpspiIPCState.status);
    }
}

/********************************************************************
 function:     ipc_init_spi
 description:  initiaze spi
 input:        none
 output:       none
 return:       none
 *********************************************************************/
void ipc_init_spi(void)
{
    IPC_MSG_HDR *msg_hdr;
    lpspi_slave_config_t lpspiIPC_SlaveConfig =
    {
        .pcsPolarity = LPSPI_ACTIVE_LOW,
        .bitcount = 32U,
        .clkPhase = LPSPI_CLOCK_PHASE_1ST_EDGE,
        .whichPcs = LPSPI_PCS0,
        .clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
        .lsbFirst = false,
        .transferType = LPSPI_USING_DMA,
        .rxDMAChannel = 1U,
        .txDMAChannel = 2U,
    };
    LPSPI_DRV_SlaveInit(IPC_SPI_INSTANCE, &lpspiIPCState, &lpspiIPC_SlaveConfig);
    LPSPIT_DRV_SlaveInstallCallback(IPC_SPI_INSTANCE, ipc_callback, NULL);
    //memset(spi_dummy, 0, sizeof(spi_dummy));
    msg_hdr = (IPC_MSG_HDR *) spi_dummy[0];
    msg_hdr->flag     = 0x5a;
    msg_hdr->len      = 0;
    msg_hdr = (IPC_MSG_HDR *) spi_dummy[1];
    msg_hdr->flag     = 0x5a;
    msg_hdr->len      = 0;
    return;
}

/********************************************************************
 function:     ipc_init_gpio
 description:  initiaze gpio which is used for
 slave ready and master ready
 input:        none
 output:       none
 return:       none
 *********************************************************************/
static void ipc_init_gpio(void)
{
    /* Define the init structure for slave ready pin */
    GPIO_IO_CFG srdy_config = { GP_4G_SRDY_3V3, GP_4G_SRDY_3V3_PIN_OUT, GPIO_OUT_LOW_CONF };

    gpio_io_init(&srdy_config);
}

void ipc_wakeup_init(void)
{
    ipc_init_gpio();
    ltimer_start(ipc_timer_handle, IPC_SRDY_TIMEOUT);
}

static void ipc_timeout_handler(int8_t handle)
{
    if (handle != ipc_timer_handle)
    {
        return;
    }

    printk(PRINTK_LEVEL_ERROR, "ipc_timeout_handler");
    ipc_tranfer_end();
    ipc_tranfer_start();
}

/********************************************************************
 function:     ipc_init
 description:  initiaze ipc spi device
 input:        none
 output:       none
 return:       none
 *********************************************************************/
void ipc_device_init(void)
{
    rb_init(&ipcspi_txring, ipcspi_txbuf, sizeof(ipcspi_txbuf));
    ipc_init_spi();
    ipc_init_gpio();
    ipc_lst_init();

    ipc_timer_handle = ltimer_create(ipc_timeout_handler);
    ipc_can_timer = ltimer_create(ipc_can_timeout);
}

void ipc_device_deinit(void)
{
    ipc_stop();
    LPSPI_DRV_SlaveDeinit(IPC_SPI_INSTANCE);
}

/********************************************************************
 function:     ipc_set_taskid
 description:  initiaze ipc task
 input:        none
 output:       none
 return:       none
 *********************************************************************/
void ipc_set_taskid(uint8_t task)
{
    ipc_task = task;
    ltimer_start(ipc_timer_handle, IPC_SRDY_TIMEOUT);
}

/********************************************************************
 function:     ipc_read
 description:  receiving message api
 input:        unsigned char *msg, message buffer;
 output:       none
 return:       message length
 *********************************************************************/
int ipc_read(unsigned char *msg, unsigned int msglen)
{
    unsigned char *msg_body;
    static IPC_FRAME_T *frame = NULL;
    static unsigned int fr_rpos = 0;
    unsigned int msg_wpos = 0;
    unsigned int size = 0;

    if (!ipc_enable)
    {
        printk(PRINTK_LEVEL_ERROR, "ipc disable");
        return -2;
    }

    while (msg_wpos < msglen)
    {
        if (frame)
        {
            size = MIN((msglen - msg_wpos), (frame->msg.header.len - fr_rpos));
            memcpy(&msg[msg_wpos], &frame->msg.body[fr_rpos], size);
            fr_rpos += size;
            msg_wpos += size;

            if (fr_rpos >= frame->msg.header.len)
            {
                ipc_put_freeframe(frame);
                frame = NULL;
                fr_rpos = 0;
            }
        }
        else
        {
            if ((frame = ipc_get_recvframe()) == NULL)
            {
                fr_rpos = 0;
                return msg_wpos; // recv msg empty
            }
            else
            {
                msg_body = ((unsigned char *) frame) + sizeof(IPC_MSG_HDR);

                if (frame->msg.header.cs !=
                    msg_checksum(msg_body, (int)frame->msg.header.len, 0))
                {
                    ipc_put_freeframe(frame);
                    frame = NULL;
                    fr_rpos = 0;
                }
            }
        }
    }

    return msg_wpos;    // need recv next
}

/********************************************************************
 function:     ipc_write
 description:  sending message api
 input:        unsigned char *msg, message buffer;
               unsigned int msglen, message length
 output:       none
 return:       none
 *********************************************************************/
int ipc_write(unsigned char *msg, unsigned int msglen)
{
    //printk( PRINTK_LEVEL_DEBUG, "ipc_write msg len:%u", msglen );
    //DUMPHEX( PRINTK_LEVEL_DEBUG, msg, msglen );

    /* tx queue overflow */
    if (rb_unused_len(&ipcspi_txring) < msglen)
    {
        printk(PRINTK_LEVEL_ERROR, "ipc tx queue overflow.");
        ipc_clean();
        printk(PRINTK_LEVEL_ERROR, "clear ring_buffer data");
        return -1;
    }

    rb_put(&ipcspi_txring, msg, msglen);
    kernelSetTaskEvent(ipc_task, TASK_SYS_EVENT_SEND);
    return 0;
}

int ipc_send(void)
{
    IPC_FRAME_T *frame;
    unsigned int data_len;

    while (!rb_empty(&ipcspi_txring))
    {
        if ((frame = ipc_get_freeframe(SPI_APP_FRAME)) != NULL)
        {
            data_len = rb_get(&ipcspi_txring, frame->msg.body, IPC_SPI_FRAME_LEN - IPC_SPI_HDR);
            frame->msg.header.flag  = 0x5a;
            frame->msg.header.ctl   = 0;
            frame->msg.header.chn   = SPI_APP_FRAME;
            frame->msg.header.len   = data_len;
            frame->msg.header.cs    = msg_checksum(frame->msg.body, (int)data_len, 0);
            ipc_put_sendframe(frame);
        }
        else
        {
            break;
        }
    }

    return rb_empty(&ipcspi_txring) ? 0 : 1;
}

void ipc_stop(void)
{
    ipc_enable = 0;
    ltimer_stop(ipc_timer_handle);
    ltimer_stop(ipc_can_timer);
    GPIO_HAL_WritePin(GP_4G_SRDY_3V3, GP_4G_SRDY_3V3_PIN_OUT, 0U);
}

void ipc_clean(void)
{
    rb_clean(&ipcspi_txring);
}

unsigned char *ipc_can_handle(unsigned char opt)
{
    static volatile int pos = 0;
    static IPC_FRAME_T *frame = NULL;

    if (SPI_GET_FRAME == opt)
    {
        if (!frame)
        {
            pos = 0;
            frame = ipc_get_freeframe(SPI_CAN_FRAME);

            /* Liubinkui added for geely vehilock ,start */
            //shellPrintf(" freelist has no space1\r\n");
            if (!frame)
            {
                frame = ipc_get_sendframe();
            }

            /* Liubinkui added for geely vehilock ,end */

            return frame != NULL ? &frame->msg.body[pos] : NULL;
        }
        else if (pos + sizeof(CAN_MSG) > IPC_SPI_BODY)
        {
            frame->msg.header.flag  = 0x5a;
            frame->msg.header.len   = pos;
            frame->msg.header.cs    = msg_checksum(frame->msg.body, pos, 0);
            frame->msg.header.chn   = SPI_CAN_FRAME;
            frame->msg.header.ctl   = 0;
            ipc_put_sendframe(frame);
            frame = ipc_get_freeframe(SPI_CAN_FRAME);
            pos = 0;

            /* Liubinkui added for geely vehilock ,start */
            //shellPrintf(" freelist has no space2\r\n");
            if (!frame)
            {
                frame = ipc_get_sendframe();
            }

            /* Liubinkui added for geely vehilock ,end */

            return frame != NULL ? &frame->msg.body[pos] : NULL;
        }
        else
        {
            return &frame->msg.body[pos];
        }
    }
    else if (SPI_SET_FRAME == opt)
    {
        pos += sizeof(CAN_MSG);

        if (pos + sizeof(CAN_MSG) > IPC_SPI_BODY)
        {
            frame->msg.header.flag  = 0x5a;
            frame->msg.header.len   = pos;
            frame->msg.header.cs    = msg_checksum(frame->msg.body, pos, 0);
            frame->msg.header.chn   = SPI_CAN_FRAME;
            frame->msg.header.ctl   = 0;
            ipc_put_sendframe(frame);
            frame = ipc_get_freeframe(SPI_CAN_FRAME);
            pos = 0;
        }
        else
        {
            ltimer_start(ipc_can_timer, IPC_CAN_TIMEOUT);
        }
    }
    else if (SPI_DROP_FRAME == opt)
    {
        //memset(frame->body[pos], 0, sizeof(CAN_MSG));
    }
    else if (SPI_SEND_FRAME == opt)
    {
        if (pos)
        {
            frame->msg.header.flag  = 0x5a;
            frame->msg.header.len   = pos;
            frame->msg.header.cs    = msg_checksum(frame->msg.body, pos, 0);
            frame->msg.header.chn   = SPI_CAN_FRAME;
            frame->msg.header.ctl   = 0;
            ipc_put_sendframe(frame);
            frame = ipc_get_freeframe(SPI_CAN_FRAME);
            pos = 0;
        }
    }
    else
    {
        printk(PRINTK_LEVEL_ERROR, "unkown spi opt%d", opt);
    }

    return NULL;
}

static void ipc_can_timeout(int8_t handle)
{
    if (handle != ipc_can_timer)
    {
        printk(PRINTK_LEVEL_ERROR, "wrong handle");
        return;
    }

    ltimer_stop(ipc_can_timer);
    printk(PRINTK_LEVEL_DEBUG, "ipc_can_timeout");
    ipc_can_handle(SPI_SEND_FRAME);
}

