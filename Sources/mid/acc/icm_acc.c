/**
* @Title: icm_acc.c
* @author yuzhimin
* @date Mar 27, 2018
* @version V1.0
*/

#include <acc.h>
#include <config.h>
#include <i2c.h>
#include <math.h>
#include <mid/acc/icm_acc.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>


static inline int icm_write(unsigned char addr, unsigned char val, unsigned int num)
{
    return driverI2CWrite(ICM20600_SLAVE_ADDR, addr, &val, num);
}

static inline int icm_read(unsigned char addr, unsigned char *valPtr, unsigned int num)
{
    return driverI2CRead(ICM20600_SLAVE_ADDR, addr, valPtr, num);
}


bool is_icm_acc(void)
{
    int ret;
    unsigned char  temp_val = 0;

    ret = icm_read(ICM_REG_WHO_AM_I, &temp_val, 1);

    if (!ret)
    {
        if (temp_val != ICM_VAL_WHO_AM_I)
        {
            printk(PRINTK_LEVEL_ERROR, "is not icm acc:(0x%x)", temp_val);
            ret = 1;
        }
    }
    else
    {
        printk(PRINTK_LEVEL_ERROR, "icm read faild,ret:%d", ret);
    }

    return ret ? false : true;
}

bool icm_acc_init(void)
{
    int ret;
    unsigned char  temp_val = 0;
    ret = icm_write(ICM_REG_PWR_MGMT_1, 0x00, 1);
    //ret |= icm_write(ICM_REG_GYRO_CONFIG, 0x10, 1);
    /* ¡À250 dps */
    ret |= icm_write(ICM_REG_GYRO_CONFIG, 0x00, 1);
    /* ¡À8g */
    ret |= icm_write(ICM_REG_ACCEL_CONFIG, 0x10, 1);

    ret |= icm_write(ICM_REG_SMPLRT_DIV, 0x09, 1);
    ret |= icm_write(ICM_REG_PWR_MGMT_2, 0x00, 1);
    ret |= icm_write(ICM_REG_PWR_MGMT_1, 0x01, 1);

    icm_read(ICM_REG_GYRO_CONFIG, &temp_val, 1);

    printk(PRINTK_LEVEL_ERROR, "ICM_REG_GYRO_CONFIGret:%x", temp_val);

    /*config interrupt*/
    if (cfg_get_anti_thief())
    {
        ret |= icm_write(ICM_REG_INT_ENABLE, 0xE0, 1);
        ret |= icm_write(ICM_REG_ACCEL_WOM_X_THR, 0x80, 1);
        ret |= icm_write(ICM_REG_ACCEL_WOM_Y_THR, 0x80, 1);
        ret |= icm_write(ICM_REG_ACCEL_WOM_Z_THR, 0x80, 1);
        ret |= icm_write(ICM_REG_ACCEL_INTEL_CTRL, 0xC0, 1);
        ret |= icm_write(ICM_REG_INT_PIN_CFG, 0x81, 1);  //A1
    }
    else
    {
        ret |= icm_write(ICM_REG_INT_ENABLE, 0x00, 1);
    }

    if (ret)
    {
        printk(PRINTK_LEVEL_ERROR, "icm write faild,ret:%d", ret);
        return false;
    }
    else
    {
        printk(PRINTK_LEVEL_LOG, "icm acc init ok");
        return true;
    }
}

int icm_get_interrupt_src(void)
{
    int ret, flag = 0;
    unsigned char int_src;

    ret = icm_read(ICM_REG_INT_STATUS, &int_src, 1);

    if (ret)
    {
        printk(PRINTK_LEVEL_ERROR, "reg read error:%d", ret);
        return -1;
    }

    printk(PRINTK_LEVEL_DEBUG, "int_src:0x%x", int_src);

    if (int_src > 1)
    {
        flag |= 1;
    }

    return flag;

}

