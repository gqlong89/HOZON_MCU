/**
* @Title: ipc_spi_list.c
* @author yuzhimin
* @date Dec 22, 2017
* @version V1.0
*/

#include <assert.h>
#include <interrupt_manager.h>
#include <mid/ipc_spi/ipc_spi_list.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <string.h>

IPC_FRAME_T spi_frame[IPC_SPI_FRAME_MAX] __attribute__((section(".data.spibuf")));
list_t ipc_flst;
list_t ipc_slst;
list_t ipc_rlst;


void ipc_lst_init(void)
{
    unsigned int i;
    list_init(&ipc_flst);
    list_init(&ipc_slst);
    list_init(&ipc_rlst);
    irq_cnt[11] = IPC_SPI_FRAME_MAX;

    for (i = 0; i < IPC_SPI_FRAME_MAX; i++)
    {
        list_insert_before(&ipc_flst, &spi_frame[i].link);
    }
}

#if 0
IPC_FRAME_T *ipc_get_freeframe(unsigned char type)
{
    list_t *node = NULL;
    IPC_FRAME_T *frame = NULL;
    int fcnt;

    if (type == SPI_CAN_FRAME) //can frame
    {
        LIST_LOCK();
        node = list_get_first(&ipc_flst);
        LIST_UNLOCK();
    }
    else if (type == SPI_APP_FRAME)  // app frame
    {
        LIST_LOCK();

        if (list_count(&ipc_flst) > 4)
        {
            node = list_get_first(&ipc_flst);
        }

        LIST_UNLOCK();
    }

    fcnt = list_count(&ipc_flst);
    irq_cnt[11] = MIN(irq_cnt[11], fcnt);

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

void ipc_put_freeframe(IPC_FRAME_T *frame)
{
    assert(frame);
    LIST_LOCK();
    list_insert_before(&ipc_flst, &frame->link);
    LIST_UNLOCK();

}

IPC_FRAME_T *ipc_get_sendframe(void)
{
    list_t *node;
    IPC_FRAME_T *frame = NULL;
    int scnt;
    LIST_LOCK();

    if ((node = list_get_first(&ipc_slst)) != NULL)
    {
        frame = list_entry(node, IPC_FRAME_T, link);
    }

    scnt = list_count(&ipc_slst);
    irq_cnt[12] = MAX(irq_cnt[12], scnt);
    LIST_UNLOCK();
    return frame;
}

void ipc_put_sendframe(IPC_FRAME_T *frame)
{
    assert(frame);
    LIST_LOCK();
    list_insert_before(&ipc_slst, &frame->link);
    LIST_UNLOCK();
}

IPC_FRAME_T *ipc_get_recvframe(void)
{
    list_t *node = NULL;
    IPC_FRAME_T *frame = NULL;
    LIST_LOCK();

    if ((node = list_get_first(&ipc_rlst)) != NULL)
    {
        frame = list_entry(node, IPC_FRAME_T, link);
    }

    LIST_UNLOCK();
    return frame;
}

void ipc_put_recvframe(IPC_FRAME_T *frame)
{
    assert(frame);
    LIST_LOCK();
    list_insert_before(&ipc_rlst, &frame->link);
    LIST_UNLOCK();
}

#endif

void ipc_list_show(void)
{
    shellPrintf(" free list =[%d]\r\n", list_count(&ipc_flst));
    shellPrintf(" send list =[%d]\r\n", list_count(&ipc_slst));
    shellPrintf(" recv list =[%d]\r\n", list_count(&ipc_rlst));
}

