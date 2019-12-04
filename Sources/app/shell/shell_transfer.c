/**
 * @Title: shell_transfer.c
 * @author yuzhimin
 * @date Jul 25, 2017
 * @version V1.0
 */

#include <dbg_uart.h>
#include <string.h>
#include <sys/_stdint.h>

unsigned char g_xModemCommEnable = 0;

/* read from dbg uart */
uint32_t shell_read(uint8_t **bufPtr, uint32_t *length)
{
    static uint8_t recv_buf[256];
    static uint32_t r_pos = 0;
    static uint32_t w_pos = 0;
    uint32_t i, head, len, recv_bytes;

    if (r_pos && r_pos < sizeof(recv_buf))
    {
        if (r_pos < w_pos)
        {
            for (i = r_pos; i < w_pos; i++)
            {
                recv_buf[i - r_pos] = recv_buf[i];
            }

            w_pos = w_pos - r_pos;
        }
    }
    else
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        r_pos = w_pos = 0;
    }

    recv_bytes = dbg_uart_recv(recv_buf + w_pos, sizeof(recv_buf) - w_pos);

    head = len = 0;

    for (i = 0; i < recv_bytes + w_pos; i++)
    {
        if ('\0' == recv_buf[i])
        {
            head = i + 1;
            len = 0;
            continue;
        }

        /* one commmand is found */
        if ((i + 1 < recv_bytes + w_pos) &&
            ('\r' == recv_buf[i]) && ('\n' == recv_buf[i + 1]))
        {
            recv_buf[i] = '\0';
            break;
        }

        len++;
    }

    if (i == 0)  /* recv nothing */
    {
        *bufPtr = NULL;
        *length = 0;
        return 0;
    }
    else if (i < recv_bytes + w_pos)  /* one at commmand is found */
    {
        *bufPtr = &recv_buf[head];
        *length = len;
        i++;

        if (recv_bytes + w_pos - i < 2)     // recv_buf only has one command
        {
            r_pos = w_pos = 0;
        }
        else    // recv_buf has one full command and part of command
        {
            r_pos = i + 1;
            w_pos = w_pos + recv_bytes;
        }

        return w_pos - r_pos;
    }
    else
    {
        /* this is part of one commmand */
        r_pos = i;
        w_pos = w_pos + recv_bytes;
        *bufPtr = NULL;
        *length = 0;
        return 0;
    }
}

/* write to dbg uart */
uint32_t shell_write(uint8_t *buf, uint32_t length)
{
    return dbg_uart_send(buf, length);
}

