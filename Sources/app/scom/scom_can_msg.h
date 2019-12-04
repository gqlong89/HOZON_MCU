/**
 * @Title: scom_can_msg.h
 * @author yuzhimin
 * @date Sep 4, 2017
 * @version V1.0
 */
#ifndef __SCOM_CAN_MSG_H__
#define __SCOM_CAN_MSG_H__

#include <app/scom/scom_tl.h>

enum CAN_REPORT_TYPE
{
    CAN_REPORT_MASK = 0x1u,                //0:stop, 1:send
    CAN_BROADCAST_TIME_MASK = 0x2u,        //1:time frame
    CAN_BROADCAST_VERSION_MASK  = 0x4u,    //1:version frame
    CAN_REMOTE_MASK = 0x8u,                //1: remote frame
};

enum CAN_FRAME_TYPE
{
    CAN_FRAME_STANDARD = 0, /* Standard ID */
    CAN_FRAME_EXTENDED, /* Extended ID */
};

typedef struct
{
    unsigned char port; /* CAN PORT */
    unsigned char used; /* 0: disable; 1: init(first enable); 2: change baudrate */
    unsigned short reserve;
    unsigned int baudrate;
} CAN_PORT_CFG;

typedef struct
{
    unsigned char rtype; /* see also CAN_REPORT_TYPE */
    unsigned char ftype; /* see also CAN_FRAME_TYPE */
    unsigned char port; /* CAN PORT */
    unsigned char dlc; /* Length of Data in Bytes */
    unsigned int id; /* CAN ID */
    unsigned char data[8]; /* CAN Data */
    unsigned int val; /* the unit of interval is ms, the minimum precision is 50ms */
    unsigned int times; /* send times, 0: send until the stop */
} CAN_REPORT_CFG;

void scom_can_proc(SCOM_TL_MSG_HDR *hdr, unsigned char *body);
void scom_can_baud_sync(void);
void scom_can_default_baud_sync(void);


#endif /* __SCOM_CAN_MSG_H__ */
