/**
* @Title: fxls_acc.c
* @author yuzhimin
* @date Mar 27, 2018
* @version V1.0
*/

#include <acc.h>
#include <config.h>
#include <i2c.h>
#include <math.h>
#include <mid/acc/fxls_acc.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <sys/_stdint.h>


static inline int fxls_write(unsigned char addr, unsigned char val, unsigned int num)
{
    return driverI2CWrite(FXLS8471Q_SLAVE_ADDR, addr, &val, num);
}

static inline int fxls_read(unsigned char addr, unsigned char *valPtr, unsigned int num)
{
    return driverI2CRead(FXLS8471Q_SLAVE_ADDR, addr, valPtr, num);
}

bool is_fxls_acc(void)
{
    int ret;
    unsigned char temp_val = 0;
    ret = fxls_read(FXLS_REG_WHOAMI, &temp_val, 1);

    if (!ret)
    {
        if (temp_val != FXLS_VAL_WHOAMI)
        {
            printk(PRINTK_LEVEL_ERROR, "is not fxls acc:(0x%x)", temp_val);
            ret = 1;
        }
    }
    else
    {
        printk(PRINTK_LEVEL_ERROR, "fxls read faild,ret:%d", ret);
    }

    return ret ? false : true;
}

bool fxls_acc_init(void)
{
    int ret;
    uint8_t databyte = 0;
    /*  please refer to the "example FXOS8700CQ Driver Code" in FXOS8700 datasheet. */
    /*  write 0000 0000 = 0x00 to accelerometer control register 1 */
    /*  standby */
    /*  [7-1] = 0000 000 */
    /*  [0]: active=0 */
    databyte = 0;
    ret = fxls_write(FXLS_REG_CTRL_REG1, databyte, 1);

    /*  write 0000 0001= 0x01 to XYZ_DATA_CFG register */
    /*  [7]: reserved */
    /*  [6]: reserved */
    /*  [5]: reserved */
    /*  [4]: hpf_out=0 */
    /*  [3]: reserved */
    /*  [2]: reserved */
    /*  [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB */
    /*  databyte = 0x01; */
    databyte = 0x01;
    ret |= fxls_write(FXLS_REG_XYZ_DATA_CFG, databyte, 1);

    /*  write 0000 1101 = 0x0D to accelerometer control register 1 */
    /*  [7-6]: aslp_rate=00 */
    /*  [5-3]: dr=001 for 200Hz data rate (when in hybrid mode) */
    /*  [2]: lnoise=1 for low noise mode */
    /*  [1]: f_read=0 for normal 16 bit reads */
    /*  [0]: active=1 to take the part out of standby and enable sampling */
    /*   databyte = 0x0D; */
    databyte = 0x0d;
    ret |= fxls_write(FXLS_REG_CTRL_REG1, databyte, 1);

    /*Accelerometer vector-magnitude interrupt*/
    if (cfg_get_anti_thief())
    {
        ret |= fxls_write(FXLS_REG_CTRL_REG1, 0x00, 1);
        ret |= fxls_write(FXLS_REG_CTRL_REG3, 0x04, 1);
        ret |= fxls_write(FXLS_REG_CTRL_REG4, 0x02, 1);
        ret |= fxls_write(FXLS_REG_CTRL_REG5, 0xff, 1);
        ret |= fxls_write(FXLS_REG_A_VECM_CFG, 0x48, 1);
        databyte = ((cfg_get_acc_vecm_ths() >> 8) & 0x1f);
        databyte |= 0x80;
        ret |= fxls_write(FXLS_REG_A_VECM_THS_MSB, databyte, 1);
        databyte = (cfg_get_acc_vecm_ths() & 0xff);
        ret |= fxls_write(FXLS_REG_A_VECM_THS_LSB, databyte, 1);
        ret |= fxls_write(FXLS_REG_A_VECM_CNT, 0x01, 1);
        ret |= fxls_write(0x2a, 0x29, 1);
    }
    else
    {
        ret |= fxls_write(FXLS_REG_CTRL_REG1, 0x00, 1);
        ret |= fxls_write(FXLS_REG_CTRL_REG3, 0x00, 1);
        ret |= fxls_write(FXLS_REG_CTRL_REG4, 0x00, 1);
        ret |= fxls_write(FXLS_REG_CTRL_REG1, 0x29, 1);
    }

    if (ret)
    {
        printk(PRINTK_LEVEL_ERROR, "fxls write faild,ret:%d", ret);
        return false;
    }
    else
    {
        printk(PRINTK_LEVEL_LOG, "fxls acc init ok");
        return true;
    }
}

