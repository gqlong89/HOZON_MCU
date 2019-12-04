#ifndef __WDG_H__
    #define __WDG_H__

    void driverWDGInit(void);
    void driverWDGFeed(void);
    void driverWDGEnable(void);
    void driverWDGDisable(void);
    void driverReset(void);
    uint32_t getResetStatus(void);

#endif
