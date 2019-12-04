#include <adc.h>
#include <app/battery/battery.h>
#include <app/power/power.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <pm.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_stdint.h>
#include <gpio.h>
#include <config.h>

static unsigned int vartahealthtestflag = 1;

#define VARTA_HW_SOPPORT_TRICKLE 1    /*PTD5 Config*/


const int Varta_NTC_table[33][2] =
{
    -40,    204650,
    -38,    182330,
    -36,    162920,
    -34,    145960,
    -32,    131060,
    -30,    117870,
    -28,    106150,
    -26,    95699,
    -24,    86356,
    -22,    77998,
    -20,    70519,
    -18,    63827,
    -16,    57840,
    -14,    52482,
    -12,    47686,
    -10,    43389,
    -8,     39536,
    -6,     36074,
    -4,     32960,
    -2,     30153,
    0,      27619,
    10,     18091,
    20,     12121,
    30,     8294,
    40,     5800,
    50,     4142,
    60,     3012,
    70,     2226,
    80,     1668,
    90,     1266,
    100,    974,
    110,    760,
    120,    600,
};


static int insert_value(int x0, int y0, int x1, int y1, int x)
{
    if (x0 == x1)
    {
        return ((y1 - y0) / 2);
    }
    else
    {
        return ((y1 - y0) * (x - x0) / (x1 - x0) + y0);
    }
}


static void VartaBatteryStopCharge(void)
{
    #if VARTA_HW_SOPPORT_TRICKLE
    {
        pm_cpu_charge_ctrl(false);
        pm_cpu_charge_trickle_ctrl(false);
        pm_cpu_charge_trickle_varta_ctrl(false);
    }
    #else
    {
        pm_cpu_charge_ctrl(false);
    }
    #endif

    taskPrintf(TASK_LEVEL_LOG, "%s:stop charge.......", __func__);
}

static void VartaBatteryFastCharge(void)
{
    #if VARTA_HW_SOPPORT_TRICKLE
    {
        pm_cpu_charge_ctrl(true);
        pm_cpu_charge_trickle_ctrl(true);
        pm_cpu_charge_trickle_varta_ctrl(false);
    }
    #else
    {
        pm_cpu_charge_ctrl(true);
    }
    #endif
    taskPrintf(TASK_LEVEL_LOG, "%s:standard charge.......", __func__);
}

static void VartaBatteryTrickleCharge(void)
{
    #if VARTA_HW_SOPPORT_TRICKLE
    {
        pm_cpu_charge_ctrl(false);
        pm_cpu_charge_trickle_ctrl(false);
        pm_cpu_charge_trickle_varta_ctrl(true);
    }
    #else
    {
        static unsigned long long fastchargetime = 0;
        static unsigned long long stopchargetime = 0;

        if (batterychargestatus.tricklechargeflag == 0)
        {
            fastchargetime = 0;
            stopchargetime = 0;
        }

        if (driverGetUptime() > fastchargetime)
        {
            VartaBatteryFastCharge();
            stopchargetime = driverGetUptime() + 6000;    // 100 * 60    1min
            fastchargetime = driverGetUptime() + 66000;    // 11 min
            taskPrintf(TASK_LEVEL_LOG, "%s:fast charge 1min.......%d", __func__, driverGetUptime());
        }
        else if (driverGetUptime() > stopchargetime)
        {
            VartaBatteryStopCharge();
            fastchargetime = driverGetUptime() + 60000;    // 10 min
            stopchargetime = driverGetUptime() + 66000;    // 11 min
            taskPrintf(TASK_LEVEL_LOG, "%s:stop charge 10min........%d", __func__, driverGetUptime());
        }
    }
    #endif
}

static int Vartaresistancetotemprature(float resistance)
{
    int temp;
    int j;

    for (j = 1; j < 33; j++)
    {
        if (resistance > Varta_NTC_table[j][1])
        {
            break;
        }
    }

    if (j < 33)
    {
        temp  = insert_value(Varta_NTC_table[j - 1][1], Varta_NTC_table[j - 1][0],
                             Varta_NTC_table[j][1], Varta_NTC_table[j][0], resistance);

        if (temp > 120)
        {
            temp = 120;
        }
        else if (temp < -40)
        {
            temp = -40;
        }
    }
    else
    {
        temp = 0;
    }

    return temp;

}

