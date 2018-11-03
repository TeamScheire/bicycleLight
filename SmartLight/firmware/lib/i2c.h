#ifndef I2C_H__
#define I2C_H__

#include <stdint.h>
#include <stdbool.h>

#include <nrf_twi_mngr.h>

#define TWI_INSTANCE_ID     0
#define MAX_PENDING_TRANSACTIONS    5
#define TWI_TIMEOUT 30000

extern const nrf_twi_mngr_t* twi_mngr;

void i2c_init (void);
void i2c_deinit(void);


#define MAX_MULTI_DEVICE_NUM 5

uint8_t i2c_write(uint8_t data[], uint8_t length, uint8_t device_address, bool no_stop);
uint8_t i2c_read(uint8_t buf[], uint8_t reg, uint8_t length, uint8_t device_address);

uint8_t i2c_read_register (uint8_t reg, uint8_t* val, uint8_t device_address);
uint8_t i2c_write_register(uint8_t reg, uint8_t val, uint8_t device_address);


 #endif