/************************************************************************
 file:         timer.c
 description:  the source file of hard timer function implementation
 date:         2016/1/6
 copyright:    Wuhan Intest Electronic and technology Co.,Ltd
 author        liuzhongwen
 ************************************************************************/

#include <can_api.h>
#include <ftm_hal.h>
#include <ipc_spi.h>
#include <osif.h>
#include <softrtc.h>
#include <stdbool.h>
#include <string.h>
#include <timer.h>

static TIMER_CTX timer_ctx[TIMER_NUM];
static ftm_state_t ftmStateStruct[TIMER_NUM];

extern void htimer_irq_handler(void);
extern void ltimer_irq_handler(void);

static TIMER_ATTR timer_attr[TIMER_NUM] =
{
    {
        TIMER_HIGH_PRE,
        HTIMER_FTM,
        HTIMER_UNIT,
        HTIMER_IRQ_ID,
        htimer_irq_handler,
        &ftmStateStruct[TIMER_HIGH_PRE]
    },
    {
        TIMER_LOW_PRE,
        LTIMER_FTM,
        LTIMER_UNIT,
        LTIMER_IRQ_ID,
        ltimer_irq_handler,
        &ftmStateStruct[TIMER_LOW_PRE]
    },
};

static void timer_irq_handler(TIMER_CTX *ctx, TIMER_ATTR *attr)
{
    uint8_t handle;

    for (handle = 0; handle < ctx->create_cnt; handle++)
    {
        /* the timer is stopped, ignore it */
        if (TIMER_INVALID_VAL == ctx->cur_val[handle] || TIMER_STOP == ctx->status[handle])
        {
            continue;
        }
        /* the timer not timeout, update the cur_val */
        else if (ctx->cur_val[handle] > attr->unit)
        {
            ctx->cur_val[handle] -= attr->unit;
        }
        /* the timer timeout */
        else
        {
            if (NULL != ctx->timeout[handle])
            {
                ctx->timeout[handle](handle);
            }

            ctx->cur_val[handle] = ctx->total_val[handle];
        }
    }
}

static void timer_init(TIMER_TYPE type)
{
    uint8_t handle;
    TIMER_CTX *ctx;
    TIMER_ATTR *attr;
    ftm_user_config_t init_cfg =
    {
        .syncMethod =
        {
            .softwareSync = false,
            .hardwareSync0 = false,
            .hardwareSync1 = false,
            .hardwareSync2 = false,
            .maxLoadingPoint = false,
            .minLoadingPoint = false,
            .inverterSync = FTM_SYSTEM_CLOCK,
            .outRegSync = FTM_SYSTEM_CLOCK,
            .maskRegSync = FTM_SYSTEM_CLOCK,
            .initCounterSync = FTM_SYSTEM_CLOCK,
            .autoClearTrigger = false,
            .syncPoint = FTM_UPDATE_NOW,
        },
        .ftmMode = FTM_MODE_UP_TIMER,
        .ftmPrescaler = FTM_CLOCK_DIVID_BY_64,
        .ftmClockSource = FTM_CLOCK_SOURCE_SYSTEMCLK,
        .BDMMode = FTM_BDM_MODE_11,
        .isTofIsrEnabled = true,
        .enableInitializationTrigger = false,
    };

    ftm_timer_param_t timer_cfg =
    {
        .mode = FTM_MODE_UP_TIMER, /* counter mode */
        .initialValue = 0U, /* Initial counter value */
        .finalValue = 12500U, /* Final counter value */
    };

    ctx = timer_ctx + type;
    attr = timer_attr + type;

    memset(ctx, 0, sizeof(TIMER_CTX));

    for (handle = 0; handle < TIMER_MAX_CNT; handle++)
    {
        ctx->total_val[handle] = TIMER_INVALID_VAL;
        ctx->cur_val[handle] = TIMER_INVALID_VAL;
        ctx->status[handle] = TIMER_NOT_USED;
    }

    if (TIMER_LOW_PRE == type)
    {
        //        timer_cfg.finalValue = 12500U;    // 10ms
        timer_cfg.finalValue = 6250U;     //5ms
    }
    else
    {
        timer_cfg.finalValue = 125U;      // 100us
    }

    FTM_DRV_Init(attr->instance, &init_cfg, attr->ftmStateStruct);
    INT_SYS_InstallHandler(attr->irq, attr->handler, (isr_t *) 0);
    /* set timer period */
    FTM_DRV_InitCounter(attr->instance, &timer_cfg);

    return;
}