static int VartaBatteryTempratue(void)
{
    int res_temp, resistance;
    int temprature;

    //voltage = 3.3 * adc_ADC_BAT() / 4096;
    //resistance = 10000 * voltage / (3.3 - voltage);

    res_temp = adc_ADC_BAT();

    if (res_temp != 4096)
    {
        resistance = res_temp * 10000 / (4096 - res_temp);
    }
    else
    {
        resistance = 0;
    }

    temprature = Vartaresistancetotemprature(resistance);

    batterytemprature = temprature;

    taskPrintf(TASK_LEVEL_DEBUG, ":temprature = %d,resistance=%d", temprature, (int)resistance);

    return temprature;
}

void VartaBatteryHealthStrategy(void)
{
    static float ocv, ccv, voltage;
    float Ri;
    static unsigned char Ritestflag = 0, flag = 0;
    static uint64_t time;
    char adcValueStr[20] = {0 };

    voltage = (3.3 * adc_Bbat() * 2 / 4096);

    float_to_string(voltage, adcValueStr);
    //taskPrintf(TASK_LEVEL_DEBUG,":voltage = %f",voltage);
    taskPrintf(TASK_LEVEL_DEBUG, ":voltage = %s", adcValueStr);

    if ((voltage < 3.6) && (flag == 0))
    {
        vartahealthtestflag = 1;
        return;
    }

    if (Ritestflag == 0)
    {
        if (flag == 0)
        {
            flag = 1;
            ocv = 3.3 * adc_Bbat() * 2 / 4096;
            time = driverGetUptime();
            BatteryHealthtestLoadEnable();
            vartahealthtestflag = 0;
            taskPrintf(TASK_LEVEL_LOG, ":start time = %d", (uint32_t)driverGetUptime());
        }
        else if (driverGetUptime() > (time + 100))
        {
            flag = 0;
            ccv = 3.3 * adc_Bbat() * 2 / 4096;
            Ri = 10 * (ocv - ccv) / ccv;
            batteryResistance = (unsigned int)(Ri * 1000);
            BatteryHealthtestLoadDisable();
            float_to_string(ocv, adcValueStr);
            taskPrintf(TASK_LEVEL_LOG, ":ocv = %s", adcValueStr);
            float_to_string(ccv, adcValueStr);
            taskPrintf(TASK_LEVEL_LOG, ":ccv = %s", adcValueStr);
            float_to_string(Ri, adcValueStr);
            taskPrintf(TASK_LEVEL_LOG, ":Ri = %s", adcValueStr);
            //taskPrintf(TASK_LEVEL_LOG,":ocv = %f,ccv = %f,Ri = %f¦¸",ocv,ccv,Ri);
            taskPrintf(TASK_LEVEL_LOG, ":end time = %d", (uint32_t)driverGetUptime());

            if (Ri < 1.2)
            {
                batteryhealthstatus = 0;
                taskPrintf(TASK_LEVEL_LOG, ":battery SOH = green");
            }
            else if ((1.2 <= Ri) && (Ri <= 1.5))
            {
                batteryhealthstatus = 1;
                taskPrintf(TASK_LEVEL_LOG, ":battery SOH = yellow");
            }
            else if ((Ri >= 1.5))
            {
                batteryhealthstatus = 2;
                taskPrintf(TASK_LEVEL_LOG, ":battery SOH = red");
            }

            Ritestflag = 1;
            vartahealthtestflag = 1;
        }
    }
}

