/**
 * @Title: dbg_uart.h
 * @author yuzhimin
 * @date Jul 12, 2017
 * @version V1.0
 */
#ifndef INCLUDE_DBG_UART_H_
#define INCLUDE_DBG_UART_H_

#include <sys/_stdint.h>

void dbg_uart_init(void);
void dbg_uart_open(uint8_t task);
uint32_t dbg_uart_send(const uint8_t *buffer, uint32_t size);
uint32_t dbg_uart_recv(uint8_t *buffer, uint32_t size);
void dbg_uart_fflush(void);
void dbgUartwait(void);

#endif /* INCLUDE_DBG_UART_H_ */

