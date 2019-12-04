/**
 * @Title: ipc_spi.h
 * @author yuzhimin
 * @date Aug 10, 2017
 * @version V1.0
 */
#ifndef __IPC_SPI_H__
#define __IPC_SPI_H__

#define IPC_SPI_INSTANCE            (0U)

#define IPC_SRDY_TIMEOUT            (300U)
#define IPC_CAN_TIMEOUT             (100U)
#define IPC_SPI_FRAME_LEN           (256U)
#define IPC_SPI_HDR                 (4U)
#define IPC_SPI_BODY                (IPC_SPI_FRAME_LEN - IPC_SPI_HDR)
#define IPC_SPI_FRAME_MAX           (50U)

/***大小待修改 --- IPC帧头4字节 + 待发送数据2032字节 + 预留标志、校验、转义符共10字节 ***/
#define IPC_FRAME_LENGTH            (2048)

enum
{
    SPI_GET_FRAME = 0U,
    SPI_SET_FRAME,
    SPI_DROP_FRAME,
    SPI_SEND_FRAME,
};

enum
{
    SPI_CAN_FRAME = 0U,
    SPI_APP_FRAME,
    SPI_RCV_FRAME,
};

void ipc_device_init(void);
void ipc_device_deinit(void);
void ipc_set_taskid(unsigned char task);
int  ipc_write(unsigned char *msg, unsigned int msglen);
int  ipc_read(unsigned char *msg, unsigned int msglen);
void ipc_stop(void);
void ipc_wakeup_init(void);
int  ipc_send(void);
void ipc_clean(void);
unsigned char *ipc_can_handle(unsigned char opt);

#endif