float acc_Get_Angle(float x, float y, float z, unsigned char dir)
{
    float temp;
    float res = 0;

    switch (dir)
    {
        case 0:
            temp = z / sqrt((x * x + y * y));
            res = atan(temp);
            break;

        case 1:
            temp = x / sqrt((y * y + z * z));
            res = atan(temp);
            break;

        case 2:
            temp = y / sqrt((x * x + z * z));
            res = atan(temp);
            break;
    }

    return res * 180 / PI;
}

static void icm_xyz_convent(signed short *accvel, double *angle)
{
    float angle_x, angle_y, angle_z;
    static float acc_x, acc_y, acc_z, acc_w;
    static unsigned int cnt = 0;
    static unsigned int pcnt = 0;
    static unsigned char crash_state = 0; //bit0 current crash state;   bit1 once crash
    static unsigned char rollover_state =
        0; //bit0 current rollover state;  bit1 once rollover but now normal;  bit2 once rollover
    static float crash_acc;
    static float rollover_angle;
    angle_x = acc_Get_Angle((float)accvel[0], (float)accvel[1], (float)accvel[2], 1);

    if (angle_x < 0)
    {
        angle_x = 90 + angle_x;
    }
    else
    {
        angle_x = 90 - angle_x;
    }

    angle_y = acc_Get_Angle((float)accvel[0], (float)accvel[1], (float)accvel[2], 2);

    if (angle_y < 0)
    {
        angle_y = 90 + angle_y;
    }
    else
    {
        angle_y = 90 - angle_y;
    }

    angle_z = acc_Get_Angle((float)accvel[0], (float)accvel[1], (float)accvel[2], 0);
    angle_z = 90 - angle_z;

    if (angle)
    {
        angle[0] = angle_x;
        angle[1] = angle_y;
        angle[2] = angle_z;
    }

    acc_x = ((float)accvel[0]) / 4096;
    acc_y = ((float)accvel[1]) / 4096;
    acc_z = ((float)accvel[2]) / 4096;
    acc_w = sqrt(acc_x * acc_x + acc_y * acc_y);

    if (angle_z > 55)
    {
        cnt++;

        if (cnt > 100) //1s
        {
            rollover_state |= 0x05;
            rollover_angle = angle_z;
        }
    }
    else if (angle_z < 50)
    {
        cnt = 0;

        if (rollover_state)
        {
            rollover_state |= 0x02;
            rollover_state &= (~0x01);
        }
    }
    else
    {
        if (cnt > 0)
        {
            cnt++;

            if (cnt > 100)
            {
                rollover_state |= 0x05;
                rollover_angle = angle_z;
            }
        }
    }

    if (acc_w > 6)
    {
        crash_state = 0x03;
        crash_acc = acc_w;
    }
    else
    {
        crash_state &= (~0x01);
    }


    if (crash_state & 0x01)
    {
        printk(PRINTK_LEVEL_ERROR, "Crash!!! the acc is:(%d)", (int)crash_acc);
    }

    pcnt++;

    if (pcnt % 1000 == 0)
    {
        printk(PRINTK_LEVEL_DEBUG, "acc_x,acc_y,acc_z,acc_w:%d,%d,%d,  %d", (int)acc_x, (int)acc_y,
               (int)acc_z, (int)acc_w);
        printk(PRINTK_LEVEL_DEBUG, "angle_x,angle_y,angle_z:%d,%d,%d", (int)angle_x, (int)angle_y,
               (int)angle_z);

        if (crash_state & 0x02)
        {
            printk(PRINTK_LEVEL_DEBUG, "Once Crash!!! the acc is:(%d)", (int)crash_acc);
        }

        if (rollover_state & 0x01)
        {
            printk(PRINTK_LEVEL_ERROR, "Roll over!!! the angle is:(%d)", (int)rollover_angle);
        }

        if (rollover_state & 0x02)
        {
            printk(PRINTK_LEVEL_DEBUG, "Once Roll over,now get right !!! ");
        }
    }

    #if 0

    if (angle_z > 55)
    {
        printk(PRINTK_LEVEL_ERROR, " Roll over");
        printk(PRINTK_LEVEL_ERROR, " angle_z:(%d)", (int)angle_z);
    }

    #endif

}

short accVal[3] = {0, 0, 0};
short gyroVal[3] = {0, 0, 0};

