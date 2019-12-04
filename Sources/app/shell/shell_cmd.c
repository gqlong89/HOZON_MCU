/**
 * @Title: shell_cmd.c
 * @author yuzhimin
 * @date Jul 25, 2017
 * @version V1.0
 */

#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <shell.h>
#include <string.h>

static SHELL_CMD_TABLE shell_cmd_tbl;

/****************************************************************
 * @function:     shell_cmd_register
 * @description:  register shell commmand
 * @input:        char *name, cmd name;
 *                hander_fun *handler, hander function;
 * @output:       none
 * @return:       0 indicates success;
 *                others indicates failed
 *****************************************************************/
int shell_cmd_register(char *name, hander_fun handler)
{
    int name_len;

    if (NULL == name || NULL == handler)
    {
        taskPrintf(TASK_LEVEL_ERROR, "invalid parameter");
        return SHELL_INVALID_PARAMETER;
    }

    name_len = strlen(name);

    if (name_len >= SHELL_MAX_CMD_NAME_LEN)
    {
        taskPrintf(TASK_LEVEL_ERROR, "the length of name is overflow");
        return SHELL_INVALID_PARAMETER;
    }

    if (shell_cmd_tbl.cnt >= SHELL_MAX_CMD_NUM)
    {
        taskPrintf(TASK_LEVEL_ERROR, "shell table overflow");
        return SHELL_CMD_TABLE_OVERFLOW;
    }

    shell_cmd_tbl.cmd[shell_cmd_tbl.cnt].handler = handler;
    strcpy(shell_cmd_tbl.cmd[shell_cmd_tbl.cnt].name, name);

    shell_cmd_tbl.cnt++;

    return 0;
}

/****************************************************************
 * @function:     shell_cmd_help
 * @description:  print the help info of all command
 * @input:        none
 * @output:       none
 * @return:       0 indicates success;
 *                others indicates failed
 *****************************************************************/
static int shell_cmd_help(void)
{
    unsigned int i;
    char cmd = '!';
    char *argv[SHELL_MAX_CMD_PARA_NUM];
    argv[0] = &cmd;

    shellPrintf(" shell command list(cmd num:%u):", shell_cmd_tbl.cnt);

    for (i = 0; i < shell_cmd_tbl.cnt; i++)
    {
        argv[1] = shell_cmd_tbl.cmd[i].name;
        shell_cmd_tbl.cmd[i].handler(2, (unsigned char **) argv);
    }

    return 0;
}

/****************************************************************
 * @function:     shell_cmd_proc
 * @description:  process shell command
 * @input:        unsigned char *cmd, cmd and para
 * @output:       unsigned int len, cmd and para length
 * @return:       none
 *****************************************************************/
void shell_cmd_proc(unsigned char *cmd, unsigned int len)
{
    unsigned int i, argc = 0;
    unsigned char *argv[SHELL_MAX_CMD_PARA_NUM] = {0};

    if (0 == len || len > SHELL_MAX_CMD_LEN)
    {
        taskPrintf(TASK_LEVEL_ERROR, "invalid len:%u", len);
        return;
    }

    //DUMPHEX( TASK_LEVEL_DEBUG, cmd, len );

    argc = 0;

    for (i = 0; i + 1 < len; i++)
    {
        if (0 == i && ' ' != cmd[i])     // first char is valid
        {
            argv[argc] = cmd;
            argc++;
        }

        if (' ' == cmd[i] && ' ' != cmd[i + 1] && '\0' != cmd[i + 1])  /* one para is end */
        {
            if (argc == SHELL_MAX_CMD_PARA_NUM)
            {
                shell_cmd_help();
                return;
            }

            argv[argc] = cmd + i + 1;
            argc++;
        }

        if (' ' == cmd[i])
        {
            cmd[i] = '\0';
        }
    }

    for (i = 0; i < shell_cmd_tbl.cnt; i++)
    {
        if (strcmp((char const *) argv[0], shell_cmd_tbl.cmd[i].name) == 0)
        {
            shell_cmd_tbl.cmd[i].handler(argc - 1, argv + 1);
            break;
        }
    }

    if (i >= shell_cmd_tbl.cnt)
    {
        shell_cmd_help();
    }
}

