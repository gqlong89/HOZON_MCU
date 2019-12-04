/**
 * @Title: gpio.c
 * @author yuzhimin
 * @date Aug 8, 2017
 * @version V1.0
 */

#include <gpio.h>
#include <gpio_hal.h>
#include <interrupt_manager.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_stdint.h>

#define PORT_ISR_MAX_CNT      6         /* max isr can be registered by app for each PORT */

typedef struct
{
    PORT_Type *port_base;
    GPIO_Type *gpio_base;
    IRQn_Type irq; /* port interrupt num */
    isr_t isr; /* port interrupt handler */
} PORT_ATTR;

typedef struct
{
    unsigned char cnt;
    bool used[PORT_ISR_MAX_CNT]; /* the handler status: not used, used */
    unsigned int pins[PORT_ISR_MAX_CNT]; /* interrupt handler corresponding pin */
    port_handler handler[PORT_ISR_MAX_CNT]; /* the pin interrupt handler */
} PORT_CTX;

static void porta_isr(void);
static void portb_isr(void);
static void portc_isr(void);
static void portd_isr(void);
static void porte_isr(void);

static PORT_CTX port_ctx[PORT_NUM_MAX];
static PORT_ATTR port_attr[PORT_NUM_MAX] =
{
    {PORTA, PTA, PORTA_IRQn, porta_isr },
    {PORTB, PTB, PORTB_IRQn, portb_isr },
    {PORTC, PTC, PORTC_IRQn, portc_isr },
    {PORTD, PTD, PORTD_IRQn, portd_isr },
    {PORTE, PTE, PORTE_IRQn, porte_isr },
};

static void gpio_irq_handler(PORT_CTX *ctx, uint32_t int_flag)
{
    uint8_t handle;

    for (handle = 0; handle < PORT_ISR_MAX_CNT; handle++)
    {
        if (false == ctx->used[handle])
        {
            continue;
        }

        if ((int_flag & 1 << ctx->pins[handle]) && (NULL != ctx->handler[handle]))
        {
            ctx->handler[handle](ctx->pins[handle]);
        }
    }
}

void gpio_io_init(GPIO_IO_CFG *cfg)
{
    if (NULL == cfg)
    {
        return;
    }

    if (cfg->io_cfg == GPIO_IN_CONF)
    {
        GPIO_HAL_SetPinDirection(cfg->base, cfg->pin, 0U);
    }
    else
    {
        GPIO_HAL_SetPinDirection(cfg->base, cfg->pin, 1U);

        if (cfg->io_cfg == GPIO_OUT_LOW_CONF)
        {
            GPIO_HAL_WritePin(cfg->base, cfg->pin, 0U);
        }
        else
        {
            GPIO_HAL_WritePin(cfg->base, cfg->pin, 1U);
        }
    }
}

void gpio_isr_init(GPIO_INT_CFG *cfg)
{
    uint8_t handle;
    PORT_CTX *ctx;
    PORT_ATTR *attr;
    static bool first_init = true;

    if (first_init)
    {
        memset(&port_ctx[0], 0, sizeof(port_ctx));
        first_init = false;
    }

    if (NULL == cfg || NULL == cfg->handler)
    {
        return;
    }

    ctx = port_ctx + cfg->port_num;
    attr = port_attr + cfg->port_num;

    /* find unused handle */
    for (handle = 0; handle < PORT_ISR_MAX_CNT; handle++)
    {
        if (true == ctx->used[handle])
        {
            continue;
        }

        break;
    }

    if (handle == PORT_ISR_MAX_CNT)
    {
        printk(PRINTK_LEVEL_LOG, "port[%d] isr list is full", cfg->port_num);
        return;
    }

    GPIO_HAL_SetPinDirection(attr->gpio_base, cfg->port_pin, 0U);      // INPUT DIRECTION
    PORT_HAL_SetPinIntSel(attr->port_base, cfg->port_pin, cfg->port_cfg);
    PORT_HAL_ClearPinIntFlagCmd(attr->port_base, cfg->port_pin);

    INT_SYS_DisableIRQ(attr->irq);

    /* this is the first pin of the port to startup, install the isr of the port */
    if (0 == ctx->cnt)
    {
        INT_SYS_InstallHandler(attr->irq, attr->isr, NULL);
    }

    ctx->handler[handle] = cfg->handler;
    ctx->pins[handle] = cfg->port_pin;
    ctx->used[handle] = true;
    ctx->cnt++;
    INT_SYS_EnableIRQ(attr->irq);
}

