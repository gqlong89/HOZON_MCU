#ifndef _POWER_H_
#define _POWER_H_

//#define VOLTAGE_COMPENSATION    1.0011
//#define VOLTAGE_SLOPE           0.9673

#define VOLTAGE_COMPENSATION    0.777
#define VOLTAGE_SLOPE           1


#define PM_KL30_MIN_POW_VOL     (5)


/* powerStatus mask */
#define POWERSTATUS_MASK_ON         (1 << 0)
#define POWERSTATUS_MASK_ACC        (1 << 1)
#define POWERSTATUS_MASK_USB        (1 << 2)
#define POWERSTATUS_MASK_NCHARGE    (1 << 3)
#define POWERSTATUS_MASK_FCHARGE    (1 << 4)
#define POWERSTATUS_MASK_RING       (1 << 5)
#define POWERSTATUS_MASK_RTC        (1 << 6)
#define POWERSTATUS_MASK_ECUUP      (1 << 7)
#define POWERSTATUS_MASK_BT         (1 << 8)
#define POWERSTATUS_MASK_G_SENSOR   (1 << 9)
#define POWERSTATUS_MASK_CAN1       (1 << 10)
#define POWERSTATUS_MASK_CAN2       (1 << 11)
#define POWERSTATUS_MASK_CAN3       (1 << 12)
#define POWERSTATUS_MASK_PM         (1 << 13)
#define POWERSTATUS_MASK_RF         (1 << 14)
#define POWERSTATUS_MASK_BTN        (1 << 15)

#define POWER_LOWER_THRED           8
#define POWER_HIGH_THRED            18


void PowerInit(void);
void PowerListen(void);
void PowerSleep(void);
unsigned int driverGetHardLineWakeSignals(void);
unsigned int driverGetRTCWakeSignal(void);
unsigned int driverGetRingWakeSignal(void);
unsigned int driverGetWakeUpSrc(void);
void driverDcOut1(unsigned int on);
void driverDcOut2(unsigned int on);
unsigned int driverGetDcOut1Status(void);
unsigned int driverGetDcOut2Status(void);
float get_powervoltage(void);
void driverSetWakeUpSrc(unsigned int wakesrc);
void driverInitWakeUpSrc(void);

#endif

