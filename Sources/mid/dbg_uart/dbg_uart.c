/**
 * @Title: dbg_uart.c
 * @author yuzhimin
 * @date Jul 12, 2017
 * @version V1.0
 */

#include <dbg_uart.h>
#include <lpuart_driver.h>
#include <lpuart_hal.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <ring_buffer.h>
#include <status.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_stdint.h>

#define INST_DBGUART (0U)
/*! Driver state structure */
lpuart_state_t dbgUart_State;
static uint8_t task_id = INVALID_TASK_NUMBER;

#define DBG_UART_RX_BUFSIZE   (128U)
//#define DBG_UART_TX_BUFSIZE   (128U)
static uint8_t dbg_rx_buf[DBG_UART_RX_BUFSIZE];
//static uint8_t dbg_tx_buf[DBG_UART_TX_BUFSIZE];
//static ring_buffer dbg_tx_ring, dbg_rx_ring;
static ring_buffer  dbg_rx_ring;

#define DBGUART_BUFSIZE   (512U)
#define DBGUART_TX_LEN    (256U)

#define UART_SEND_TIMEOUT   (50)

/* Tx buffer */
static unsigned char DBGUART_TxBuf[DBGUART_BUFSIZE];
/* Pointer used by interrupt */
static unsigned int  DBGUART_TxPtr = 0;
/* Pointer used by app */
static unsigned int  DBGUART_TxAppPtr = 0;
/* sending or not */
static volatile unsigned int isSending = 0;

int _write(int fd, char *ptr, int len)
{
    UNUSED(fd);
    dbg_uart_send((uint8_t *) ptr, (uint32_t) len);
    return len;
}

int _read(int fd, char *ptr, int len)
{
    /* Read "len" of char to "ptr" from file id "fd"
     * Return number of char read.
     * Need implementing with UART here. */
    UNUSED(fd);
    UNUSED(ptr);

    return len;
}

void _ttywrch(int ch)
{
    #if 0
    uint32_t bytesRemaining;

    /* Write one char "ch" to the default console
     * Need implementing with UART here. */
    LPUART_DRV_SendData(INST_DBGUART, (const uint8_t *) &ch, 1);

    while (LPUART_DRV_GetTransmitStatus(INST_DBGUART, &bytesRemaining) != STATUS_SUCCESS)
    {
    }

    #endif
    dbg_uart_send((const uint8_t *) &ch, 1);

}

void dbg_uart_tx_callback(uint32_t instance, void *callbackParam)
{
    UNUSED(instance);
    UNUSED(callbackParam);

    #if 0

    if (false == dbgUart_State.isTxBusy)
    {
        if (rb_used_len(&dbg_tx_ring) > 0)
        {
            dbgUart_State.transmitStatus = STATUS_UART_TX_FFLUSH;
            kernelSetTaskEvent(task_id, TASK_SYS_EVENT_SEND);
        }
    }

    #endif

    #if 1
    unsigned char *txbuffer;
    unsigned int txlength;

    if (DBGUART_TxAppPtr == DBGUART_TxPtr)
    {
        isSending = 0;
        return;
    }

    /* enable TX */
    if (((DBGUART_TxAppPtr - DBGUART_TxPtr + DBGUART_BUFSIZE) % DBGUART_BUFSIZE) > DBGUART_TX_LEN)
    {
        txlength = DBGUART_TX_LEN;
        txbuffer = (uint8_t *)(DBGUART_TxBuf + DBGUART_TxPtr);
        DBGUART_TxPtr = (DBGUART_TxPtr + DBGUART_TX_LEN) % DBGUART_BUFSIZE;
    }
    else
    {
        txlength = (DBGUART_TxAppPtr - DBGUART_TxPtr + DBGUART_BUFSIZE) % DBGUART_BUFSIZE;
        txbuffer = (uint8_t *)(DBGUART_TxBuf + DBGUART_TxPtr);
        DBGUART_TxPtr    = 0;
        DBGUART_TxAppPtr = 0;
    }

    LPUART_DRV_SendData(INST_DBGUART, txbuffer, txlength);
    #endif
}

