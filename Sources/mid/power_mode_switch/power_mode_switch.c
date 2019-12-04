#include <power_manager.h>
#include <power_mode_switch.h>
#include <smc_hal.h>
#include <stdbool.h>


power_manager_user_config_t pwrMgrCfg0 =
{
    .powerMode = POWER_MANAGER_HSRUN,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

power_manager_user_config_t pwrMgrCfg1 =
{
    .powerMode = POWER_MANAGER_RUN,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

power_manager_user_config_t pwrMgrCfg2 =
{
    .powerMode = POWER_MANAGER_VLPR,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

power_manager_user_config_t pwrMgrCfg3 =
{
    .powerMode = POWER_MANAGER_STOP,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

power_manager_user_config_t pwrMgrCfg4 =
{
    .powerMode = POWER_MANAGER_STOP1,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

power_manager_user_config_t pwrMgrCfg5 =
{
    .powerMode = POWER_MANAGER_STOP2,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

power_manager_user_config_t pwrMgrCfg6 =
{
    .powerMode = POWER_MANAGER_VLPS,
    .sleepOnExitOption = false,
    .sleepOnExitValue = false,
};

/*! @brief Array of pointers to User configuration structures */
power_manager_user_config_t *pwrCfgArr[] =
{
    &pwrMgrCfg0,
    &pwrMgrCfg1,
    &pwrMgrCfg2,
    &pwrMgrCfg3,
    &pwrMgrCfg4,
    &pwrMgrCfg5,
    &pwrMgrCfg6
};
/*! @brief Array of pointers to User defined Callbacks configuration structures */
power_manager_callback_user_config_t *pwrStaticCallbacksCfgsArr[] = {(void *)0};




void power_mode_switch_init(void)
{
    /* Initialize Power Manager */
    POWER_SYS_Init(&pwrCfgArr, POWER_MODES, &pwrStaticCallbacksCfgsArr, 0);
}

void power_mode_switch_HSRUN(void)
{
    POWER_SYS_SetMode(HSRUN, POWER_MANAGER_POLICY_AGREEMENT);
}

void power_mode_switch_RUN(void)
{
    POWER_SYS_SetMode(RUN, POWER_MANAGER_POLICY_AGREEMENT);
}

void power_mode_switch_VLPR(void)
{
    POWER_SYS_SetMode(VLPR, POWER_MANAGER_POLICY_AGREEMENT);
}

void power_mode_switch_STOP(void)
{
    POWER_SYS_SetMode(STOP, POWER_MANAGER_POLICY_AGREEMENT);
}

void power_mode_switch_STOP1(void)
{
    POWER_SYS_SetMode(STOP1, POWER_MANAGER_POLICY_AGREEMENT);
}

void power_mode_switch_STOP2(void)
{
    POWER_SYS_SetMode(STOP2, POWER_MANAGER_POLICY_AGREEMENT);
}

void power_mode_switch_VLPS(void)
{
    POWER_SYS_SetMode(VLPS, POWER_MANAGER_POLICY_AGREEMENT);
}

