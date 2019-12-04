#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <mid/msg/msg_parse.h>

/**********************************************************************
 function:     msg_convert
 description:  Escape a string for transmission
 input:        const char *src, to escape the string
               int src_len, length of to escape the string
               int dst_len, Save the escaped string buffer size
               char marker, Characteristics of character
               char esc, Escape character
 output:       char *dst, escaped string
 return:       positive value indicates success,
               and the value is the length of escaped string;
               negative value indicates failed
 ***********************************************************************/
static int msg_convert(const unsigned char *src, int src_len,
                       unsigned char *dst, int dst_len,
                       unsigned char marker, unsigned char esc)
{
    int i = 0, j = 0, ret = 0;

    if (src == NULL || dst == NULL)
    {
        return -1;
    }

    for (i = 0; i < src_len && j < dst_len; i++, j++)
    {
        if (src[i] == marker)
        {
            if (j + 1 < dst_len)
            {
                dst[j++] = esc;
                dst[j] = 0x02;
            }
            else
            {
                ret = -1;
                break;
            }
        }
        else if (src[i] == esc)
        {
            if (j + 1 < dst_len)
            {
                dst[j++] = esc;
                dst[j] = 0x01;
            }
            else
            {
                ret = -1;
                break;
            }
        }
        else
        {
            dst[j] = src[i];
        }
    }

    if (0 <= ret)
    {
        ret = j;
    }

    if (i != src_len)
    {
        ret = -1;
    }

    return ret;
}

/**********************************************************
 function:     msg_reverse
 description:  Reduction of a string
 input:        char *buf, unreduced string
               int len, length of to unreduced string
               unsigned char marker, Characteristics of character
               unsigned char esc, Escape character
 output:       char *buf, reduced string
 return:       positive value indicates success,
               and the value is the length of reduced string;
               -1 indicates reduced failed
 **********************************************************/
static int msg_reverse(unsigned char *buf, int len, unsigned char marker, unsigned char esc)
{
    int i, j;

    if (buf == NULL || len <= 0)
    {
        return -1;
    }

    /* reverse */
    for (j = 0, i = 0; i < len; i++)
    {
        if (buf[i] != esc)
        {
            buf[j++] = buf[i];
        }
        else
        {
            i += 1;

            if (buf[i] == 0x01)
            {
                buf[j++] = esc;
            }
            else if (buf[i] == 0x02)
            {
                buf[j++] = marker;
            }
            else
            {
                taskPrintf(TASK_LEVEL_ERROR, "unkown type,0x%x", buf[i]);
                return -1;
            }
        }
    }

    return j;
}

/*********************************************
 function:     msg_check
 description:  Escape reduction dccp package, and check whether it is right
 input:        unsigned unsigned char *buf_ptr, unreduced dccp packet
               unsigned int *len_ptr,  size of unreduced dccp packet
 output:       unsigned unsigned char *buf_ptr, reduced dccp packet
               unsigned int *len_ptr,  size of reduced dccp packet
 return:       0 indicates reduced successful
               -1 indicates reduced failed
 *********************************************/
static int msg_check(unsigned char *buf, int *len)
{
    unsigned char cs = 0x0;
    int ilen = *len;

    ilen = msg_reverse(buf, ilen, DCCP_PKG_MARKER, DCCP_PKG_ESC);
    *len = ilen;

    if (ilen < DCCP_PKG_CS_SIZE)  /* length */
    {
        taskPrintf(TASK_LEVEL_ERROR, "header is broken! len=%d", ilen);
        return -1;
    }

    /* dcom_checksum */
    cs = msg_checksum(buf, ilen - DCCP_PKG_CS_SIZE, cs);

    if (cs != buf[ilen - DCCP_PKG_CS_SIZE])
    {
        taskPrintf(TASK_LEVEL_ERROR, "checksum unmatch!, cs:%u,%u", buf[ilen - DCCP_PKG_CS_SIZE],
                   cs);
        return -1;
    }

    return 0;
}

static int msg_find_dccp(const unsigned char *rxbuf, int len)
{
    int i;

    for (i = 0; i < len; ++i)
    {
        if (rxbuf[i] == DCCP_PKG_MARKER)
        {
            return i;
        }
    }

    return -1;
}

#if 0  // Optimized for inline functions.
/*********************************************
 function:     msg_checksum
 description:  Additive to calculate the checksum
 input:        const unsigned char* buf, to calculate the checksum of a string
               int len, length of the string
               unsigned char cs, initial dcom_checksum, usually as the last checksum result
 output:       none
 return:       checksum result
 *********************************************/
