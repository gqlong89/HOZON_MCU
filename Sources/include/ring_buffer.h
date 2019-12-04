/****************************************************************
 file:         ring_buffer.h
 description:  the header file of ring_buffer definition
 date:         2016/9/22
 author        yuzhimin
 ****************************************************************/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <sys/_stdint.h>
#include <assert.h>
#include <string.h>

//判断x是否是2的次方
#define is_power_of_2(x)((x) != 0 &&(((x) &((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b)(((a) <(b)) ?(a) :(b))

typedef struct
{
    uint8_t *buffer;        //缓冲区
    uint32_t size;           //缓冲区大小
    uint32_t in;             //入口位置
    uint32_t out;            //出口位置
} ring_buffer;

/*********************************************
 function:     rb_init
 description:  ring_buffer initialization
 input:        char *buffer,pointer to a buffer space
 unsigned int size,buffer space size
 output:       none
 return:       ring_buffer  pointer of ring_buffer type
 *********************************************/
static inline ring_buffer *rb_init(ring_buffer *ring_buf, uint8_t *buffer, uint32_t size)
{
    assert(ring_buf || buffer);

    if (!is_power_of_2(size))
    {
        return NULL;
    }

    memset(buffer, 0, size);
    ring_buf->buffer = buffer;
    ring_buf->size = size;
    ring_buf->in = 0;
    ring_buf->out = 0;

    return ring_buf;
}

/*********************************************
 function:     rb_used_len
 description:  Idle buffer size
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 output:       none
 return:       the size of idle buffer
 *********************************************/
static inline uint32_t rb_used_len(const ring_buffer *ring_buf)
{
    return (ring_buf->in - ring_buf->out);
}

/**************************************************************
 function:     rb_unused_len
 description:  Idle buffer size
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 output:       none
 return:       the size of idle buffer
 ***************************************************************/
static inline uint32_t rb_unused_len(const ring_buffer *ring_buf)
{
    return (ring_buf->size - (ring_buf->in - ring_buf->out));
}

/**************************************************************
 function:     rb_empty
 description:  chech whether the ring buffer is empty
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 output:       none
 return:       1 indicates the ring buffer is empty;
 0 indicates the ring buffer is not empty;
 ***************************************************************/
static inline uint32_t rb_empty(const ring_buffer *ring_buf)
{
    return (ring_buf->in == ring_buf->out);
}

/*********************************************
 function:     rb_get
 description:  get data from ring buffer
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 unsigned int size, data length
 output:       char *buffer, save the data to data buffer
 return:       the actual size of get out data
 *********************************************/
static inline uint32_t rb_get(ring_buffer *ring_buf, uint8_t *buffer, uint32_t size)
{
    assert(ring_buf || buffer);

    uint32_t len = 0;
    size = min(size, ring_buf->in - ring_buf->out);
    /* first get the data from fifo->out until the end of the buffer */
    len = min(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)));
    memcpy(buffer, ring_buf->buffer + (ring_buf->out & (ring_buf->size - 1)), len);

    /* then get the rest(if any) from the beginning of the buffer */
    if (size - len)
    {
        memcpy(buffer + len, ring_buf->buffer, size - len);
    }

    ring_buf->out += size;

    if (ring_buf->in == ring_buf->out)
    {
        ring_buf->in = ring_buf->out = 0;
    }

    return size;
}

/**************************************************************
 function:     rb_get
 description:  get data from ring buffer
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 unsigned int size, data length
 output:       char *buffer, save the data to data buffer
 return:       the actual size of get out data
 ***************************************************************/
static inline uint32_t rb_get_ex(ring_buffer *ring_buf, char *buffer,
                                 unsigned int offset,
                                 unsigned int size)
{
    unsigned int len = 0;

    if (ring_buf->out + offset >= ring_buf->in)
    {
        return 0;
    }

    size = min(size, ring_buf->in - (ring_buf->out + offset));
    /* first get the data from fifo->out until the end of the buffer */
    len = min(size, ring_buf->size - ((ring_buf->out + offset) & (ring_buf->size - 1)));
    memcpy(buffer, ring_buf->buffer + ((ring_buf->out + offset) & (ring_buf->size - 1)),
           len);

    /* then get the rest(if any) from the beginning of the buffer */
    if (size - len)
    {
        memcpy(buffer + len, ring_buf->buffer, size - len);
    }

    return size;
}

/*********************************************
 function:     rb_put
 description:  put data to ring_buffer
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 const char *buffer, the put in ring_buffer data
 unsigned int size, data length
 output:       none
 return:       the actual size of put in data
 *********************************************/
static inline uint32_t rb_put(ring_buffer *ring_buf, const uint8_t *buffer, uint32_t size)
{
    assert(ring_buf || buffer);

    uint32_t len = 0;
    size = min(size, ring_buf->size - ring_buf->in + ring_buf->out);
    /* first put the data starting from fifo->in to buffer end */
    len = min(size, ring_buf->size - (ring_buf->in & (ring_buf->size - 1)));
    memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);

    /* then put the rest(if any) at the beginning of the buffer */
    if (size - len)
    {
        memcpy(ring_buf->buffer, buffer + len, size - len);
    }

    ring_buf->in += size;

    return size;
}

/*********************************************
 function:     rb_clean
 description:  clear ring_buffer data
 input:        ring_buffer *ring_buf, the pointer of ring_buffer type
 output:       none
 return:       none
 *********************************************/
static inline void rb_clean(ring_buffer *ring_buf)
{
    assert(ring_buf);
    memset(ring_buf->buffer, 0, ring_buf->size);
    ring_buf->in = ring_buf->out = 0;
    return;
}


#endif /* !__RING_BUFFER_H__ */

