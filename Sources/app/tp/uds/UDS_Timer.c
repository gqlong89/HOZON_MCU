/*****************************************************************************
 *   Include Files
 *****************************************************************************/

#include <app/tp/uds/UDS_Config.h>
#include <sys/_stdint.h>
#include <timer.h>

int8_t uds_server_timer;
int8_t uds_client_timer;

#if ( ( UDS_TYPE_SEL == UDS_TYPE_SERVER ) || ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT ) )

void uds_server_timeout(int8_t handle)
{
    if (uds_server_timer == handle)
    {
        CF_Request(&tUDS_Server);
    }
}

#endif

#if ( ( UDS_TYPE_SEL == UDS_TYPE_CLIENT ) || ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT ) )

void uds_client_timeout(int8_t handle)
{
    if (uds_client_timer == handle)
    {
        CF_Request(&tUDS_Client);
    }
}

#endif

/*****************************************************************************
 *   Function   :    UDS_Timer_Init
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void UDS_Timer_Init(uds_t *tUDS)
{
    #if ( ( UDS_TYPE_SEL == UDS_TYPE_SERVER ) || ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT ) )

    if (tUDS->mode == UDS_TYPE_SERVER)
    {
        uds_server_timer = htimer_create(uds_server_timeout);
    }

    #endif

    #if ( ( UDS_TYPE_SEL == UDS_TYPE_CLIENT ) || ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT ) )

    if (tUDS->mode == UDS_TYPE_CLIENT)
    {
        uds_client_timer = htimer_create(uds_client_timeout);
    }

    #endif
}

/*****************************************************************************
 *   Function   :    UDS_TimerClose
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void UDS_TimerClose(uds_t *tUDS)
{
    if (tUDS->mode == UDS_TYPE_SERVER)
    {
        htimer_stop(uds_server_timer);
    }
    else if (tUDS->mode == UDS_TYPE_CLIENT)
    {
        htimer_stop(uds_client_timer);
    }
}

/*****************************************************************************
 *   Function   :    UDS_Timer_SetPeriod
 *   Description:
 *   Inputs     :    None
 *   Outputs    :    NULL
 *   Notes      :
 *****************************************************************************/
void UDS_Timer_SetPeriod(uds_t *tUDS, uint32_t u32PeriodValue100us)
{
    if (tUDS->mode == UDS_TYPE_SERVER)
    {
        htimer_start(uds_server_timer, u32PeriodValue100us);
    }
    else if (tUDS->mode == UDS_TYPE_CLIENT)
    {
        htimer_start(uds_client_timer, u32PeriodValue100us);
    }
}

