/**
 * @Title: shell.h
 * @author yuzhimin
 * @date Jul 12, 2017
 * @version V1.0
 */
#ifndef INCLUDE_SHELL_H_
#define INCLUDE_SHELL_H_

#define SHELL_MAX_CMD_NAME_LEN    (12)

//#define SHELL_MAX_CMD_NUM         (30)
#define SHELL_MAX_CMD_NUM (33)

#define SHELL_MAX_CMD_LEN         (512)

#define SHELL_MAX_CMD_PARA_NUM    (7)

typedef int (*hander_fun)(unsigned int argc, unsigned char **argv);

enum SHELL_ERROR_CODE
{
    SHELL_INVALID_CMD = -1,
    SHELL_INVALID_PARAMETER = -2,
    SHELL_CMD_TABLE_OVERFLOW = -3,
    SHELL_CMD_INVALID_LEN = -4,
};

typedef struct
{
    char name[SHELL_MAX_CMD_NAME_LEN];
    hander_fun handler;
} SHELL_CMD;

typedef struct
{
    unsigned int cnt;
    SHELL_CMD cmd[SHELL_MAX_CMD_NUM];
} SHELL_CMD_TABLE;

unsigned int shell_task(unsigned int mask);
int shell_cmd_register(char *name, hander_fun handler);
void shell_cmd_proc(unsigned char *cmd, unsigned int len);

void setshellRedirectBufSize(unsigned int size);
unsigned int getshellRedirectBufSize(void);
void setshellRedirectBuf(char *buf);
void stopshellRedirect(void);

#endif /* INCLUDE_SHELL_H_ */

