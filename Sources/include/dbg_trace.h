#ifndef _DBG_TRACE_H_
#define _DBG_TRACE_H_

typedef enum
{
    DBG_TRACE_ENTER = 0x11,
    DBG_TRACE_EXIT  = 0x22,
} DBG_TRACE_TYPE;

typedef struct
{
    unsigned int   magic;
    unsigned int   TaskId;
    unsigned int   EventMask;
    unsigned int   Type;
} DBG_TRACE_INFO;

void dbgTraceInit(void);

void dbgTraceTaskId(unsigned int TaskId, unsigned int EventMask, DBG_TRACE_TYPE Type);

void dbgTraceDump(void);

#endif /* _DBG_TRACE_H_ */

