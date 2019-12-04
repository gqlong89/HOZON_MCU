#ifndef __BATTERY_H__
#define __BATTERY_H__

#define TEMP_MAX_V500   80
#define TEMP_MAX_V450   65
#define BATTERY_SUPPORT_HARD_VER      BATTERY_HARD_2       //>=IN78200048P0.2

enum BATTERY_TYPE
{
    BATTERY_TYPE_VARTA = 0,
    BATTERY_TYPE_PANASONIC = 1,
};

enum BATTERY_MODEL
{
    BATTERY_MODEL_V450 = 0,
    BATTERY_MODEL_V500 = 1,
};

typedef struct
{
    unsigned char standardchargeflag;
    unsigned char tricklechargeflag;
    unsigned char stopchargeflag;
} BATTERYCHARGESTATUS;

//added by Cindy. BATTERY_HARD_2 ?¡ì3???¨¢¡Â3?¦Ì?
enum BATTERY_SUPPORT
{
    BATTERY_HARD_1 = 0,
    BATTERY_HARD_2 = 1,
};

extern BATTERYCHARGESTATUS batterychargestatus;
extern signed char batteryhealthstatus;
extern unsigned int batteryResistance;
extern int batterytemprature;

void BatterychargeInit(void);
float BatteryVoltage(void);
void BatterychargedischargeStrategy(void);
void BatteryHealthStrategy(void);
void BatteryHealthtestLoadEnable(void);
void BatteryHealthtestLoadDisable(void);
int BatteryGetTemprature(void);
unsigned int BatteryGetResistance(void);
signed char BatteryGetStatus(void);
unsigned int BatteryInsertDetect(void);
unsigned int BatteryGetModel(void);

//varta battery
void VartaBatteryDischargeStrategy(void);
void VartaBatteryChargeStrategy(void);
void VartaBatteryHealthStrategy(void);
void VartaChargePinStatus(void);

//panasonic
void PanasonicBatteryDischargeStrategy(void);
void PanasonicBatteryChargeStrategy(void);
void PanasonicBatteryHealthStrategy(void);

void BatterySetTempPrintFlag(unsigned int flag);
void BatteryTempPrint(void);

#endif

