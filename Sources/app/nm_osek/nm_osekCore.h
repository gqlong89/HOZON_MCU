


#ifndef NM_OSEKCORE_H
#define NM_OSEKCORE_H

#include <app/can_mgr/can_basic_cfg.h>

#if USED_NM_OSEK
/*NM��ַ��Ϣ*/
#define NM_BASE_ID          0x400
#define NM_MIN_ID           0
#define NM_MAX_ID           0xff
#define NM_LOCAL_ID         0x02


#define RX_ERROR_LIMIT      4   // ���մ�����ֵ
#define TX_ERROR_LIMTT      8   // ���ʹ�����ֵ


/*����ʱ����ֵ����λ��MS*/
#define TTyp_Value                  100
#define TMax_Value                  260
#define TError_Value                1000
#define TWBS_Value                  5000
#define TTx_Value                   20
#define TSleepReqMin_LimpHome_Value 5000
#define TSleepReqMin_Normal_Value   5000

#define TAppActive_Value            90  // Ӧ�ñ��� ��֡��ʱ��

/*PDU�������ݹ����룬��λ������*/
#define NM_MaskAlive                0x01
#define NM_MaskRing                 0x02
#define NM_MaskLimphome             0x04
#define NM_MaskSleepInd             0x10   /* sleep ind */
#define NM_MaskSleepAck             0x20   /* sleep ack */


#define Rx_PDU_MAX                  (5)// PDU�����ջ���


#define  GOTOMODE_AWAKE_MASK        (1<<0)
#define  GOTOMODE_BUSSLEEP_MASK     (1<<1)



typedef enum
{
    NM_stOff = 0,
    NM_stInit,
    NM_stInitReset,
    NM_stNormal,
    NM_stNormalPrepSleep,      /* sleep indication */
    NM_stTwbsNormal,           /* wait bus sleep   */
    NM_stBusSleep,             /* bus sleep        */
    NM_stLimphome,             /* limp hone        */
    NM_stLimphomePrepSleep,    /* limphome sleep indication */
    NM_stTwbsLimphome,         /* limphone wait bus sleep */
    NM_stOn                    /* in fact if not Off then ON. */
} NMState_E;                 // ����״̬


typedef enum
{
    NM_BusSleep = 1,
    NM_Awake
} NMMode_E;

typedef enum
{
    NM_ckNormal = 1,              /* supported by direct and indirect NM */
    NM_ckNormalExtended,          /* only supported by indirect NM */
    NM_ckLimphome                 /* only supported by direct NM */
} ConfigKindName_E;



typedef struct
{
    unsigned char source;
    unsigned char destination;
    union
    {
        unsigned char byte;
        struct
        {
            unsigned char alive     : 1;
            unsigned char ring      : 1;
            unsigned char limphome  : 1;
            unsigned char reserved1 : 1;
            unsigned char sleepInd  : 1;
            unsigned char sleepAck  : 1;
            unsigned char reserved2 : 2;
        } bit;
    } opCode;
    unsigned char ringData[6] ;
} NMPDU_ST; //  PDU�ṹ��

typedef struct
{
    unsigned char p_in;//дָ��
    unsigned char p_out;//��ָ��
    NMPDU_ST BUF[Rx_PDU_MAX];//�Ѿ����յ���PDU
} CircularBuffer_ST;   //����һ�����λ������ṹ��


typedef union
{
    unsigned int word;
    struct
    {
        unsigned int NMactive: 1;                /* ����NMͨѶ */
        unsigned int NMAPPactive: 1;             /*����Ӧ��ͨѶ*/
        unsigned int canModeNormal: 1;           /*1��CAN�ڴ���������״̬��0��CAN�ڴ��ڼ���״̬*/
        unsigned int bussleep: 1;                /*��ʾ�������������ǳ���*/
        unsigned int configurationstable: 1;
        unsigned int presleepenable: 1;          /* Ԥ����ʹ�� */
        unsigned int sleepAck: 1;                // ����SLEEPACK��������
    } bit;
} NetworkStatus_U;         //��ǰ����״̬


