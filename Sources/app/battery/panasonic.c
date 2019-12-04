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

static unsigned int panasonichealthtestflag = 1;

const int Panasonic_NTC_table[17][2] =
{
    -50,    329500,
    -40,    188500,
    -30,    111300,
    -20,    67770,
    -10,    42470,
    0,      27280,
    10,     17960,
    20,     12090,
    30,     8313,
    40,     5827,
    50,     4160,
    60,     3020,
    70,     2228,
    80,     1668,
    90,     1266,
    100,    973,
    110,    757,
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


static void PanasonicBatteryStopCharge(void)
{
    pm_cpu_charge_trickle_ctrl(false);  //Cindy
    pm_cpu_charge_ctrl(false);
    taskPrintf(TASK_LEVEL_LOG, "%s:stop charge.......", __func__);
}

static void PanasonicBatteryFastCharge(void)
{
    if (BATTERY_SUPPORT_HARD_VER == BATTERY_HARD_2)
    {
        pm_cpu_charge_trickle_ctrl(true);   //Cindy
        printf("pm_cpu_charge_trickle_ctrl(true)");
    }

    pm_cpu_charge_ctrl(true);
    taskPrintf(TASK_LEVEL_LOG, "%s:standard charge.......", __func__);
}

static void PanasonicBatteryTrickleCharge(void)             //Cindy
{
    if (BATTERY_SUPPORT_HARD_VER == BATTERY_HARD_2)
    {
        pm_cpu_charge_trickle_ctrl(true);
        pm_cpu_charge_ctrl(false);
    }
    else
    {
        pm_cpu_charge_ctrl(true);
    }

    taskPrintf(TASK_LEVEL_LOG, "%s:Trickle charge.......", __func__);
}

static int Panasonicresistancetotemprature(float resistance)
{
    int temp;
    int j;

    for (j = 1; j < 17; j++)
    {
        if (resistance > Panasonic_NTC_table[j][1])
        {
            break;
        }
    }

    if (j < 17)
    {
        temp  = insert_value(Panasonic_NTC_table[j - 1][1], Panasonic_NTC_table[j - 1][0],
                             Panasonic_NTC_table[j][1], Panasonic_NTC_table[j][0], resistance);

        if (temp > 110)
        {
            temp = 110;
        }
        else if (temp < -50)
        {
            temp = -50;
        }
    }
    else
    {
        temp = 0;
    }

    return temp;

}

static int PanasonicBatteryTempratue(void)
{
    //float voltage, resistance;
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

    temprature = Panasonicresistancetotemprature(resistance);

    batterytemprature = temprature;

    taskPrintf(TASK_LEVEL_DEBUG, ":temprature = %d", temprature);

    return temprature;
}

void PanasonicBatteryHealthStrategy(void)
{
    static float ocv, ccv, voltage;
    float Ri;
    static unsigned char Ritestflag = 0, flag = 0;
    static uint64_t time;
    char adcValueStr[20] = { 0 };
    int temprature;

    temprature = PanasonicBatteryTempratue();

    voltage = (3.3 * adc_Bbat() * 2 / 4096);

    float_to_string(voltage, adcValueStr);
    //taskPrintf(TASK_LEVEL_DEBUG,":voltage = %f",voltage);
    taskPrintf(TASK_LEVEL_DEBUG, ":voltage = %s", adcValueStr);

    if ((voltage < 3.6) && (flag == 0))     //低于3.6不进行健康检查
    {
        panasonichealthtestflag = 1;
        return;
    }

    if (temprature < 0 || temprature > 60)  //低于0，高于60，均不进行电池检测
    {
        panasonichealthtestflag = 1;
        return;
    }

    if (PanasonicBatteryTempratue())

        if (Ritestflag == 0)
        {
            if (flag == 0)
            {
                flag = 1;
                ocv = 3.3 * adc_Bbat() * 2 / 4096;
                time = driverGetUptime();
                BatteryHealthtestLoadEnable();
                panasonichealthtestflag = 0;
                taskPrintf(TASK_LEVEL_LOG, ":time1 = %d", (uint32_t)driverGetUptime());
            }
            else if (driverGetUptime() > (time + 500))      //放电500ms Panasonic
            {
                flag = 0;
                ccv = 3.3 * adc_Bbat() * 2 / 4096;
                Ri = 10 * (ocv - ccv) / ocv;        //根据策略更改公式
                batteryResistance = (unsigned int)(Ri * 1000);
                BatteryHealthtestLoadDisable();
                float_to_string(ocv, adcValueStr);
                taskPrintf(TASK_LEVEL_LOG, ":ocv = %s", adcValueStr);
                float_to_string(ccv, adcValueStr);
                taskPrintf(TASK_LEVEL_LOG, ":ccv = %s", adcValueStr);
                float_to_string(Ri, adcValueStr);
                taskPrintf(TASK_LEVEL_LOG, ":Ri = %s", adcValueStr);
                //taskPrintf(TASK_LEVEL_LOG,":ocv = %f,ccv = %f,Ri = %fΩ",ocv,ccv,Ri);
                taskPrintf(TASK_LEVEL_LOG, ":time2 = %d", (uint32_t)driverGetUptime());

                if (Ri <= 1.35)
                {
                    batteryhealthstatus = 0;
                    taskPrintf(TASK_LEVEL_LOG, "Ri <= 1.35");
                }
                else
                {
                    batteryhealthstatus = 2;
                    taskPrintf(TASK_LEVEL_LOG, "Ri > 1.35");
                }

                Ritestflag = 1;
                panasonichealthtestflag = 1;
            }
        }
}

void PanasonicBatteryChargeStrategy(void)
{
    float battervoltage;
    static unsigned int result = 0;
    static unsigned long long standardchargetime = 0;
    static unsigned long long tricklechargetime = 0;
    static unsigned int standardchargetimeoutflag = 0;
    static unsigned int tricklechargetimeoutflag = 0;
    static uint8_t temp_first_index = 0;
    static unsigned int flag = 1, flag1 = 0;
    int temprature;

    temprature = PanasonicBatteryTempratue();

    BatteryTempPrint();

    pm_cpu_charge_trickle_varta_ctrl(false);        /*Panasonic charge_trickle_varta keep low*/

    if (panasonichealthtestflag == 0)
    {
        return;
    }

    if (get_powervoltage() > PM_KL30_MIN_POW_VOL)     //system Voltage
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

    if (((temprature < (-20)) || (temprature > 60)) && (temp_first_index == 0))     /*P20170516*/
    {
        if (batterychargestatus.stopchargeflag == 0)
        {
            PanasonicBatteryStopCharge();
            batterychargestatus.stopchargeflag = 1;
            batterychargestatus.standardchargeflag = 0;
            batterychargestatus.tricklechargeflag = 0;
        }

        return;
    }
    else
    {
        temp_first_index = 1;
    }


    if (flag == 1)
    {
        flag = 0;
        battervoltage = BatteryVoltage();
        result = (unsigned int)(battervoltage * 100);
    }


    if ((temprature < (-20)) || (temprature > 70))  /*P20170516*/
    {
        if (batterychargestatus.stopchargeflag == 0)
        {
            PanasonicBatteryStopCharge();
            batterychargestatus.stopchargeflag = 1;
            batterychargestatus.standardchargeflag = 0;
            batterychargestatus.tricklechargeflag = 0;
        }
    }
    else if ((result > 402) && (temprature >= (-20)) && (temprature <= 70))
    {
        if ((batterychargestatus.tricklechargeflag == 0) && (tricklechargetimeoutflag == 0))
        {
            PanasonicBatteryTrickleCharge();
            batterychargestatus.stopchargeflag = 0;
            batterychargestatus.standardchargeflag = 0;
            batterychargestatus.tricklechargeflag = 1;
            tricklechargetime = driverGetUptime() + 1440000;    // 100 * 3600 * 4
        }
        else if (driverGetUptime() > tricklechargetime)
        {
            if (batterychargestatus.stopchargeflag == 0)
            {
                PanasonicBatteryStopCharge();
                batterychargestatus.stopchargeflag = 1;
                batterychargestatus.standardchargeflag = 0;
                batterychargestatus.tricklechargeflag = 0;
                tricklechargetimeoutflag = 1;
            }
        }
    }
    else if ((result > 390) && (temprature >= (-20)) && (temprature <= 70))
    {
        if ((batterychargestatus.standardchargeflag == 0) && (standardchargetimeoutflag == 0))
        {
            PanasonicBatteryFastCharge();
            batterychargestatus.stopchargeflag = 0;
            batterychargestatus.standardchargeflag = 1;
            batterychargestatus.tricklechargeflag = 0;
            standardchargetime = driverGetUptime() + 2160000;    // 100 * 3600 * 6
        }
        else if (driverGetUptime() > standardchargetime)
        {
            if (batterychargestatus.stopchargeflag == 0)
            {
                PanasonicBatteryStopCharge();
                batterychargestatus.stopchargeflag = 1;
                batterychargestatus.standardchargeflag = 0;
                batterychargestatus.tricklechargeflag = 0;
                standardchargetimeoutflag = 1;
            }
        }
    }
    else if ((temprature >= (-20)) && (temprature <= 70))
    {
        if ((batterychargestatus.standardchargeflag == 0) && (standardchargetimeoutflag == 0))
        {
            PanasonicBatteryFastCharge();
            batterychargestatus.stopchargeflag = 0;
            batterychargestatus.standardchargeflag = 1;
            batterychargestatus.tricklechargeflag = 0;
            standardchargetime = driverGetUptime() + 4320000;    // 100 * 3600 * 12
        }
        else if (driverGetUptime() > standardchargetime)
        {
            if (batterychargestatus.stopchargeflag == 0)
            {
                PanasonicBatteryStopCharge();
                batterychargestatus.stopchargeflag = 1;
                batterychargestatus.standardchargeflag = 0;
                batterychargestatus.tricklechargeflag = 0;
                standardchargetimeoutflag = 1;
            }
        }
    }
}

void PanasonicBatteryDischargeStrategy(void)
{
    float battervoltage;
    unsigned int result;
    static unsigned int counter = 0;

    battervoltage = BatteryVoltage();
    result = (unsigned int)(battervoltage * 100);

    memset(&batterychargestatus, 0x00, sizeof(batterychargestatus));

    if (result < 300)
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

