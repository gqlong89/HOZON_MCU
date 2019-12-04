

#ifndef CAN_SENDFIFIO_H_
#define CAN_SENDFIFIO_H_


#define APP_CANSENDFIFO_NUM   (10)



typedef struct
{
    CAN_SEND_MSG buf[APP_CANSENDFIFO_NUM];
    unsigned char p_in;                // –¥÷∏’Î
    unsigned char p_out;               // ∂¡÷∏’Î
} CAN_SENDFIFO_ST;


extern void APP_ClearTxFifo(unsigned char canport);
extern unsigned char APP_CANSend(unsigned char canport, CAN_SEND_MSG *canMsg);
extern unsigned char APP_CANSendFifoHandle(unsigned char canport);
extern void busOff_RecoverSend(unsigned char canport);

#endif




