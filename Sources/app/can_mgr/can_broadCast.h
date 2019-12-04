
#ifndef CAN_BROADCAST_H_
#define CAN_BROADCAST_H_

/**********************************************
*�ļ����ƣ�can_boradCast.h
*ʱ�䣺    2019��1��11��11:39:11
*���ߣ� INTEST_DSC

**********************************************/

/*����汾�ţ�Ӳ���汾��*/
#define    SW_VERSION_SUPPLIER            (0x0A)
#define    SW_VERSION_MAIN                (0x01)
#define    SW_VERSION_SUB1                (0)
#define    SW_VERSION_SUB2                (0x01)

#define    HW_VERSION_SUPPLIER            (0x0A)
#define    HW_VERSION_MAIN                (0x01)
#define    HW_VERSION_SUB1                (0)
#define    HW_VERSION_SUB2                (0x01)


#define     MAX_BC_EVENT_NUMBER    (5) //�¼��Թ㲥�������
#define     MAX_BC_CYCLE_NUMBER    (5) //�����Թ㲥�������
#define     MAX_BC_CE_NUMBER       (5) //����Թ㲥�������
#define     MAX_BC_ACTIVE_NUMBER   (1) //ʹ���Թ㲥�������



#define     BC_TYPE_INVALID   (0)    // �Ƿ��Ĺ㲥����
#define     BC_TYPE_EVENT     (1)    // �¼��Թ㲥 
#define     BC_TYPE_CYCLE     (2)    // �����Թ㲥 
#define     BC_TYPE_CE        (3)    // ����ԣ������¼��ԣ��㲥 
#define     BC_TYPE_ACTIVE    (4)    // ʹ���Թ㲥 


/*���ݷ�װ����ö��*/
typedef enum
{
    DATA_TYPE_INVALID = 0,
    DATA_TYPE_CMX_FIX,
	HZ_TIME,
	HZ_0X440,
	HZ_0X445,
	HZ_0X526,
	HZ_0X3D2,
    DATA_TYPE_USER1,
    DATA_TYPE_USER2,
    DATA_TYPE_USER3,
    DATA_TYPE_TIME
} DATA_TYPE_E;


#define     BC_FOREVER1          (0x7ffffffe)   // ��Զ����

#define     BC_USED              (1)
#define     BC_USEDNO            (0)

#define     SEND_TIMER0          (1<<0)
#define     SEND_TIMER1          (1<<1)
#define     SEND_TIMER2          (1<<2)
#define     SEND_TIMER3          (1<<3)
#define     SEND_TIMER4          (1<<4)






typedef struct
{
    unsigned int  id;           //  ID
    unsigned char type;        //   �㲥����
    unsigned char port;         // ���͵Ķ˿�
    unsigned int  num_C;    //  �����Թ㲥����
    unsigned int  cycle_C;  // �����Թ㲥 ����
    unsigned char data_C_type;// ������������������
    unsigned char data_C[8];   //  Ҫ���͵�����������
    unsigned char mask_C[8];   //  ����������Ҫ���͵�����
} BC_SET_ST;


typedef struct
{
    unsigned int id;
    unsigned char port;
    unsigned int num_C;
    unsigned int  cycle_C;
    unsigned char data_C_type;
    unsigned char data_C[8];
    unsigned char mask_C[8];
    unsigned char used;
} BC_CYCLE_ST; //���������¼��ṹ��



typedef struct
{
    unsigned int id;
    unsigned char port;
    /*�����ѡ������ ��Ҫ��Ӧ�ò���*/
    unsigned char num_Event;        //�����¼�����Ҫ���ͱ��ĵĴ���
    unsigned int  cycle_Event;     //�����¼�����Ҫ���ͱ��ĵ�����
    unsigned char  data_Event[8];
    unsigned char  len_Event;
    unsigned int  eventIsGoing;       // ���ڽ���ʱ

    unsigned char used;
} BC_ENENT_ST; //�¼��Թ㲥 �ṹ��

typedef struct
{
    unsigned int id;
    unsigned char port;
    unsigned int num_C;
    unsigned int  cycle_C;
    unsigned char data_C_type;
    unsigned char data_C[8];
    unsigned char mask_C[8];

    unsigned char num_Event;        //�����¼�����Ҫ���ͱ��ĵĴ���
    unsigned int  cycle_Event;     //�����¼�����Ҫ���ͱ��ĵ�����
    unsigned char data_Event[8];
    unsigned char len_Event;
    unsigned int  eventIsGoing;       // ���ڽ���ʱ

    unsigned char used;
} BC_CE_ST; //��� ���¼��ṹ��

typedef struct
{
    unsigned int id;
    unsigned char port;
    unsigned char used;

    /*ûд�꣬û�о���Ӧ�ã���֪����Ҫ���ݲ�д*/
} BC_ACTIVE_ST;

extern void set_BCParam_Manual(void);
extern unsigned char BC_Init(void);
extern void first_BC(unsigned char canport);

extern unsigned char start_BC_timerByID(unsigned char canport, unsigned int id);
extern void start_BC_timerByChl(unsigned char canport);
extern unsigned char stop_BC_timerByID(unsigned char canport, unsigned int id);
extern void stop_BC_timerByChl(unsigned char canport);

extern void APP_SendHandle(void);
extern unsigned char BC_Event_handle(unsigned int id ,               //  ID
                                     unsigned char port,
                                     unsigned char len,          //�¼��Ա������ݳ���
                                     unsigned char *pdata,       // �¼��Ա�������
                                     unsigned char num_Event,    // �����Ա��ķ��ʹ���
                                     unsigned int cycle_Event);   //

extern unsigned char BC_CE_handle(unsigned int id,
                                  unsigned char port,
                                  unsigned char len,
                                  unsigned char *pdata,
                                  unsigned char num_Event,
                                  unsigned int  cycle_Event);

extern void saveInf_dearCar0x221(unsigned char mask, unsigned char value);

#endif

