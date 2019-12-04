/**
* @Title: ipc_spi_list.h
* @author yuzhimin
* @date Dec 26, 2017
* @version V1.0
*/
#ifndef __IPC_SPI_LIST_H__
#define __IPC_SPI_LIST_H__

#include <assert.h>
#include <interrupt_manager.h>
#include <ipc_spi.h>
#include <list.h>
#include <stddef.h>
#include <S32K144.h>

typedef struct IPC_MSG_HDR
{
    unsigned char flag;
    unsigned char cs;
    unsigned short len: 12;
    unsigned short ctl: 2;
    unsigned short chn: 2;
} IPC_MSG_HDR;

typedef struct
{
    IPC_MSG_HDR   header;
    unsigned char body[IPC_SPI_BODY];
} IPC_MSG_T;

typedef struct IPC_FRAME_T
{
    union
    {
        IPC_MSG_T msg;
        unsigned char pack[IPC_SPI_FRAME_LEN];
    };
    list_t link;
} IPC_FRAME_T;

#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))

#define LIST_LOCK()          INT_SYS_DisableIRQGlobal()
#define LIST_UNLOCK()        INT_SYS_EnableIRQGlobal()

extern IPC_FRAME_T spi_frame[IPC_SPI_FRAME_MAX];
extern list_t ipc_flst;
extern list_t ipc_slst;
extern list_t ipc_rlst;

void ipc_lst_init(void);

static inline IPC_FRAME_T *ipc_get_freeframe(unsigned char type)
{
    list_t *node = NULL;
    IPC_FRAME_T *frame = NULL;
    int fcnt;

    LIST_LOCK();

    if (type == SPI_CAN_FRAME) //can frame
    {
        node = list_get_first(&ipc_flst);
    }
    else if (type == SPI_APP_FRAME)  // app frame
    {
        if (list_count(&ipc_flst) > 4)
        {
            node = list_get_first(&ipc_flst);
        }
    }
    else        // recv frame
    {
        if (list_count(&ipc_flst) > 3)
        {
            node = list_get_first(&ipc_flst);
        }
    }

    fcnt = list_count(&ipc_flst);
    irq_cnt[11] = MIN(irq_cnt[11], (unsigned int)fcnt);
    LIST_UNLOCK();

    if (NULL == node)
    {
        //printk(PRINTK_LEVEL_ERROR, "no more free frame for[%u]", type);
    }
    else
    {
        frame = list_entry(node, IPC_FRAME_T, link);
    }

    return frame;
}

static inline void ipc_put_freeframe(IPC_FRAME_T *frame)
{
    assert(frame);
    LIST_LOCK();
    list_insert_before(&ipc_flst, &frame->link);
    LIST_UNLOCK();
}

static inline IPC_FRAME_T *ipc_get_sendframe(void)
{
    list_t *node;
    IPC_FRAME_T *frame = NULL;
    int scnt;
    LIST_LOCK();

    if ((node = list_get_first(&ipc_slst)) != NULL)
    {
        frame = list_entry(node, IPC_FRAME_T, link);
        scnt = list_count(&ipc_slst);
        irq_cnt[12] = MAX(irq_cnt[12], (unsigned int)(scnt + 1));
    }

    LIST_UNLOCK();
    return frame;
}

static inline void ipc_put_sendframe(IPC_FRAME_T *frame)
{
    assert(frame);
    LIST_LOCK();
    list_insert_before(&ipc_slst, &frame->link);
    LIST_UNLOCK();
}

static inline int ipc_get_recvframe_cnt(void)
{
    int rcnt;
    LIST_LOCK();
    rcnt = list_count(&ipc_rlst);
    LIST_UNLOCK();
    return rcnt;
}

static inline IPC_FRAME_T *ipc_get_recvframe(void)
{
    list_t *node = NULL;
    IPC_FRAME_T *frame = NULL;
    int rcnt;
    LIST_LOCK();

    if ((node = list_get_first(&ipc_rlst)) != NULL)
    {
        frame = list_entry(node, IPC_FRAME_T, link);
        rcnt = list_count(&ipc_rlst);
        irq_cnt[13] = MAX(irq_cnt[13], (unsigned int)(rcnt + 1));
    }

    LIST_UNLOCK();
    return frame;
}

static inline void ipc_put_recvframe(IPC_FRAME_T *frame)
{
    assert(frame);
    LIST_LOCK();
    list_insert_before(&ipc_rlst, &frame->link);
    LIST_UNLOCK();
}

#endif