typedef union
{
    unsigned int word;
    struct
    {
        unsigned int stable : 1;// ���ȶ�������������ALIVE������LIMPHOME״̬
        unsigned int limphome : 1;  //�����Ѿ����͹�limphome״̬
    } bit;
} NMMerker_U; //  �����ȶ�




typedef struct
{
    unsigned char nm_osekEN;

    unsigned char nmLocalID;  // ���ؽڵ��ַ

    unsigned int nmBaseADDR;   // nm ID����ַ

    unsigned char nmMinID;     //nm ID��Χ��С��ַ
    unsigned char nmMaxID;     //nm ID��Χ����ַ

    unsigned int  remoteAwakeMinID;//Զ�̻�����СID
    unsigned int  remoteAwakeMaxID;//Զ�̻������ID

    unsigned char normalList[32];      // ����normal״̬�Ľڵ�
    unsigned char
    limphomeList[32];    // ����limehome״̬�µĽڵ㣬��λ����ʾ�����֧��32*8= 256���ڵ㡣

    NMState_E currentState;    // ���ڵ㵱ǰ״̬
    NMState_E lastState;       // ���ڵ���һ��״̬

    unsigned char logical_successor;  // �߼��̳���

    unsigned int TTx;
    unsigned int TTyp;
    unsigned int TMax;
    unsigned int TError;
    unsigned int TWbs;
    unsigned int TSleepReqMin_LimpHome;
    unsigned int TSleepReqMin_Normal;       //��Щ���Ƕ�ʱ����ֵ

    unsigned int TAppActive;                //Ӧ�ñ���֡ʱ��

    unsigned char appActiveFlag;            //Ӧ�ñ�����֡��־

    unsigned int nmTimerOverFlow;     //  NM ��ʱ���������־
    unsigned char
    goToModeUsedFlag;   // gotoMODE ���ñ�־����һλ�����ù�AWAKE���ڶ�λ�����ù�BUSSLEEP��
    unsigned char NM_TX_OKFLAG;       //1:���� �ɹ���0û�з��ͳɹ���
    unsigned char awake_ByApp_Flag;        //1:��Ӧ�ñ��Ļ��ѣ�0��û�б�Ӧ�ñ��Ļ���
    unsigned char busSleep_ErrFlag;         //������״̬�£��յ����ߴ����п������б��ġ�
    unsigned char receive_NM_Flag;      // �յ���������ı�־��1Ϊ�յ���0Ϊû���յ�

    unsigned char nmRxCount;  //RX���մ������
    unsigned char nmTxCount;  //���ʹ������

    NetworkStatus_U NetworkStatus;// ��ǰ���繤��״̬��
    NMMerker_U   nmMerker;        //�߼����ȶ�״̬


    NMPDU_ST RxPDU;//���յ���PDU

    NMPDU_ST TxPDU;//Ҫ���͵�PDU

} NMControlBlock_ST;

void nmAddtoPresent(unsigned char canport, unsigned char nodeId);
void nmAddtoLimphome(unsigned char canport, unsigned char nodeId);



/*���⺯���ӿ�*/
extern void nm_OsekMainFunction(void);
extern void NM_TxConformation(unsigned char canport, FXCAN_BUFF_TYPE mail_n);
extern void NM_RxIndication(unsigned char canport, CAN_MSG *msg);
extern void GotoMode(unsigned char canport, NMMode_E newMode);
extern void Start_NM_Osek(unsigned char canport);
extern void D_Stop(unsigned char canport);
extern void D_Start(unsigned char canport);


extern void wakeUpNM_ByAPP(unsigned char canport);
extern void gotoLimpHome_ByBusOff(unsigned char canport);
extern void CanBus_ErrorCheck(unsigned char canport);



#endif

#endif



