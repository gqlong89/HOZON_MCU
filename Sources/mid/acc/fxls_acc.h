/**
* @Title: fxls_acc.h
* @author yuzhimin
* @date Mar 27, 2018
* @version V1.0
*/
#ifndef __FXLS_ACC_H__
#define __FXLS_ACC_H__
#include <stdbool.h>


#define FXLS8471Q_SLAVE_ADDR            (0x1E)
#define FXLS_VAL_WHOAMI                 (0x6A)

#define FXLS_INT_MASK_INTERRUPT          (1 << 0)
#define FXLS_INT_MASK_VECM               (1 << 1)
#define FXLS_INT_MASK_TRANS              (1 << 2)

#define FXLS_REG_STATUS                 (0x00)
#define FXLS_REG_INT_SOURCE             (0x0C)
#define FXLS_REG_WHOAMI                 (0x0D)
#define FXLS_REG_XYZ_DATA_CFG           (0x0E)
#define FXLS_REG_TRANSIENT_SRC          (0x1E)
#define FXLS_REG_CTRL_REG1              (0x2A)
#define FXLS_REG_CTRL_REG2              (0x2B)
#define FXLS_REG_CTRL_REG3              (0x2C)
#define FXLS_REG_CTRL_REG4              (0x2D)
#define FXLS_REG_CTRL_REG5              (0x2E)
#define FXLS_REG_A_VECM_CFG             (0x5F)
#define FXLS_REG_A_VECM_THS_MSB         (0x60)
#define FXLS_REG_A_VECM_THS_LSB         (0x61)
#define FXLS_REG_A_VECM_CNT             (0x62)




bool is_fxls_acc(void);
bool fxls_acc_init(void);
int fxls_get_interrupt_src(void);
int fxls_get_xyz(double *angle);
void fxls_standby(void);
void fxls_sleep(void);

#endif /* __FXLS_ACC_H__ */

