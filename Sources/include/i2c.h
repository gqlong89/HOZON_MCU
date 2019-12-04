#ifndef _I2C_H_
#define _I2C_H_

#include <sys/_stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define I2C_INSTANCE                (0U)

void driverI2CInit(void);
void driverI2CDeinit(void);
int driverI2CWrite(uint16_t slave_addr, unsigned int addr, unsigned char *buf,
                   unsigned int num);
int driverI2CRead(uint16_t slave_addr, unsigned int addr, unsigned char *buf,
                  unsigned int num);

int shell_i2c_read(unsigned int argc, unsigned char **argv);


#endif

