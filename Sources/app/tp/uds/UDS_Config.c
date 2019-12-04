#include <app/tp/uds/UDS_Config.h>

#if ( UDS_TYPE_SEL == UDS_TYPE_SERVER )
    uds_t tUDS_Server;
    uds_t *pt_ServerUDS = &tUDS_Server;
    unsigned char UDS_DATA_S[UDS_SERVER_MAX_BYTE + UDS_DATA_HEAD_LENGTH];
#elif ( UDS_TYPE_SEL == UDS_TYPE_CLIENT )
    uds_t tUDS_Client;
    uds_t *pt_ClientUDS = &tUDS_Client;
    unsigned char UDS_DATA_C[UDS_CLIENT_MAX_BYTE + UDS_DATA_HEAD_LENGTH];
#elif ( UDS_TYPE_SEL == UDS_TYPE_SERVER_CLIENT )
    uds_t tUDS_Server;
    uds_t tUDS_Client;

    uds_t *pt_ServerUDS = &tUDS_Server;
    uds_t *pt_ClientUDS = &tUDS_Client;
    __attribute__((section(".large_data"))) unsigned char UDS_DATA_S[UDS_SERVER_MAX_BYTE +
    UDS_DATA_HEAD_LENGTH];
    __attribute__((section(".large_data"))) unsigned char UDS_DATA_C[UDS_CLIENT_MAX_BYTE +
    UDS_DATA_HEAD_LENGTH];
#endif

/* get uds protocol strack version */
char *UDSTP_GetVersion(void)
{
    return UDSTP_VER_NO;
}

