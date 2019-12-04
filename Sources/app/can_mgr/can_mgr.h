#ifndef __CAN_MANAGE_H__
#define __CAN_MANAGE_H__

#include <can_api.h>
#include <sys/_stdint.h>

/* CAN report Type */
typedef enum CAN_REPORT_MODEL
{
    CAN_REPORT_NONE = 0,
    CAN_REPORT_GEELY,
} CAN_REPORT_MODEL;

//#define CAN_REPORT_MD       CAN_REPORT_NONE
#define CAN_REPORT_MD       CAN_REPORT_GEELY

/**************can mailbox number*****************/
#define CAN_MAX_PORT        5

/**************can message number*****************/
#define CAN_MES_NUM         5

/**************time invalid value (default)*****************/
#define TIMER_INVALID       (-1)

/**************flag , try to send data using all available mailbox*****************/
#define MAIL_TRY            0xff

/**************time out ,when send message*****************/
#define CAN_TIMEOUT         10

/**************cycle forever*****************/
#define CYCLE_FOREVER       0xfefe

/* can default baud structure */
typedef struct
{
    bool            canfd_en;                  /* canFD enable*/
    bool            canfd_brs;                 /* canFD bit rate switch */
    signed short    canfd_baud;                /* canFD baud*/
    signed short    can_normal_baud;           /* can baud*/
}tCanBaud;

typedef struct
{
    tCanBaud        baud[CAN_MAX_PORT];
    unsigned char   cs;
} CanDftBaudStr;


/*10ms*/
#define MINIMUM_PERIOD      (5U)

#define CAN_TIME_ERR        (200)

/**************can bus off recover*****************/
#define CAN_RECOVER_PERIOD  15                       /*150ms*/

/**************can work state*****************/
#define STATE_Z                 0
#define STATE_OK            (1<<0)
#define STATE_BUSOFF        (1<<1)
#define STATE_ERR           (1<<2)
#define STATE_LOST          (1<<3)
#define STATE_RECEIVE       (1<<6)
#define STATE_SEND          (1<<7)

/**************time -- can no data*****************/
#define CAN_NODATA_TIME     400
#define RET_INVALID         2
#define RET_HAVE_DATA       1
#define RET_NO_DATA         0

/**************time -- can lost*****************/
#define CAN_LOST_TIME       400

typedef struct
{
    uint8_t         u8SwitchValue;
    uint32_t        u32timevalue;
    uint32_t        u32Starttimevalue;
} CAN_TIMER_STR;

typedef struct
{
    uint8_t         workstate;
    uint64_t        rectime;
} CAN_PARAM_STR;


extern CanDftBaudStr can_baud;             
extern CanDftBaudStr can_default_baud;  


void can_err_set(unsigned char port, unsigned char state);

uint8_t can_send(uint8_t canport, uint8_t can_mail, CAN_SEND_MSG *canMsg);
uint8_t can_send_busoff(uint8_t canport, uint8_t can_mail, CAN_SEND_MSG *canMsg);

int can_receive(uint32_t canport, CAN_MSG *tCANMsg);
void can_dump(uint8_t *data, uint16_t *len);
void can_rectime_set(uint8_t port);
uint8_t can_sta_get(uint8_t port);
uint8_t can_state_lost(uint8_t port);
uint8_t can_state_busoff(uint8_t port);
uint8_t can_state_buserr(uint8_t port);

void can_set_baud(uint8_t port, int16_t baud);

void can_start_cantag(void);
void can_wakeup_init(void);
#endif

