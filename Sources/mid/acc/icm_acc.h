/**
* @Title: icm_acc.h
* @author yuzhimin
* @date Mar 27, 2018
* @version V1.0
*/
#ifndef __ICM_ACC_H__
#define __ICM_ACC_H__
#include <stdbool.h>

#define ICM20600_SLAVE_ADDR         (0x68)
#define ICM_VAL_WHO_AM_I            (0x11)

#define ICM_REG_GYRO_CONFIG         (0x1B)
#define ICM_REG_ACCEL_CONFIG        (0x1C)
#define ICM_REG_SMPLRT_DIV          (0x19)
#define ICM_REG_ACCEL_WOM_X_THR     (0X20)
#define ICM_REG_ACCEL_WOM_Y_THR     (0X21)
#define ICM_REG_ACCEL_WOM_Z_THR     (0X22)
#define ICM_REG_INT_PIN_CFG         (0X37)
#define ICM_REG_INT_ENABLE          (0X38)
#define ICM_REG_INT_STATUS          (0X3A)
#define ICM_REG_ACCEL_XOUT_H        (0X3B)
#define ICM_REG_ACCEL_INTEL_CTRL    (0X69)
#define ICM_REG_PWR_MGMT_1          (0X6B)
#define ICM_REG_PWR_MGMT_2          (0X6C)
#define ICM_REG_WHO_AM_I            (0x75)

bool is_icm_acc(void);
bool icm_acc_init(void);
void icm_standby(void);
void icm_sleep(void);
int icm_get_xyz(double *angle);


#endif /* __ICM_ACC_H__ */

