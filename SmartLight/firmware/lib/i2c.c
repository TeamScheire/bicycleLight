#include "i2c.h"

#include "app_error.h"
#include "nrf_drv_twi.h"

#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_delay.h"


/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done;

void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context);

/* TWI instance. */
//static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
NRF_TWI_MNGR_DEF(m_nrf_twi_mngr, MAX_PENDING_TRANSACTIONS, TWI_INSTANCE_ID);
const nrf_twi_mngr_t *twi_mngr = &m_nrf_twi_mngr;

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done;

void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context);

void i2c_init(void) {
  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_config = {
      .scl = 31,
      .sda = 2,
      .frequency = NRF_TWI_FREQ_400K,
      .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //APP_IRQ_PRIORITY_LOWEST
      .clear_bus_init = false};

  err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);

  nrf_drv_twi_uninit(&m_twi);
  *(uint32_t *)(0x40003000 + 0xFFC ) = 0; //http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.Rev1.errata%2Fanomaly_832_89.html&cp=2_2_1_0_1_26
  *(uint32_t *)(0x40003000 + 0xFFC ) = 1;
  err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);

  //err_code = nrf_twi_mngr_init(&m_nrf_twi_mngr, &twi_config);
  APP_ERROR_CHECK(err_code);

  nrf_drv_twi_enable(&m_twi);
}

void i2c_deinit(void) {
  nrf_drv_twi_uninit(&m_twi);
}


uint8_t i2c_write(uint8_t data[], uint8_t length, uint8_t addr, bool no_stop) {
  NRF_LOG_DEBUG("writing %x%x of length %d to %x\n", data[0], data[1], length, addr);

  ret_code_t err_code;
  uint32_t timeout = TWI_TIMEOUT;
  m_xfer_done = false;

  err_code = nrf_drv_twi_tx(&m_twi, addr, data, length, no_stop);
  if (err_code != NRF_SUCCESS)
    return err_code;

  while ((!m_xfer_done) && --timeout)
    ;
  if (!timeout) {
    NRF_LOG_DEBUG("timout");
    return NRF_ERROR_TIMEOUT;
  }

  return err_code;
}

uint8_t i2c_read(uint8_t buf[], uint8_t reg, uint8_t length, uint8_t device_address) {
  NRF_LOG_DEBUG("reading register %x", reg);

  
  uint32_t timeout = TWI_TIMEOUT;

  ret_code_t err_code;
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, device_address, &reg, 1, true);

  if (err_code != NRF_SUCCESS)
    return err_code;

  while ((!m_xfer_done) && --timeout)
    ;
  if (!timeout) {
    NRF_LOG_DEBUG("timout");
    return NRF_ERROR_TIMEOUT;
  }

  timeout = TWI_TIMEOUT;

  m_xfer_done = false;
  err_code = nrf_drv_twi_rx(&m_twi, device_address, buf, length);
  if (err_code != NRF_SUCCESS)
    return err_code;

  if ( buf[0] != 0x00)
  {
   NRF_LOG_DEBUG("RX %x", buf[0]);
  }

 

  while ((!m_xfer_done) && --timeout)
    ;
  if (!timeout) {
    NRF_LOG_DEBUG("timout");
    return NRF_ERROR_TIMEOUT;
  }

  return err_code;
}

uint8_t i2c_read_register(uint8_t reg, uint8_t *val, uint8_t device_address) {
  return i2c_read(val, reg, 1, device_address);
}

uint8_t i2c_write_register(uint8_t reg, uint8_t val, uint8_t device_address) {
  uint8_t data[2];
  data[0] = reg;
  data[1] = val;

  return i2c_write(data, 2, device_address, 0);
}

void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context) {
  NRF_LOG_DEBUG("twi_handler %d", p_event->type);
  m_xfer_done = true;
  switch (p_event->type) {
  case NRF_DRV_TWI_EVT_DONE:
    NRF_LOG_DEBUG("twi_handler NRF_DRV_TWI_EVT_DONE");

    if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX) {
      uint8_t m_sample = 1;
      //  twi_data_handler(m_sample);
    }

    m_xfer_done = true;
    break;
  case NRF_DRV_TWI_EVT_ADDRESS_NACK:
    NRF_LOG_DEBUG("twi_handler NRF_DRV_TWI_EVT_ADDRESS_NACK");
    break;
  default:
    break;
  }
}