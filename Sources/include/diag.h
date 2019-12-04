/****************************************************
 *             diagnose header file                 *
 *                             www.intest.com.cn    *
 ****************************************************/
#ifndef _DIAG_H
#define _DIAG_H

#define MAX_CAR_TYPE        10//((50)
#define MAX_CARTYPE_LEN     10//(20)
#define MAX_MODULE          10//(100)
#define MAX_MODULE_LEN      10//(20)
#define MAX_CMD             10//(200)
#define MAX_SUBCMD          (8)
#define MAX_SUBCMD_LEN      (8)

#define DIAG_OK             (0)
#define DIAG_TIMEOUT        (1)
#define DIAG_NAK            (2)

typedef struct
{
    unsigned int totalNo;
    unsigned char type[MAX_CAR_TYPE][MAX_CARTYPE_LEN];
} diagCarTypeStructure;

typedef struct
{
    unsigned int totalNo;
    unsigned char module[MAX_MODULE][MAX_MODULE_LEN];
    unsigned int carIndex[MAX_MODULE];
} diagModuleStructure;

typedef struct
{
    unsigned int used;
    unsigned int mdlIndex;
    unsigned char cmdType; /* G / C / D */
    unsigned int portNum;
    unsigned int portAttr;
    unsigned int diagAddr;
    unsigned int ecuAddr;
    unsigned int cmdTotal;
    unsigned int cmdLen[MAX_SUBCMD];
    unsigned char cmd[MAX_SUBCMD][MAX_SUBCMD_LEN];
} diagCmdListStructure;

#endif
