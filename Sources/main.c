/* ###################################################################
 **     Filename    : main.c
 **     Processor   : S32K144
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.00
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */
#include "Cpu.h"
#include "pin_mux.h"
#include "clockMan1.h"
#include "osif1.h"
#include "lowFTimer.h"
#include "lpuart1.h"
#include "dmaController1.h"
#include "can0_flexcan.h"
#include "lpspiCom.h"
#include "adc0.h"
#include "watchdog1.h"
#include "rtcTimer1.h"
#include "SRSFTimer.h"
#include "flash1.h"
#include "pwrMan1.h"
#include "lpi2c1.h"

#include <adc.h>
#include <app/power/power.h>
#include <brdInfo.h>
#include <can_api.h>
#include <config.h>
#include <dbg_trace.h>
#include <dbg_uart.h>
#include <edma.h>
#include <flash.h>
#include <i2c.h>
#include <ipc_spi.h>
#include <mcu_mgr.h>
#include <mid/kernel/kernel.h>
#include <mid/kernel/task.h>
#include <osif.h>
#include <pin.h>
#include <scom.h>
#include <shell.h>
#include <srs.h>
#include <stdbool.h>
#include <timer.h>
#include <uds.h>
#include <wdg.h>
#include <adc.h>
#include <app/nm_osek/nm_osekCore.h>
#include <app/can_mgr/can_basic.h>




volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */

extern void clock_init(void);

static void mcu_init(void)
{
    clock_init();
    pin_init();

    irq_priority_init();
    flash_init();
    SysTick_Init();
    edma_init();

    dbg_uart_init();
    kernelSetDebugLevel(PRINTK_LEVEL_LOG);

    dbgTraceDump();
    dbgTraceInit();

    cfg_init();

    brd_info_init();

    htimer_init();
    ltimer_init();
    ctimer_init();

    PowerInit();

    spk_close();        //close spk
    RTCInit();//add by DSC 2018.9.18
    ipc_device_init();

    // can_device_init();
    // nm_init();

    adc_Abat_init();

    getResetStatus();
    srs_init();
    driverI2CInit();
}

static void task_init(void)
{
    /* init task */
    kernelInitTask();
    /* create mcu manager(This task should be placed at first) */
    kernelCreateTask(mcumgr_task, TASKNAME_MCU_MGR);

    printk(PRINTK_LEVEL_LOG, "get tick=%d", OSIF_GetMilliseconds());
    kernelCreateTask(canBasic_task, TASKNAME_CANBASIC);

    kernelCreateTask(scom_task, TASKNAME_SPI_COM);

    kernelCreateTask(uds_task, TASKNAME_UDS);

    kernelCreateTask(shell_task, TASKNAME_SHELL);

}

/*!
 \brief The main function for the project.
 \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
 */
int main(void)
{
    /* Write your local variable definition here */

    /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
    #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT(); /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
    #endif
    /*** End of Processor Expert internal initialization.                    ***/

    /* Write your code here */
    /* For example: for(;;) { } */
    mcu_init();

    task_init();

    driverWDGInit();

    /* run kernel */
    kernelRun();

    /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.1 [05.21]
 **     for the Freescale S32K series of microcontrollers.
 **
 ** ###################################################################
 */
