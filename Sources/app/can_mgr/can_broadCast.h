
#ifndef CAN_BROADCAST_H_
#define CAN_BROADCAST_H_

/**********************************************
*文件名称：can_boradCast.h
*时间：    2019年1月11日11:39:11
*作者： INTEST_DSC

**********************************************/

/*软件版本号，硬件版本号*/
#define    SW_VERSION_SUPPLIER            (0x0A)
#define    SW_VERSION_MAIN                (0x01)
#define    SW_VERSION_SUB1                (0)
#define    SW_VERSION_SUB2                (0x01)

#define    HW_VERSION_SUPPLIER            (0x0A)
#define    HW_VERSION_MAIN                (0x01)
#define    HW_VERSION_SUB1                (0)
#define    HW_VERSION_SUB2                (0x01)


#define     MAX_BC_EVENT_NUMBER    (5) //事件性广播最多条数
#define     MAX_BC_CYCLE_NUMBER    (5) //周期性广播最多条数
#define     MAX_BC_CE_NUMBER       (5) //混合性广播最多条数
#define     MAX_BC_ACTIVE_NUMBER   (1) //使能性广播最多条数



#define     BC_TYPE_INVALID   (0)    // 非法的广播类型
#define     BC_TYPE_EVENT     (1)    // 事件性广播 
#define     BC_TYPE_CYCLE     (2)    // 周期性广播 
#define     BC_TYPE_CE        (3)    // 混合性（周期事件性）广播 
#define     BC_TYPE_ACTIVE    (4)    // 使能性广播 


/*数据封装类型枚举*/
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


#define     BC_FOREVER1          (0x7ffffffe)   // 永远发送

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
    unsigned char type;        //   广播类型
    unsigned char port;         // 发送的端口
    unsigned int  num_C;    //  周期性广播次数
    unsigned int  cycle_C;  // 周期性广播 周期
    unsigned char data_C_type;// 周期性数据内容类型
    unsigned char data_C[8];   //  要发送的周期性数据
    unsigned char mask_C[8];   //  周期性数据要发送的掩码
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
} BC_CYCLE_ST; //纯周期性事件结构体



typedef struct
{
    unsigned int id;
    unsigned char port;
    /*下面的选项内容 都要从应用层获得*/
    unsigned char num_Event;        //本次事件触发要发送报文的次数
    unsigned int  cycle_Event;     //本次事件触发要发送报文的周期
    unsigned char  data_Event[8];
    unsigned char  len_Event;
    unsigned int  eventIsGoing;       // 正在进行时

    unsigned char used;
} BC_ENENT_ST; //事件性广播 结构体

typedef struct
{
    unsigned int id;
    unsigned char port;
    unsigned int num_C;
    unsigned int  cycle_C;
    unsigned char data_C_type;
    unsigned char data_C[8];
    unsigned char mask_C[8];

    unsigned char num_Event;        //本次事件触发要发送报文的次数
    unsigned int  cycle_Event;     //本次事件触发要发送报文的周期
    unsigned char data_Event[8];
    unsigned char len_Event;
    unsigned int  eventIsGoing;       // 正在进行时

    unsigned char used;
} BC_CE_ST; //混合 性事件结构体

typedef struct
{
    unsigned int id;
    unsigned char port;
    unsigned char used;

    /*没写完，没有具体应用，不知具休要求，暂不写*/
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
                                     unsigned char len,          //事件性报文内容长度
                                     unsigned char *pdata,       // 事件性报文内容
                                     unsigned char num_Event,    // 事情性报文发送次数
                                     unsigned int cycle_Event);   //

extern unsigned char BC_CE_handle(unsigned int id,
                                  unsigned char port,
                                  unsigned char len,
                                  unsigned char *pdata,
                                  unsigned char num_Event,
                                  unsigned int  cycle_Event);

extern void saveInf_dearCar0x221(unsigned char mask, unsigned char value);

#endif

