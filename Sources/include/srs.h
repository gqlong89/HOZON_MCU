#ifndef _SRS_H_
#define _SRS_H_

enum
{
    SRS_SIGNAL_UNKNOWN = 0,
    SRS_SIGNAL_NORMAL,
    SRS_SIGNAL_CRASH
};

/*******************************************************************************
 * Definitions
 ******************************************************************************/
void SRS_CRTL(void);
void srs_init(void);
unsigned int kernelGetSRSStatus(void);

#endif

