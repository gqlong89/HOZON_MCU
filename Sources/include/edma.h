/**
 * @Title: edma.h
 * @author yuzhimin
 * @date Jul 13, 2017
 * @version V1.0
 */
#ifndef INCLUDE_EDMA_H_
#define INCLUDE_EDMA_H_

#include "status.h"

enum
{
    EDMA_CHN_LPUART0_TX = 0U,
    EDMA_CHN_LPSPI0_RX,
    EDMA_CHN_LPSPI0_TX,
    EDMA_CHN_COUNT,
};

status_t edma_init(void);
status_t edma_deinit(void);
status_t edma_chn_realese(unsigned char channel);
status_t edma_chn_reinit(unsigned char channel);

#endif /* INCLUDE_EDMA_H_ */