void dbg_uart_rx_callback(uint32_t instance, void *callbackParam)
{
    static uint8_t rx;

    UNUSED(instance);
    UNUSED(callbackParam);

    if (task_id == INVALID_TASK_NUMBER)
    {
        LPUART_DRV_AbortReceivingData(instance);
        return;
    }

    if (STATUS_UART_RX_OVERRUN == dbgUart_State.receiveStatus)
    {
        printk(PRINTK_LEVEL_ERROR, "Hardware buffer overflow !");
    }

    /* Get data and put in receive buffer  */
    if (true == LPUART_DRV_GetChar(instance, &rx))
    {
        if (!rb_put(&dbg_rx_ring, &rx, 1))
        {
            printk(PRINTK_LEVEL_ERROR, "Software buffer overflow !");
        }

        kernelSetTaskEvent(task_id, TASK_SYS_EVENT_READ);
    }
}

/* open dbg uart */
void dbg_uart_open(uint8_t task)
{
    task_id = task;
    LPUART_DRV_ReceiveData(INST_DBGUART, dbg_rx_buf, sizeof(dbg_rx_buf));
}

void dbg_uart_init(void)
{
    /*! dbgUart configuration structure */
    lpuart_user_config_t initConfig =
    {
        .transferType = LPUART_USING_TX_DMA_RX_INTERRUPTS,
        .baudRate = 115200U,
        .parityMode = LPUART_PARITY_DISABLED,
        .stopBitCount = LPUART_ONE_STOP_BIT,
        .bitCountPerChar = LPUART_8_BITS_PER_CHAR,
        .rxDMAChannel = 255U,
        .txDMAChannel = 0U,
    };

    //rb_init( &dbg_tx_ring, dbg_tx_buf, sizeof( dbg_tx_buf ) );
    rb_init(&dbg_rx_ring, dbg_rx_buf, sizeof(dbg_rx_buf));
    LPUART_DRV_Init(INST_DBGUART, &dbgUart_State, &initConfig);
    LPUART_DRV_InstallTxCallback(INST_DBGUART, dbg_uart_tx_callback, NULL);
    LPUART_DRV_InstallRxCallback(INST_DBGUART, dbg_uart_rx_callback, NULL);
}

