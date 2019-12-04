/*
 * canbus_off.h
 *
 *  Created on: 2018Äê3ÔÂ8ÈÕ
 *      Author: zhouhong
 */

#ifndef CANBUS_OFF_H_
#define CANBUS_OFF_H_

#include <sys/_stdint.h>

#define L1TOL2              5      // times
#define L1RECOVER           100     // 100ms
#define L2RECOVER           1000    // 1s

typedef struct
{
    uint8_t         cL1ToL2;
    uint32_t        tBusOffRecoveryL1;
    uint32_t        tBusOffRecoveryL2;
} CAN_RECOVER_STRATEGY;

extern void can_recover_init(void);
extern void can_busOffMgr(unsigned char canport, unsigned char state);

extern unsigned char check_busOffState(unsigned char canport);



#endif /* APP_CAN_MGR_CANBUS_OFF_H_ */

