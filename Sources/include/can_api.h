/****************************************************************
 file:         can_api.h
 description:  the header file of can definition
 date:         2017/01/07
 author        yuzhimin
 ****************************************************************/

#ifndef __CAN_API_H__
#define __CAN_API_H__

#include <sys/_stdint.h>
#include <flexcan_hal.h>

/***** API for the application layer *****/
/* Type definition to hold a CAN message */
#pragma pack(1)
typedef struct
{
    unsigned int  uptime;
    unsigned int  miscUptime;
    unsigned int  MsgID;       /* CAN Message ID (11-bit or 29-bit) */
    unsigned int  type  : 8;   /* can message or can Tag */
    unsigned int  dlc   : 7;   /* data length */
    unsigned int  brs   : 1;   /* bit rate switch */
    unsigned int  esi   : 1;   /* error state indicator */
    unsigned int  canFD : 1;   /* canFD flag */
    unsigned int  isRTR : 1;   /* RTR flag */
    unsigned int  isEID : 1;   /* EID flag */
    unsigned int  port  : 3;   /* can port */
    unsigned int  res   : 9;
    unsigned char Data[64];
} CAN_MSG;                    
#pragma pack(0)


/* Type definition to send a CAN message */
typedef struct
{
    unsigned int : 13;
    unsigned int PAD : 8;
    unsigned int DLC : 7;
    unsigned int BRS : 1;
    unsigned int canFD:1;
    unsigned int isRTR : 1;
    unsigned int isEID : 1;
    unsigned int MsgID;     /* CAN Message ID (11-bit or 29-bit) */
    unsigned char Data[64]; /* CAN Message Data Bytes 0-63 */
} CAN_SEND_MSG;


/***** definition for the middle layer of flexcan and spican *****/
#define NATIVE_CAN_MAX                  3
#define EXTENDED_CAN_MAX                2

enum
{
    CAN_PORT0 = 0U,  // flexcan 0
    CAN_PORT1,       // flexcan 1
    CAN_PORT2,       // flexcan 2
    CAN_PORT3,       // spi can 0
    CAN_PORT4,       // spi can 1
    CAN_PORT_MAX,
};
typedef enum
{
    RX_CAN_STD = 0,        
    RX_CAN_EXT = 1,       
    TX_CAN_UDS = 2,            
    TX_CAN_NM = 3,
    TX_CAN_GENERAL1 = 4,
    TX_CAN_GENERAL2 = 5,
    TX_CAN_BUFF_MAX = 6,    
} FXCAN_BUFF_TYPE;         /*support CAN FD*/ 


typedef enum
{
    RX_CAN_NORMAL_STD = 0,
    RX_CAN_NORMAL_EXT = 1,
    TX_CAN_NORMAL_UDS = 2,            // 用于发送UDS报文
    TX_CAN_NORMAL_ECU_UPG = 3,
    TX_CAN_NORMAL_NM_OSEK = 4,
    TX_CAN_NORMAL_NM_AUTOSAR = 5,
        
    TX_CAN_NORMAL_GENERAL1 = 6,
    TX_CAN_NORMAL_GENERAL2 = 7,
    TX_CAN_NORMAL_GENERAL3 = 8,
    TX_CAN_NORMAL_GENERAL4 = 9,
    TX_CAN_NORMAL_GENERAL5 = 10,
    TX_CAN_NORMAL_GENERAL6 = 11,
    TX_CAN_NORMAL_GENERAL7 = 12,
    TX_CAN_NORMAL_GENERAL8 = 13,
    TX_CAN_NORMAL_GENERAL9 = 14,
    TX_CAN_NORMAL_BUFF_MAX = 15,      //used tx general buff max
} FXCAN_NORMAL_BUFF_TYPE;




typedef enum
{
    SPI_CAN_GENERAL0,       //spi can mailbox 0
    SPI_CAN_GENERAL1,       //spi can mailbox 1
    SPI_CAN_GENERAL2,       //spi can mailbox 2
    SPI_CAN_BUFF_MAX,
} SPICAN_BUFF_TYPE;



#if 0
#pragma pack(1)
typedef struct
{
    unsigned int uptime;
    unsigned int miscUptime;
    unsigned char type;
    unsigned char dlc;
    unsigned int MsgID; /* CAN Message ID (11-bit or 29-bit) */
    unsigned int DataA; /* CAN Message Data Bytes 0-3 */
    unsigned int DataB; /* CAN Message Data Bytes 4-7 */
} CAN_MSG_BUF;
#pragma pack(0)
#endif

#pragma pack(1)
typedef struct
{
    unsigned int  uptime;
    unsigned int  miscUptime;
    unsigned int  MsgID;       /* CAN Message ID (11-bit or 29-bit) */
    unsigned int  type  : 8;   /* can message or can Tag */
    unsigned int  dlc   : 7;   /* data length */
    unsigned int  brs   : 1;   /* bit rate switch */
    unsigned int  esi   : 1;   /* error state indicator */
    unsigned int  canFD : 1;   /* canFD flag */
    unsigned int  isRTR : 1;   /* RTR flag */
    unsigned int  isEID : 1;   /* EID flag */
    unsigned int  port  : 3;    /* can port */
    unsigned int  res   : 9;
    unsigned int  DataA;       /* CAN Message Data Bytes 0-3 */
    unsigned int  DataB;       /* CAN Message Data Bytes 4-7 */
    unsigned char Data[64-4-4];
} CAN_MSG_BUF;
#pragma pack(0)



typedef struct
{
    unsigned char type;
    unsigned char port;
    unsigned short cycles;
    unsigned int per_time;
    unsigned int cur_val;
    CAN_SEND_MSG mes;
} MES_PARAM_STR;

void can_device_init(void);
//void can_init(uint8_t canport, int16_t baud, flexcan_operation_modes_t can_mode);
void can_deinit(uint8_t canport);
void can_deinit_all(void);

uint8_t can_send(uint8_t canport, uint8_t can_mail, CAN_SEND_MSG *canMsg);
void can_mes_set(CAN_SEND_MSG *canMsg, uint8_t port, uint32_t cycles, uint32_t period);
void can_mes_stop(CAN_SEND_MSG *canMsg, uint8_t port);

#endif
