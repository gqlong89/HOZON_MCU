#ifndef __CAN_FLEXCAN_H__
#define __CAN_FLEXCAN_H__

#include <stdbool.h>
#include <flexcan_hal.h>



int flexcan_init(uint8_t instance, tCanBaud baud, flexcan_operation_modes_t mode);
int flexcan_deinit(uint8_t instance);
uint8_t flexcan_set_baud(uint8_t instance, uint32_t baud);
int flexcan_tx_mb_no_block(uint8_t instance, uint8_t *bufferPtr, uint8_t mbIdx);
void flexcan_tx_buff_no_block(uint8_t instance, uint8_t *bufferPtr);
uint8_t flexcan_tx_mb_block(uint8_t instance, uint8_t *bufferPtr, uint8_t mbIdx, uint32_t timeout);
bool flexcan_get_tx_status(uint8_t instance, uint8_t mbIdx);
extern unsigned int flexcan_get_errstatus(uint8_t instance);
extern void flexcan_clear_errstatus(uint8_t instance);
extern void flexcan_set_errint(uint8_t instance, bool enable);

#endif