void VartaBatteryChargeStrategy(void)
{
    float battervoltage;
    static unsigned int result = 0;
    static unsigned long long standardchargetime = 0;
    static unsigned int standardchargetimeoutflag = 0;
    static unsigned int flag = 1, flag1 = 0;
    int temprature;
    int TempratueMax = TEMP_MAX_V450;

    temprature = VartaBatteryTempratue();

    BatteryTempPrint();

    if (cfg_get_batterytype() == 2)     /*2: VARTA 500   1:VARTA 450*/
    {
        TempratueMax = TEMP_MAX_V500;
    }
    else
    {
        TempratueMax = TEMP_MAX_V450;
    }

    if (vartahealthtestflag == 0)
    {
        return;
    }

    if (get_powervoltage() > 5)     //system Voltage
    {
        if (flag1 == 1)
        {
            flag = 1;
            flag1 = 0;
        }
    }
    else
    {
        flag1 = 1;
    }

    if (flag == 1)
    {
        flag = 0;
        battervoltage = BatteryVoltage();
        result = (unsigned int)(battervoltage * 100);
    }

    if (BATTERY_MODEL_V500 == BatteryGetModel())
    {
        TempratueMax = TEMP_MAX_V500;
    }
    else
    {
        TempratueMax = TEMP_MAX_V450;
    }

    if ((result > 408) || (temprature < (-20)) || (temprature > TempratueMax))
    {
        if (batterychargestatus.stopchargeflag == 0)
        {
            VartaBatteryStopCharge();
            batterychargestatus.stopchargeflag = 1;
            batterychargestatus.standardchargeflag = 0;
            batterychargestatus.tricklechargeflag = 0;
            taskPrintf(TASK_LEVEL_LOG, ":temprature = %d\r\n", temprature);
        }
    }
    else if ((result >= 390) && (temprature >= (-20)) && (temprature <= TempratueMax))
    {
        VartaBatteryTrickleCharge();

        if (batterychargestatus.tricklechargeflag == 0)
        {
            #if VARTA_HW_SOPPORT_TRICKLE
            {
                taskPrintf(TASK_LEVEL_LOG, "%s:trickle charge.......", __func__);
            }
            #endif
            batterychargestatus.stopchargeflag = 0;
            batterychargestatus.standardchargeflag = 0;
            batterychargestatus.tricklechargeflag = 1;
        }
    }
    else if ((result > 0) && (temprature >= (-20)) && (temprature <= TempratueMax))
    {
        if ((batterychargestatus.standardchargeflag == 0) && (standardchargetimeoutflag == 0))
        {
            VartaBatteryFastCharge();
            batterychargestatus.stopchargeflag = 0;
            batterychargestatus.standardchargeflag = 1;
            batterychargestatus.tricklechargeflag = 0;
            standardchargetime = driverGetUptime() + 5040000;    // 100 * 3600 * 14
        }
        else if (driverGetUptime() > standardchargetime)
        {
            VartaBatteryTrickleCharge();
            standardchargetimeoutflag = 1;

            if (batterychargestatus.tricklechargeflag == 0)
            {
                #if VARTA_HW_SOPPORT_TRICKLE
                {
                    taskPrintf(TASK_LEVEL_LOG, "%s:trickle charge.......", __func__);
                }
                #endif
                batterychargestatus.stopchargeflag = 0;
                batterychargestatus.standardchargeflag = 0;
                batterychargestatus.tricklechargeflag = 1;
            }
        }
    }
}

void VartaBatteryDischargeStrategy(void)
{
    float battervoltage;
    unsigned int result;
    static unsigned int counter = 0;

    battervoltage = BatteryVoltage();
    result = (unsigned int)(battervoltage * 100);

    memset(&batterychargestatus, 0x00, sizeof(batterychargestatus));

    if (result < 270)
    {
        //stop discharge
        counter++;

        if (counter >= 10)
        {
            counter = 0;
            //runShellCMD("shutdown");
        }
    }
    else
    {
        counter = 0;
    }
}

void VartaChargePinStatus(void)
{
    printf("pm_cpu_charge_ctrl = %d, pm_cpu_charge_trickle_ctrl = %d, pm_cpu_charge_trickle_varta_ctrl = %d\r\n",
           GPIO_ReadPinInput(GP_CPU_CHARGE_ON, GP_CPU_CHARGE_ON_PIN_OUT),
           GPIO_ReadPinInput(GP_CPU_CHARGE_TRICKLE, GP_CPU_CHARGE_TRICKLE_PIN_OUT),
           GPIO_ReadPinInput(GP_CPU_CHARGE_TRICKLE_VARTA, GP_CPU_CHARGE_TRICKLE__VARTA_PIN_OUT));
}

