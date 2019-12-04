#ifndef _ACC_H_
#define _ACC_H_

#define PI 3.1415926

/* ACC Type */
typedef enum
{
    ACC_UNKNOWN = 0,
    ACC_ICM20600,
    ACC_FXLS8471Q,
} ACC_TYPE;

extern char accstatus;

int acc_device_init(void);
void acc_check(void);
void acc_standby(void);
void acc_sleep(void);
#endif