static int8_t timer_create(TIMER_TYPE type, timeout_handler timeout)
{
    uint8_t handle;
    TIMER_CTX *ctx;

    ctx = timer_ctx + type;

    if ((ctx->create_cnt >= TIMER_MAX_CNT) || (NULL == timeout))
    {
        return -1;
    }

    handle = ctx->create_cnt;
    ctx->timeout[handle] = timeout;
    ctx->create_cnt++;

    return handle;
}

static int8_t timer_get_status(TIMER_TYPE type, int8_t handle)
{
    TIMER_CTX *ctx;
    ctx = timer_ctx + type;

    if ((handle < 0) || (handle >= ctx->create_cnt))
    {
        return -1;
    }

    return ctx->status[handle];
}

static int8_t timer_start(TIMER_TYPE type, int8_t handle, uint32_t val)
{
    TIMER_CTX *ctx;
    TIMER_ATTR *attr;
    uint32_t pre_status;

    ctx = timer_ctx + type;
    attr = timer_attr + type;

    if ((handle < 0) || (handle >= ctx->create_cnt) || (TIMER_INVALID_VAL == val))
    {
        return -1;
    }

    /* this is the first timer to startup, startup hard timer*/
    if (0 == ctx->run_cnt)
    {
        ctx->total_val[handle] = val;
        ctx->cur_val[handle] = ctx->total_val[handle];
        ctx->status[handle] = TIMER_RUNNING;
        ctx->run_cnt++;

        /* start timer */
        FTM_DRV_CounterStart(attr->instance);
    }
    else
    {
        INT_SYS_DisableIRQ(attr->irq);
        pre_status = ctx->total_val[handle];
        ctx->total_val[handle] = val;
        ctx->cur_val[handle] = ctx->total_val[handle];
        ctx->status[handle] = TIMER_RUNNING;

        /* if it is not started, run_cnt add 1 */
        if (TIMER_INVALID_VAL == pre_status)
        {
            ctx->run_cnt++;
        }

        INT_SYS_EnableIRQ(attr->irq);
    }

    return 0;
}

static int8_t timer_stop(TIMER_TYPE type, int8_t handle)
{
    TIMER_CTX *ctx;
    TIMER_ATTR *attr;

    ctx = timer_ctx + type;
    attr = timer_attr + type;

    if ((handle < 0) || (handle >= ctx->create_cnt))
    {
        return -1;
    }

    /* the timer is already stopped */
    if ((TIMER_INVALID_VAL == ctx->total_val[handle]) || (0 == ctx->run_cnt))
    {
        INT_SYS_DisableIRQ(attr->irq);

        ctx->total_val[handle] = TIMER_INVALID_VAL;
        ctx->cur_val[handle] = TIMER_INVALID_VAL;
        ctx->status[handle] = TIMER_STOP;

        INT_SYS_EnableIRQ(attr->irq);
    }
    else
    {
        INT_SYS_DisableIRQ(attr->irq);

        ctx->total_val[handle] = TIMER_INVALID_VAL;
        ctx->cur_val[handle] = TIMER_INVALID_VAL;
        ctx->status[handle] = TIMER_STOP;
        ctx->run_cnt--;

        INT_SYS_EnableIRQ(attr->irq);

        if (0 == ctx->run_cnt)
        {
            FTM_DRV_CounterStop(attr->instance);
        }
    }

    return 0;
}

void htimer_irq_handler(void)
{
    /* Clear FTM Timer Overflow flag */
    FTM_HAL_ClearTimerOverflow(g_ftmBase[timer_attr[TIMER_HIGH_PRE].instance]);
    timer_irq_handler(timer_ctx + TIMER_HIGH_PRE, timer_attr + TIMER_HIGH_PRE);
}

void htimer_init(void)
{
    timer_init(TIMER_HIGH_PRE);
}

/* !!!note: you must make sure that there is no block function is called(include printf)
 in the timeout function */
int8_t htimer_create(timeout_handler timeout)
{
    return timer_create(TIMER_HIGH_PRE, timeout);
}

int8_t htimer_get_status(int8_t handle)
{
    return timer_get_status(TIMER_HIGH_PRE, handle);
}

/* the unit is us */
int8_t htimer_start(int8_t handle, uint32_t val)
{
    return timer_start(TIMER_HIGH_PRE, handle, val);
}

int8_t htimer_stop(int8_t handle)
{
    return timer_stop(TIMER_HIGH_PRE, handle);
}

