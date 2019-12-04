/**
 * @Title: shell_task.c
 * @author yuzhimin
 * @date Jul 12, 2017
 * @version V1.0
 */

#include <app/shell/shell_list.h>
#include <app/shell/shell_transfer.h>
#include <dbg_uart.h>
#include <flash.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <shell.h>
#include <string.h>
#include <sys/_stdint.h>

static uint8_t shell_taskid = INVALID_TASK_NUMBER;

unsigned int shell_task(unsigned int mask)
{
    uint32_t len = 0;
    uint8_t *buf = NULL;

    /* init of task */
    if (mask & TASK_SYS_EVENT_INIT)
    {
        shell_taskid = kernelGetTaskID();
        dbg_uart_open(shell_taskid);
        kernelSetTaskLevel(TASK_LEVEL_DEBUG);
        shell_list_init();
        *((volatile unsigned int *)RESET_FLAG_RAM_ADDR) = RESET_FLAG_RAM_USED;
        unsigned int tmp = 0;
        Flash_EEPROM_WriteBuffer((unsigned char *)&tmp, UPGRADE_FLAG_RAM_ADDR, 4);
        *((volatile unsigned int *)UPGRADE_COUNT_RAM_ADDR) = 0;
        taskPrintf(TASK_LEVEL_DEBUG, "TASK_SYS_EVENT_INIT!");
        return 0;
    }

    if (mask & TASK_SYS_EVENT_READ)
    {
        //taskPrintf( TASK_LEVEL_DEBUG, "TASK_SYS_EVENT_READ!" );
        if (0 == shell_read(&buf, &len))
        {
            mask &= (~TASK_SYS_EVENT_READ);
        }

        if (len)
        {
            shell_cmd_proc(buf, len);
            shellPrintf(" # \r\n");
        }
    }
    else if (mask & TASK_SYS_EVENT_SEND)
    {
        dbg_uart_fflush();
    }

    /* unknown event */
    if (mask & TASK_SYS_EVENT_MASK)
    {
        taskPrintf(TASK_LEVEL_ERROR, "Got unknown event %08x!", mask);
        mask = 0;
    }

    return mask;
}