unsigned char msg_checksum(const unsigned char *buf, int len, unsigned char cs)
{
    int i;
    unsigned char checksum = cs;

    if (buf == NULL || len <= 0)
    {
        return 0;
    }

    for (i = 0; i < len; ++i)
    {
        checksum = checksum ^ buf[i];
    }

    return checksum;
}
#endif

/*******************************************************************
 function:     msg_encode
 description:  encode message
 input:        unsigned char *imsg, the message need to encode;
               int ilen, the message length which need to encode;
               int *olen, the encoded message buffer length
 output:       unsigned char *omsg, the encoded message
               unsigned int *olen, the encoded message length
 return:       0 indicates success;
               others indicates failed
 ********************************************************************/
int msg_encode(unsigned char *imsg, int ilen,
               unsigned char *omsg, int olen,
               unsigned char flag,  unsigned char *base_cs)
{
    int ret, data_len = 0;
    int offset;

    if (FIRST_SEG == flag)
    {
        *base_cs = 0;
        offset = DCCP_PKG_MARKER_SIZE;
        omsg[0] = DCCP_PKG_MARKER;
    }
    else
    {
        offset = 0;
    }

    if (ilen > 0)
    {
        data_len = msg_convert(imsg, ilen,
                               omsg + offset, olen - offset,
                               DCCP_PKG_MARKER, DCCP_PKG_ESC);

        if (-1 == data_len)
        {
            taskPrintf(TASK_LEVEL_ERROR, "msg_encode failed");
            return -1;
        }

        *base_cs = msg_checksum(imsg, ilen, *base_cs);
    }

    data_len += offset;

    if (LAST_SEG == flag)
    {
        ret = msg_convert(base_cs, DCCP_PKG_CS_SIZE, omsg + data_len,
                          olen - data_len - DCCP_PKG_MARKER_SIZE,
                          DCCP_PKG_MARKER, DCCP_PKG_ESC);

        if (-1 == ret)
        {
            taskPrintf(TASK_LEVEL_ERROR, "msg_encode cs failed");
            return -1;
        }
        else
        {
            data_len += ret;
        }

        omsg[data_len] = DCCP_PKG_MARKER;
        data_len++;
    }

    return data_len;
}

/*******************************************************************
 function:     msg_decode
 description:  decode the message
 input:        MSG_RX *rx, message buffer;
               msg_handler msg_proc, message process function;
 output:       none
 return:       none
 ********************************************************************/
void msg_decode(MSG_RX *rx, msg_handler msg_proc)
{
    assert(rx || msg_proc);
    int ret, len, i;
    int r_pos = 0, start_pos = -1, end_pos = -1;

    while (r_pos < rx->used)
    {
        ret = msg_find_dccp(rx->data + r_pos, rx->used - r_pos);

        /* found nothing, read the next message */
        if (ret < 0)
        {
            r_pos = rx->used;
        }
        /* start tag is found, continue searching the end tag */
        else if ((ret >= 0) && (-1 == start_pos))
        {
            start_pos = r_pos + ret;
            r_pos = start_pos + 1;
        }
        /* both start tag and end tag are found, forward the message and then search the next
           start tag again */
        else
        {
            end_pos = r_pos + ret;
            r_pos = end_pos + 1;
            len = end_pos - start_pos - DCCP_PKG_CS_SIZE; /* the message length */

            if (len <= 0 || 0 != msg_check(rx->data + start_pos + DCCP_PKG_MARKER_SIZE, &len))
            {
                start_pos = end_pos;
                end_pos = -1;
            }
            else
            {
                msg_proc(rx->data + start_pos + DCCP_PKG_MARKER_SIZE, len - DCCP_PKG_CS_SIZE);
                start_pos = -1;
                end_pos = -1;
            }
        }
    }

    /* nothing is found in the buffer, ignore it */
    if (-1 == start_pos)
    {
        rx->used = 0;
    }
    /* start tag is found, but the end tag is not found in a very long buffer, ignore it */
    else if ((r_pos - start_pos) >= (rx->size - 16))
    {
        rx->used = 0;
    }
    /* start tag is found */
    else
    {
        len = rx->used - start_pos;

        if (start_pos != 0)
        {
            for (i = 0; i < len; i++)
            {
                rx->data[i] = rx->data[i + start_pos];
            }
        }

        rx->used = len;
    }
}