uint32_t dbg_uart_send(const uint8_t *buffer, uint32_t size)
{

    #if 0
    uint32_t send_size, bytesRemaining;

    if (buffer == NULL || size == 0)
    {
        return 0;
    }

    if (false == dbgUart_State.isTxBusy &&
        STATUS_UART_TX_FFLUSH != dbgUart_State.transmitStatus)
    {
        LPUART_DRV_SendData(INST_DBGUART, buffer, size);

        while (LPUART_DRV_GetTransmitStatus(INST_DBGUART, &bytesRemaining) != STATUS_SUCCESS)
        {
        }

        send_size = size;
    }
    else
    {
        send_size = rb_put(&dbg_tx_ring, buffer, size);
    }

    return send_size;
    #endif

    #if 1
    unsigned int i;
    unsigned char *txbuffer;
    unsigned int txlength = 0;

    if (size > 0)
    {
        if (DBGUART_TxAppPtr <= DBGUART_TxPtr)
        {
            if (((DBGUART_TxAppPtr + size) >= DBGUART_TxPtr))
            {
                uint32_t bytesRemaining;
                uint64_t uart_send_data_time;
                uart_send_data_time = driverGetUptime();

                while (LPUART_DRV_GetTransmitStatus(INST_DBGUART, &bytesRemaining) != STATUS_SUCCESS)
                {
                    if (driverGetUptime() - uart_send_data_time >= UART_SEND_TIMEOUT)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            if (((DBGUART_TxAppPtr + size - DBGUART_BUFSIZE) >= DBGUART_TxPtr))
            {
                uint32_t bytesRemaining;
                unsigned int uart_send_data_time;
                uart_send_data_time = driverGetUptime();

                while (LPUART_DRV_GetTransmitStatus(INST_DBGUART, &bytesRemaining) != STATUS_SUCCESS)
                {
                    if (driverGetUptime() - uart_send_data_time >= UART_SEND_TIMEOUT)
                    {
                        break;
                    }
                }
            }
        }

        //INT_SYS_DisableIRQGlobal();
        INT_SYS_DisableIRQ(LPUART0_RxTx_IRQn);
        INT_SYS_DisableIRQ(DMA0_IRQn);

        for (i = 0; i < size; i++)
        {
            if ((DBGUART_TxAppPtr + 1) % DBGUART_BUFSIZE == DBGUART_TxPtr)
            {
                /* overflow */
                break;
            }

            DBGUART_TxBuf[DBGUART_TxAppPtr] = buffer[i];
            DBGUART_TxAppPtr = (DBGUART_TxAppPtr + 1) % DBGUART_BUFSIZE;
        }

        if ((!isSending) && (i != 0))
        {
            /* enable TX */
            if (((DBGUART_TxAppPtr - DBGUART_TxPtr + DBGUART_BUFSIZE) % DBGUART_BUFSIZE) > DBGUART_TX_LEN)
            {
                txlength = DBGUART_TX_LEN;
                txbuffer = (uint8_t *)(DBGUART_TxBuf + DBGUART_TxPtr);
                DBGUART_TxPtr = (DBGUART_TxPtr + DBGUART_TX_LEN) % DBGUART_BUFSIZE;
            }
            else
            {
                txlength = (DBGUART_TxAppPtr - DBGUART_TxPtr + DBGUART_BUFSIZE) % DBGUART_BUFSIZE;
                txbuffer = (uint8_t *)(DBGUART_TxBuf + DBGUART_TxPtr);
                DBGUART_TxPtr    = 0;
                DBGUART_TxAppPtr = 0;
            }

            isSending = 1;

            //INT_SYS_EnableIRQGlobal();
            INT_SYS_EnableIRQ(LPUART0_RxTx_IRQn);
            INT_SYS_EnableIRQ(DMA0_IRQn);

            LPUART_DRV_SendData(INST_DBGUART, txbuffer, txlength);
        }
        else
        {
            //INT_SYS_EnableIRQGlobal();
            INT_SYS_EnableIRQ(LPUART0_RxTx_IRQn);
            INT_SYS_EnableIRQ(DMA0_IRQn);
        }
    }

    return txlength;
    #endif
}

/* read data on DBGUART */
uint32_t dbg_uart_recv(uint8_t *buffer, uint32_t size)
{
    uint32_t len;

    INT_SYS_DisableIRQ(LPUART0_RxTx_IRQn);
    INT_SYS_DisableIRQ(DMA0_IRQn);
    len = rb_get(&dbg_rx_ring, buffer, size);
    INT_SYS_EnableIRQ(DMA0_IRQn);
    INT_SYS_EnableIRQ(LPUART0_RxTx_IRQn);

    if (!g_xModemCommEnable && len)
    {
        /* used for echo */
        dbg_uart_send(buffer, len);
    }

    return len;
}

void dbgUartwait(void)
{
    uint32_t bytesRemaining;

    while (LPUART_DRV_GetTransmitStatus(INST_DBGUART, &bytesRemaining) != STATUS_SUCCESS)
    {
    }
}

void dbg_uart_fflush(void)
{
    #if 0
    uint32_t len, bytesRemaining;

    if (false == dbgUart_State.isTxBusy)
    {
        len = rb_used_len(&dbg_tx_ring);

        if (len)
        {
            LPUART_DRV_SendData(INST_DBGUART, dbg_tx_buf, len);

            while (LPUART_DRV_GetTransmitStatus(INST_DBGUART, &bytesRemaining) != STATUS_SUCCESS)
            {
            }

            rb_clean(&dbg_tx_ring);
        }
    }

    #endif
}