void gpio_isr_clear(void)
{
    uint32_t i, j;
    PORT_CTX *ctx;
    PORT_ATTR *attr;

    for (i = 0; i < PORT_NUM_MAX; i++)
    {
        ctx = port_ctx + i;
        attr = port_attr + i;
        INT_SYS_DisableIRQ(attr->irq);

        for (j = 0; j < ctx->cnt; j++)
        {
            PORT_HAL_SetPinIntSel(attr->port_base, ctx->pins[j], PORT_DMA_INT_DISABLED);
        }
    }

    memset(&port_ctx[0], 0, sizeof(port_ctx));
}

void gpio_isr_deinit(PORT_NUM_TYPE port_num, unsigned int pin)
{
    uint8_t handle;
    PORT_CTX *ctx;
    PORT_ATTR *attr;
    ctx = port_ctx + port_num;
    attr = port_attr + port_num;

    /* find handle by pin */
    for (handle = 0; handle < PORT_ISR_MAX_CNT; handle++)
    {
        if (true == ctx->used[handle] && ctx->pins[handle] == pin)
        {
            break;
        }
    }

    if (handle == PORT_ISR_MAX_CNT)
    {
        return;
    }

    PORT_HAL_SetPinIntSel(attr->port_base, pin, PORT_DMA_INT_DISABLED);
    GPIO_HAL_SetPortInputDisable(attr->gpio_base, pin);

    INT_SYS_DisableIRQ(attr->irq);
    PORT_HAL_ClearPinIntFlagCmd(attr->port_base, pin);

    ctx->handler[handle] = NULL;
    ctx->pins[handle] = 0xFF;
    ctx->used[handle] = false;
    ctx->cnt--;
    INT_SYS_EnableIRQ(attr->irq);

    if (0 == ctx->cnt)
    {
        INT_SYS_InstallHandler(attr->irq, NULL, NULL);
        INT_SYS_DisableIRQ(attr->irq);
    }
}

static void porta_isr(void)
{
    uint32_t int_flag = PORT_HAL_GetPortIntFlag(PORTA);
    PORT_HAL_ClearPortIntFlagCmd(PORTA);
    gpio_irq_handler(port_ctx + PORT_NUM_A, int_flag);
}

static void portb_isr(void)
{
    uint32_t int_flag = PORT_HAL_GetPortIntFlag(PORTB);
    PORT_HAL_ClearPortIntFlagCmd(PORTB);
    gpio_irq_handler(port_ctx + PORT_NUM_B, int_flag);

}

static void portc_isr(void)
{
    uint32_t int_flag = PORT_HAL_GetPortIntFlag(PORTC);
    PORT_HAL_ClearPortIntFlagCmd(PORTC);
    gpio_irq_handler(port_ctx + PORT_NUM_C, int_flag);
}

static void portd_isr(void)
{
    uint32_t int_flag = PORT_HAL_GetPortIntFlag(PORTD);
    PORT_HAL_ClearPortIntFlagCmd(PORTD);
    gpio_irq_handler(port_ctx + PORT_NUM_D, int_flag);
}

static void porte_isr(void)
{
    uint32_t int_flag = PORT_HAL_GetPortIntFlag(PORTE);
    PORT_HAL_ClearPortIntFlagCmd(PORTE);
    gpio_irq_handler(port_ctx + PORT_NUM_E, int_flag);
}

unsigned int GPIO_ReadPinInput(GPIO_Type *base, unsigned int pin)
{
    return (((base->PDIR) >> pin) & 0x1U);
}

