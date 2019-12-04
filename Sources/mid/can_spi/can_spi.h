/**
 * @Title: can_spi.h
 * @author yuzhimin
 * @date Jul 21, 2017
 * @version V1.0
 */
#ifndef MID_CAN_SPI_CAN_SPI_H_
#define MID_CAN_SPI_CAN_SPI_H_

#include <stdbool.h>
#include <sys/_stdint.h>

/*! @brief Device instance number */
#define LPSPICAN1 (1U)      //  CAN_PORT4
#define LPSPICAN2 (2U)      //  CAN_PORT5

#define SPI_2_CAN(x)    (x+2)
#define CAN_2_SPI(x)    (x-2)

#define SPI_TIMEOUT        (10)     // 10*uptime = 100ms
//#define SPI_TIMEOUT          (OSIF_WAIT_FOREVER)

/*send default length (data 8, SID 2, EID 2,DLC 1,command 1)*/
#define SEND_LEN_DEF    (8+4+1+1)

#define MCP_SPEED       500000

#define CAN4_INT_GPIO   PTA
#define CAN4_RST_GPIO   PTA
#define CAN4_INT_PORT   PORTA
#define CAN4_RST_PORT   PORTA

#define CAN4_INT_PIN     10
#define CAN4_RST_PIN     11

#define CAN5_INT_GPIO   PTE
#define CAN5_RST_GPIO   PTE
#define CAN5_INT_PORT   PORTE
#define CAN5_RST_PORT   PORTE

#define CAN5_INT_PIN     10
#define CAN5_RST_PIN     11

#define CAN4_INT_IRQn   PORTA_IRQn
#define CAN5_INT_IRQn   PORTE_IRQn

#define ERR_NO          0
#define ERR_BUSY        1
#define ERR_OVER        2
#define ERR_BUS_OFF     3
#define ERR_MOD         4

uint8_t spican_baud(uint32_t instance, uint32_t baud);
uint32_t spican_init(uint32_t instance, uint32_t baud);
uint8_t spican_send(uint32_t instance, unsigned char *bufferPtr, uint8_t num);
bool Get_send_sta(uint32_t instance, uint8_t num, uint8_t *perr);
//bool spican_receive( uint32_t instance, uint8_t *bufferPtr, uint8_t *plen );
bool spican_deinit(uint32_t instance);
void can_spi_test(void);

#endif /* MID_CAN_SPI_CAN_SPI_H_ */
