#include <stdint.h>
#include <mid/kernel/kernel.h>
#include "dbg_trace.h"
#include <string.h>
#include <flash.h>

static volatile DBG_TRACE_INFO *dbg_trace_info = NULL;

void dbgTraceInit(void)
{
    if (NULL == dbg_trace_info)
    {
        dbg_trace_info = (volatile DBG_TRACE_INFO *)DBG_TRACE_MEM_START;
    }

    memset((void *)dbg_trace_info, 0, DBG_TRACE_MEM_LEN);

    dbg_trace_info->magic = 0x5ecdabed;
}

void dbgTraceTaskId(unsigned int TaskId, unsigned int EventMask, DBG_TRACE_TYPE Type)
{
    if (NULL == dbg_trace_info)
    {
        dbg_trace_info = (volatile DBG_TRACE_INFO *)DBG_TRACE_MEM_START;
    }

    dbg_trace_info->TaskId    = TaskId;
    dbg_trace_info->EventMask = EventMask;
    dbg_trace_info->Type      = Type;

}

void dbgTraceDump(void)
{
    if (NULL == dbg_trace_info)
    {
        dbg_trace_info = (volatile DBG_TRACE_INFO *)DBG_TRACE_MEM_START;
    }

    printf("\r\n-----------------dbg trace info begin--------------------\r\n");

    if (dbg_trace_info->magic != 0x5ecdabed)
    {
        printf("trace info is null \r\n");
    }
    else
    {
        printf("last task id:%u\r\n",         dbg_trace_info->TaskId);
        printf("event mask:0x%08x\r\n",       dbg_trace_info->EventMask);
        printf("last task type:0x%02x\r\n",   dbg_trace_info->Type);
    }

    printf("\r\n-----------------dbg trace info end----------------------\r\n");
}