void getaccVal( unsigned char *buf )
{
    memcpy( buf,(unsigned char *)accVal,6 );

    printk(PRINTK_LEVEL_DEBUG, "acc_xyz=[%x,%x,%x] [%5d,%5d,%5d]\r\n", accVal[0], accVal[1], accVal[2], accVal[0], accVal[1], accVal[2]);

    return;
}


void getgyroVal( unsigned char *buf )
{
    memcpy( buf,(unsigned char *)gyroVal,6 );
    printk(PRINTK_LEVEL_DEBUG, "gyro_xyz=[%x,%x,%x] [%5d,%5d,%5d]\r\n", gyroVal[0], gyroVal[1], gyroVal[2], gyroVal[0], gyroVal[1], gyroVal[2]);
    return;
}

int icm_get_xyz(double *angle)
{
    unsigned int ret;
    unsigned char XYZ[14];
    //short accVal[3] = {0, 0, 0};
    short tempVal = 0;
    //short gyroVal[3] = {0, 0, 0};
    int temperature = 0;

    ret = icm_read(ICM_REG_ACCEL_XOUT_H, XYZ, 14);

    if (!ret)
    {
        accVal[0] = (((signed short)((XYZ[0] << 8) + XYZ[1])));
        accVal[1] = (((signed short)((XYZ[2] << 8) + XYZ[3])));
        accVal[2] = (((signed short)((XYZ[4] << 8) + XYZ[5])));
        icm_xyz_convent(accVal, angle);
        printk(PRINTK_LEVEL_DEBUG, "acc_xyz=[%5d,%5d,%5d]", accVal[0], accVal[1], accVal[2]);

        tempVal = (((signed short)((XYZ[6] << 8) + XYZ[7])));
        temperature = tempVal * 10 / 3268 + 25;
        printk(PRINTK_LEVEL_DEBUG, "temperature=%5d,tempVal=%5d", temperature, tempVal);

        gyroVal[0] = (((signed short)((XYZ[8] << 8) + XYZ[9])));
        gyroVal[1] = (((signed short)((XYZ[10] << 8) + XYZ[11])));
        gyroVal[2] = (((signed short)((XYZ[12] << 8) + XYZ[13])));
        printk(PRINTK_LEVEL_DEBUG, "gyro_xyz=[%5d,%5d,%5d]", gyroVal[0], gyroVal[1], gyroVal[2]);
        return 0;
    }
    else
    {
        printk(PRINTK_LEVEL_ERROR, "Fatal error!,ret:%d", ret);
        return -1;
    }
}

void icm_standby(void)
{
    // TODO:  Temporary code by yzm
    int ret;
    unsigned char value;
    ret = icm_read(ICM_REG_PWR_MGMT_1, &value, 1);

    if (!ret)
    {
        icm_write(ICM_REG_PWR_MGMT_2, 0x3F, 1);
        icm_write(ICM_REG_PWR_MGMT_1, 0x41, 1);
    }
    else
    {
        value |= 0x40;  // sleep bit
        icm_write(ICM_REG_PWR_MGMT_2, 0x07, 1);
        icm_write(ICM_REG_PWR_MGMT_1, value, 1);
    }
}

void icm_sleep(void)
{
    // TODO:  Temporary code by yzm
    int ret;
    unsigned char value;
    ret = icm_read(ICM_REG_PWR_MGMT_1, &value, 1);

    #if 1
    icm_write(ICM_REG_PWR_MGMT_2, 0x07, 1);  //acc xyz able, gyro xyz disable
    icm_write(ICM_REG_PWR_MGMT_1, 0x09, 1);  // disable temperature
    #endif

    #if 0

    if (!ret)
    {
        icm_write(ICM_REG_PWR_MGMT_2, 0x3F, 1);
        icm_write(ICM_REG_PWR_MGMT_1, 0x41, 1);
    }
    else
    {
        value |= 0x40;  // sleep bit
        icm_write(ICM_REG_PWR_MGMT_2, 0x07, 1);
        icm_write(ICM_REG_PWR_MGMT_1, value, 1);
    }

    #endif
}

