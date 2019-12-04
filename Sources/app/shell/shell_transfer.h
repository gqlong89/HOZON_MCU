/**
 * @Title: shell_transfer.h
 * @author yuzhimin
 * @date Jul 25, 2017
 * @version V1.0
 */
#ifndef APP_SHELL_SHELL_TRANSFER_H_
#define APP_SHELL_SHELL_TRANSFER_H_

#include <sys/_stdint.h>

#define XMODEM_SOH (0x01)
#define XMODEM_STX (0x02)
#define XMODEM_EOT (0x04)
#define XMODEM_ACK (0x06)
#define XMODEM_NAK (0x15)
#define XMODEM_ETB (0x17)
#define XMODEM_CAN (0x18)
#define XMODEM_C   (0x43)

#define XMODEM_SND (1)
#define XMODEM_RCV (2)
#define XMODEM_PRO (3)

#define XMODEM_SUB (0x1A)

uint32_t shell_read(uint8_t **bufPtr, uint32_t *length);
uint32_t shell_write(uint8_t *buf, uint32_t length);

#endif /* APP_SHELL_SHELL_TRANSFER_H_ */

