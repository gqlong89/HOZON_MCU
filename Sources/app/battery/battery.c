#include <adc.h>
#include <app/battery/battery.h>
#include <app/power/power.h>
#include <config.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <pm.h>
#include <stdbool.h>
#include <string.h>

BATTERYCHARGESTATUS batterychargestatus;
static unsigned int batterytype;
static unsigned int batterymodel;
static unsigned int lastpowerofftime;
signed char batteryhealthstatus = 0;
unsigned int batteryResistance = 0;
int batterytemprature = 0;

static unsigned int BatteryHealthStrategyFlag = 0;

static unsigned int bat_temp_print_flag = 0;

void BatterySetTempPrintFlag(unsigned int flag)
{
    bat_temp_print_flag = flag;
}

void BatteryTempPrint(void)
{
    if (bat_temp_print_flag != 0)
    {
        taskPrintf(TASK_LEVEL_LOG, "BAT temprature = %d, Voltage = %d", batterytemprature,
                   (unsigned int)(BatteryVoltage() * 1000));
    }
}

int BatteryGetTemprature(void)
{
    return batterytemprature;
}

unsigned int BatteryGetResistance(void)
{
    return batteryResistance;
}

unsigned int BatteryGetModel(void)
{
    return batterymodel;
}

signed char BatteryGetStatus(void)
{
    if (batteryhealthstatus > 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned int BatteryInsertDetect(void)
{
    float voltage;

    voltage = 3.3 * adc_ADC_BAT() / 4096;

    if (voltage > 3.2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void BatterychargeInit(void)
{
    memset(&batterychargestatus, 0x00, sizeof(batterychargestatus));

    switch (cfg_get_batterytype())
    {
        case BATTERY_VARTA_450:
            batterytype = BATTERY_TYPE_VARTA;
            batterymodel = BATTERY_MODEL_V450;
            break;

        case BATTERY_VARTA_500:
            batterytype = BATTERY_TYPE_VARTA;
            batterymodel = BATTERY_MODEL_V500;
            break;

        case BATTERY_PANASONIC_500:
            batterytype = BATTERY_TYPE_PANASONIC;
            batterymodel = BATTERY_MODEL_V500;
            break;

        default:
            batterytype = BATTERY_TYPE_PANASONIC;
            batterymodel = BATTERY_MODEL_V500;
            break;
    }

    if (cfg_restore_batchktime(&lastpowerofftime))
    {
        lastpowerofftime = 0;
    }

    BatteryHealthStrategyFlag = 0;
}

void BatteryHealthtestLoadEnable(void)
{
    pm_bat_k_ctrl(true);
}

void BatteryHealthtestLoadDisable(void)
{
    pm_bat_k_ctrl(false);
}

float BatteryVoltage(void)
{
    float voltage;

    //(3.3 * RTData.ADC[2] * (10000 + 10000)/ 4096 /10000)
    voltage = (3.3 * adc_Bbat() * 2 / 4096);

    return voltage;
}

static void BatteryChargeStrategy(void)
{
    if (batterytype == BATTERY_TYPE_VARTA)
    {
        VartaBatteryChargeStrategy();
    }
    else if (batterytype == BATTERY_TYPE_PANASONIC)
    {
        PanasonicBatteryChargeStrategy();
    }
}

static void BatteryDischargeStrategy(void)
{
    if (batterytype == BATTERY_TYPE_VARTA)
    {
        VartaBatteryDischargeStrategy();
    }
    else if (batterytype == BATTERY_TYPE_PANASONIC)
    {
        PanasonicBatteryDischargeStrategy();
    }
}

void BatterychargedischargeStrategy(void)
{
    if (get_powervoltage() < PM_KL30_MIN_POW_VOL)
    {
        BatteryDischargeStrategy();
    }
    else
    {
        BatteryChargeStrategy();
    }
}

void BatteryHealthStrategy(void)
{
    if (((kernelGetCurrentUTCTime() - lastpowerofftime) < 12600) || (BatteryHealthStrategyFlag == 1)
        || (lastpowerofftime == 0))// 3.5h
    {
        BatteryHealthStrategyFlag = 1;
        return;
    }

    if (batterytype == BATTERY_TYPE_VARTA)
    {
        VartaBatteryHealthStrategy();
    }
    else if (batterytype == BATTERY_TYPE_PANASONIC)
    {
        PanasonicBatteryHealthStrategy();
    }
}

