/*
 * lp55231.c
 *
 *  Created on: 30 Apr 2018
 *      Author: Fifth
 */
#include "lp55231.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_error.h"
#include "nrf_delay.h"

#include "i2c.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/*

#define LP55231_READ(dev_addr, p_reg_addr, p_buffer, byte_cnt)       \
  NRF_TWI_MNGR_WRITE(dev_addr, p_reg_addr, 1, NRF_TWI_MNGR_NO_STOP), \
      NRF_TWI_MNGR_READ(dev_addr, p_buffer, byte_cnt, 0)

static uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND enable_config02[] = {REG_MISC, 0x53};
static uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND enable_config01[] = {REG_CNTRL1, 0x40};
static uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND disable_config[] = {REG_CNTRL1, 0x00}; // to check value first

static uint8_t all_devices[] = { address1, address2, address3};

// Buffer for data read from sensors.
#define BUFFER_SIZE 11
static uint8_t m_buffer[BUFFER_SIZE];

nrf_twi_mngr_transfer_t const lp55231_enable_transfers[LP55231_ENABLE_TRANSFERS] =
    {
        // and re-enable
        NRF_TWI_MNGR_WRITE(address1, enable_config01, sizeof(enable_config01), 0),
        // enable internal clock & charge pump & auto increment
        NRF_TWI_MNGR_WRITE(address1, enable_config02, sizeof(enable_config02), 0),
        NRF_TWI_MNGR_WRITE(address2, enable_config01, sizeof(enable_config01), 0),
        NRF_TWI_MNGR_WRITE(address2, enable_config02, sizeof(enable_config02), 0),
        NRF_TWI_MNGR_WRITE(address3, enable_config01, sizeof(enable_config01), 0),
        NRF_TWI_MNGR_WRITE(address3, enable_config02, sizeof(enable_config02), 0)};

nrf_twi_mngr_transfer_t const lp55231_disable_transfers[3] =
    {
        NRF_TWI_MNGR_WRITE(address1, disable_config, sizeof(disable_config), 0),
        NRF_TWI_MNGR_WRITE(address2, disable_config, sizeof(disable_config), 0),
        NRF_TWI_MNGR_WRITE(address3, disable_config, sizeof(disable_config), 0)};
*/

ret_code_t lp55231_enable(uint8_t address) {
  // and re-enable
  ret_code_t err_code = i2c_write_register(REG_CNTRL1, 0x40,address);
  // enable internal clock & charge pump & auto increment
  err_code = i2c_write_register(REG_MISC, 0x53,address);

/*
  return (nrf_twi_mngr_perform(twi_mngr, NULL, lp55231_enable_transfers,
      LP55231_ENABLE_TRANSFERS, NULL));
      */
}
//
//static void disable_cb(ret_code_t result, void *p_user_data) {
//  if (result != NRF_SUCCESS) {
//    NRF_LOG_WARNING("disable_cb - error: %d", (int)result);
//    return;
//  }
//
//  m_buffer[0] &= ~0x40;
//  m_buffer[1] &= ~0x40;
//  m_buffer[2] &= ~0x40;
//
//  static uint8_t config[] = {REG_CNTRL1, m_buffer[0]};
//  //todo: use same value for all?
//  static nrf_twi_mngr_transfer_t const transfers[] =
//      {
//          NRF_TWI_MNGR_WRITE(address1, config, 2, 0),
//          NRF_TWI_MNGR_WRITE(address2, config, 2, 0),
//          NRF_TWI_MNGR_WRITE(address3, config, 2, 0),
//      };
//
//  static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
//      {
//          .callback = NULL,
//          .p_user_data = NULL,
//          .p_transfers = transfers,
//          .number_of_transfers = sizeof(transfers) / sizeof(transfers[0])};
//
//  ret_code_t err_code = nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &transaction);
//
//
//  if (err_code != NRF_SUCCESS) {
//    NRF_LOG_WARNING("disable_cb nrf_twi_mngr_perform - error: %d", (int)error_code);
//    return;
//  }
//}

ret_code_t lp55231_disable(uint8_t address) {
/*
  ret_code_t err_code = app_twi_perform_single_read(address1, REG_CNTRL1, &m_buffer[0]);

  m_buffer[0] &= ~0x40;

  err_code = app_twi_perform_single_write(address1, REG_CNTRL1, m_buffer[0]);
  err_code = app_twi_perform_single_write(address2, REG_CNTRL1, m_buffer[0]);
  err_code = app_twi_perform_single_write(address3, REG_CNTRL1, m_buffer[0]);

  return err_code;
  */
  uint8_t val;
  ret_code_t err_code = i2c_read_register (REG_CNTRL1, &val, address);
  val &= ~0x40;
  
  
  err_code = i2c_write_register(REG_CNTRL1, val,address);
}

ret_code_t lp55231_reset() {
  NRF_LOG_INFO("reset %x", address1);
  // force reset

  ret_code_t err_code = i2c_write_register(REG_RESET, 0xff, address1);
  //ret_code_t err_code=nrf_drv_twi_tx(twi_han2, device, pData, 2, 0);
  NRF_LOG_INFO("return %x", address1);

  nrf_delay_ms(100);

  return  err_code;
}


uint8_t lp55231_setBrightness(uint8_t channel, uint8_t value, uint8_t device) {
  return i2c_write_register(REG_D1_PWM + channel, value, device);

  //return  app_twi_perform_single_write(device, REG_D1_PWM + channel, value);

}

uint8_t leds_set_all(uint8_t value) {
ret_code_t err_code;
/*
  uint8_t data[9] = {value, value, value, value, value, value, value, value, value};

  ret_code_t err_code =  app_twi_perform_multi_write(address1, REG_D1_PWM, data, 9);
  err_code =  app_twi_perform_multi_write(address2, REG_D1_PWM, data, 9);
  err_code =  app_twi_perform_multi_write(address3, REG_D1_PWM, data, 9);
  */
  uint8_t data[10]= { REG_D1_PWM,value,value,value,value,value,value,value,value,value};
  err_code =i2c_write(data, 13, address1, 0);
  err_code =i2c_write(data, 13, address2, 0);
  err_code =i2c_write(data, 13, address3, 0);

  return err_code;
}