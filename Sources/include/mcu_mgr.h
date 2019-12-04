/**
 * @Title: mcu_mgr.h
 * @author yuzhimin
 * @date Jul 8, 2017
 * @version V1.0
 */

#ifndef INCLUDE_MCU_MGR_H_
#define INCLUDE_MCU_MGR_H_

#include <stdbool.h>

unsigned int mcumgr_task(unsigned int mask);
void mpu_reset_flag(bool enable);

#endif /* INCLUDE_MCU_MGR_H_ */