void ltimer_irq_handler(void)
{
    /* Clear FTM Timer Overflow flag */
    FTM_HAL_ClearTimerOverflow(g_ftmBase[timer_attr[TIMER_LOW_PRE].instance]);
    timer_irq_handler(timer_ctx + TIMER_LOW_PRE, timer_attr + TIMER_LOW_PRE);
}

void ltimer_init(void)
{
    timer_init(TIMER_LOW_PRE);
}

int8_t ltimer_create(timeout_handler timeout)
{
    return timer_create(TIMER_LOW_PRE, timeout);
}

int8_t ltimer_get_status(int8_t handle)
{
    return timer_get_status(TIMER_LOW_PRE, handle);
}

/* the unit of val is ms */
int8_t ltimer_start(int8_t handle, uint32_t val)
{
    /* change it into us */
    val = val * 1000U;
    return timer_start(TIMER_LOW_PRE, handle, val);
}

int8_t ltimer_stop(int8_t handle)
{
    return timer_stop(TIMER_LOW_PRE, handle);
}


static ftm_state_t ctmState;

void ctimer_irt_handler(void)
{
    CAN_MSG_BUF *can_buf = NULL;
    uint32_t tick;

    /* Clear FTM Timer Overflow flag */
    FTM_HAL_ClearTimerOverflow(g_ftmBase[CTIMER_FTM]);

    while (!SysTick_get_tag(&tick))
    {
        if ((can_buf = (CAN_MSG_BUF *)ipc_can_handle(SPI_GET_FRAME)) != NULL)
        {
            can_buf->miscUptime = SysTick_GetCurCounter() >> 4;
            can_buf->uptime     = tick;
            can_buf->type       = 'T';
            can_buf->dlc        = 64;
            can_buf->MsgID      = 0x0;
            can_buf->DataA      = softrtc_Basetime();
            can_buf->DataB      = softrtc_BaseUptime();
            ipc_can_handle(SPI_SET_FRAME);
        }
        else
        {
            irq_cnt[18]++;
        }
    }
}

void ctimer_init(void)
{
    ftm_user_config_t init_cfg =
    {
        .syncMethod =
        {
            .softwareSync = false,
            .hardwareSync0 = false,
            .hardwareSync1 = false,
            .hardwareSync2 = false,
            .maxLoadingPoint = false,
            .minLoadingPoint = false,
            .inverterSync = FTM_SYSTEM_CLOCK,
            .outRegSync = FTM_SYSTEM_CLOCK,
            .maskRegSync = FTM_SYSTEM_CLOCK,
            .initCounterSync = FTM_SYSTEM_CLOCK,
            .autoClearTrigger = false,
            .syncPoint = FTM_UPDATE_NOW,
        },
        .ftmMode = FTM_MODE_UP_TIMER,
        .ftmPrescaler = FTM_CLOCK_DIVID_BY_64,
        .ftmClockSource = FTM_CLOCK_SOURCE_SYSTEMCLK,
        .BDMMode = FTM_BDM_MODE_11,
        .isTofIsrEnabled = true,
        .enableInitializationTrigger = false,
    };

    ftm_timer_param_t timer_cfg =
    {
        .mode = FTM_MODE_UP_TIMER, /* counter mode */
        .initialValue = 0U, /* Initial counter value */
        .finalValue = 12500U, /* Final counter value */  // 10ms
    };

    FTM_DRV_Init(CTIMER_FTM, &init_cfg, &ctmState);
    INT_SYS_InstallHandler(CTIMER_IRQ_ID, ctimer_irt_handler, (isr_t *) 0);
    FTM_DRV_InitCounter(CTIMER_FTM, &timer_cfg);
}

void ctimer_start(void)
{
    FTM_DRV_CounterStart(CTIMER_FTM);
}

void ctimer_stop(void)
{
    FTM_DRV_CounterStop(CTIMER_FTM);
}

void timer_sleep(void)
{
    INT_SYS_DisableIRQ(HTIMER_IRQ_ID);
    INT_SYS_DisableIRQ(LTIMER_IRQ_ID);
    INT_SYS_DisableIRQ(CTIMER_IRQ_ID);
}

void timer_wakeup(void)
{
    INT_SYS_EnableIRQ(CTIMER_IRQ_ID);
    INT_SYS_EnableIRQ(LTIMER_IRQ_ID);
    INT_SYS_EnableIRQ(HTIMER_IRQ_ID);
}