int fxls_get_interrupt_src(void)
{
    int ret, flag = 0;
    unsigned char int_src, transient_src;

    ret = fxls_read(FXLS_REG_INT_SOURCE, &int_src, 1);

    if (ret)
    {
        printk(PRINTK_LEVEL_ERROR, "reg read error:%d", ret);
        return -1;
    }

    if (int_src > 0)
    {
        printk(PRINTK_LEVEL_ERROR, "int_src: 0x%x", int_src);

        if (int_src & 0x02)
        {
            flag |= FXLS_INT_MASK_VECM;
            printk(PRINTK_LEVEL_ERROR, "Accelerometer vector-magnitude interrupt");
        }

        if (int_src & 0x20)
        {
            flag |= FXLS_INT_MASK_TRANS;
            printk(PRINTK_LEVEL_ERROR, "Transient interrupt");
            ret = fxls_read(FXLS_REG_TRANSIENT_SRC, &transient_src, 1);

            if (ret)
            {
                printk(PRINTK_LEVEL_ERROR, "reg read error:%d, int_src:0x%x", ret, int_src);
            }
            else
            {
                printk(PRINTK_LEVEL_ERROR, "TRANSIENT_SRC: 0x%x", transient_src);
            }
        }
    }

    return flag;
}

static void fxls_xyz_convent(signed short *accvel, double *angle)
{
    double angle_x, angle_y, angle_z;

    angle_x = asin(accvel[0] * 0.488 / 1000) * 180 / PI;

    if (angle_x < 0)
    {
        angle_x = 90 + angle_x;
    }
    else
    {
        angle_x = 90 - angle_x;
    }

    angle_y = asin(accvel[1] * 0.488 / 1000) * 180 / PI;

    if (angle_y < 0)
    {
        angle_y = 90 + angle_y;
    }
    else
    {
        angle_y = 90 - angle_y;
    }

    angle_z = asin(accvel[2] * 0.488 / 1000) * 180 / PI;
    angle_z = 90 - angle_z;

    if (angle)
    {
        angle[0] = angle_x;
        angle[1] = angle_y;
        angle[2] = angle_z;
    }

    if (angle_z > 55)
    {
        printk(PRINTK_LEVEL_ERROR, " Roll over");
        printk(PRINTK_LEVEL_ERROR, " angle_z: (%d)", angle_z);
    }
}

int fxls_get_xyz(double *angle)
{
    int ret;
    unsigned char XYZ[7];
    short accVal[3] = {0, 0, 0};

    ret = fxls_read(FXLS_REG_STATUS, XYZ, 7);

    if ((!ret) && (XYZ[0] & (1 << 3)))
    {
        accVal[0] = (((signed short)((XYZ[1] << 8) + XYZ[2])) >> 2);
        accVal[1] = (((signed short)((XYZ[3] << 8) + XYZ[4])) >> 2);
        accVal[2] = (((signed short)((XYZ[5] << 8) + XYZ[6])) >> 2);
        fxls_xyz_convent(accVal, angle);
        printk(PRINTK_LEVEL_DEBUG, "status[0x%x],acc_xyz=[%5d,%5d,%5d]",
               XYZ[0], accVal[0], accVal[1], accVal[2]);
        return 0;
    }
    else
    {
        printk(PRINTK_LEVEL_ERROR, "Fatal error!,ret:%d,status[0x%x]", ret, XYZ[0]);
        return -1;
    }
}

void fxls_standby(void)
{
    fxls_write(FXLS_REG_CTRL_REG1, 0x00, 1);
}

void fxls_sleep(void)
{
    uint8_t databyte = 0;

    fxls_read(FXLS_REG_CTRL_REG1, &databyte, 1);
    databyte &= 0xFE; //Clear Active Bit
    fxls_write(FXLS_REG_CTRL_REG1, databyte, 1);

    fxls_read(FXLS_REG_CTRL_REG2, &databyte, 1);
    databyte |= 0x04; //Set the Sleep Enable bit
    fxls_write(FXLS_REG_CTRL_REG2, databyte, 1);

    fxls_read(FXLS_REG_CTRL_REG1, &databyte, 1);
    databyte &= 0x5E; //clear the bits that should be cleared for the sample rates
    databyte |= 0x58; //Set ASLP = 6.25 Hz, DR = 100 Hz
    fxls_write(FXLS_REG_CTRL_REG1, databyte, 1);

    fxls_read(FXLS_REG_CTRL_REG2, &databyte, 1);
    databyte &= 0xE4; //puts both Oversampling modes in Normal Mode
    databyte |= 0x1A; //Wake High Res, Sleep Low Power
    fxls_write(FXLS_REG_CTRL_REG2, databyte, 1);

    fxls_write(FXLS_REG_CTRL_REG4, 0x82, 1);

    fxls_write(FXLS_REG_CTRL_REG5, 0xff, 1);

    fxls_write(FXLS_REG_CTRL_REG3, 0x04, 1);

    fxls_read(FXLS_REG_XYZ_DATA_CFG, &databyte, 1);
    databyte &= 0xFC; //Clear the FS bits to 00 2g
    fxls_write(FXLS_REG_XYZ_DATA_CFG, databyte, 1);
}

