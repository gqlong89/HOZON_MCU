/************************************************************************
 file:         timer.h
 description:  the header file of hardware timer definition
 date:         2016/1/6
 copyright:    Wuhan Intest Electronic and technology Co.,Ltd
 author        liuzhongwen
 ************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <ftm_driver.h>
#include <interrupt_manager.h>
#include <sys/_stdint.h>
#include <S32K144.h>

#define TIMER_MAX_CNT      100         /* max timer can be created by app for each type timer  modified by cindy 5->8*/

#define TIMER_INVALID_VAL  0xFFFFFFFF

typedef void (*timeout_handler)(int8_t handle);

//#define LTIMER_UNIT         (10*1000U)              /* 10ms */
#define LTIMER_UNIT         (5*1000U)              /* 5ms */
#define LTIMER_IRQ_ID       FTM1_Ovf_Reload_IRQn
#define LTIMER_FTM          1U                      // FTM1

#define HTIMER_UNIT         (100U)                  /* 100us */
#define HTIMER_IRQ_ID       FTM2_Ovf_Reload_IRQn
#define HTIMER_FTM          2U                      // FTM2

#define CTIMER_UNIT         (1000U)                  /* 1ms, for can module only*/
#define CTIMER_IRQ_ID       FTM3_Ovf_Reload_IRQn
#define CTIMER_FTM          3U                      // FTM3

typedef enum
{
    TIMER_HIGH_PRE = 0, /* high-precision timer,  the precision is 100us */
    TIMER_LOW_PRE,      /* low-precision timer,   the precision is 10ms  */
    TIMER_NUM
} TIMER_TYPE;

typedef enum
{
    TIMER_NOT_USED = 0,
    TIMER_RUNNING,
    TIMER_STOP,
    TIMER_ERROR
} TIMER_STATUS;

typedef struct
{
    uint8_t create_cnt; /* the timer num which is created by app */
    uint8_t run_cnt; /* the timer num which is started by app */
    uint8_t status[TIMER_MAX_CNT]; /* the timer status: not used, run, pause, stop */
    uint32_t cur_val[TIMER_MAX_CNT]; /* the current timeout value from it timeout */
    uint32_t total_val[TIMER_MAX_CNT]; /* the total timeout value */
    timeout_handler timeout[TIMER_MAX_CNT]; /* timeout callback function */
} TIMER_CTX;

typedef struct
{
    TIMER_TYPE type; /* time type */
    uint32_t instance; /* hard timer address */
    uint32_t unit; /* hard timer precision */
    IRQn_Type irq; /* hard timer interrupt */
    isr_t handler; /* interrupt handler */
    ftm_state_t *ftmStateStruct;
} TIMER_ATTR;

void htimer_init(void);
int8_t htimer_create(timeout_handler timeout);
int8_t htimer_start(int8_t handle, uint32_t val);
int8_t htimer_stop(int8_t handle);
int8_t htimer_get_status(int8_t handle);

void ltimer_init(void);
int8_t ltimer_create(timeout_handler timeout);
int8_t ltimer_start(int8_t handle, uint32_t val);
int8_t ltimer_stop(int8_t handle);
int8_t ltimer_get_status(int8_t handle);

void ctimer_init(void);
void ctimer_start(void);
void ctimer_stop(void);

void timer_sleep(void);
void timer_wakeup(void);

#endif

