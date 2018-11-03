/*
 * ble_app.h
 *      Author: Maarten Weyn
 */

#include "ble_app.h"
#include "config.h"
#include "i2c.h"

#include "lp55231.h"

#include <nrf.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_delay.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <nrf_sdh_soc.h>

#include <nrf_ble_gatt.h>
#include <nrf_ble_qwr.h>

#include <ble_advdata.h>
#include <ble_advertising.h>
#include <ble_conn_params.h>
#include <ble_hci.h>
#include <ble_nus.h>

#include <bsp.h>

static app_indication_set_handler_t app_indication_set;
uint8_t ble_state = 0; //0 not connected, 1 connected, 2 adverstising

BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT); /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                         /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                           /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);               /**< Advertising module instance. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;               /**< Handle of the current connection. */
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3; /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[] =                                      /**< Universally unique service identifier. */
    {
        {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};

static uint32_t indication_handler(bsp_indication_t indicate) {
  if (app_indication_set != NULL) {
    return app_indication_set(indicate);
  } else {
    NRF_LOG_INFO("App indication set function not set");
    return NRF_ERROR_SVC_HANDLER_MISSING;
  }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
  uint32_t err_code;

  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_CONNECTED:
    NRF_LOG_INFO("Connected");
    
    ble_state = 1;
    err_code = indication_handler(BSP_INDICATE_CONNECTED);

    //APP_ERROR_CHECK(err_code);
    if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
      NRF_LOG_INFO("indication_handler return with error %d", err_code);

    m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
    break;

  case BLE_GAP_EVT_DISCONNECTED:
    NRF_LOG_INFO("Disconnected");
    ble_state = 0;

    // LED indication will be changed when advertising starts.
    m_conn_handle = BLE_CONN_HANDLE_INVALID;    
    err_code = indication_handler(BSP_INDICATE_USER_STATE_2);
    break;

  case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
    NRF_LOG_DEBUG("PHY update request.");
    ble_gap_phys_t const phys =
        {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
    err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
  } break;

  case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    // Pairing not supported
    err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
    break;

  case BLE_GATTS_EVT_SYS_ATTR_MISSING:
    // No system attributes have been stored.
    err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
    break;

  case BLE_GATTC_EVT_TIMEOUT:
    // Disconnect on GATT Client timeout event.
    err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    break;

  case BLE_GATTS_EVT_TIMEOUT:
    // Disconnect on GATT Server timeout event.
    err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    break;

  default:
    // No implementation needed.
     NRF_LOG_INFO("ble_evt_handler  %x", p_ble_evt->header.evt_id); 
    err_code = NRF_SUCCESS;
    break;
  }

  if (err_code != NRF_SUCCESS) {
    NRF_LOG_INFO("ble_evt_handler error %d", err_code);
    err_code = indication_handler(BSP_INDICATE_ALERT_1);
    if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
      NRF_LOG_INFO("indication_handler return with error %d", err_code);
  }
}

/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
ret_code_t ble_stack_init(app_indication_set_handler_t handler) {
  ret_code_t err_code;

  app_indication_set = handler;

  err_code = nrf_sdh_enable_request();
  if (err_code != NRF_SUCCESS)
    return err_code;

  // Configure the BLE stack using the default settings.
  // Fetch the start address of the application RAM.
  uint32_t ram_start = 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
  if (err_code != NRF_SUCCESS)
    return err_code;

  // Enable BLE stack.
  err_code = nrf_sdh_ble_enable(&ram_start);
  if (err_code != NRF_SUCCESS)
    return err_code;

  // Register a handler for BLE events.
  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
ret_code_t gap_params_init(void) {
  uint32_t err_code;
  ble_gap_conn_params_t gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode,
      (const uint8_t *)DEVICE_NAME,
      strlen(DEVICE_NAME));
  if (err_code != NRF_SUCCESS)
    return err_code;

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  if (err_code != NRF_SUCCESS)
    return err_code;
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt) {
  if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
    m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
  }
  NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
      p_gatt->att_mtu_desired_central,
      p_gatt->att_mtu_desired_periph);
}

/**@brief Function for initializing the GATT library. */
ret_code_t gatt_init(void) {
  ret_code_t err_code;

  err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
  if (err_code != NRF_SUCCESS)
    return err_code;

  err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
  if (err_code != NRF_SUCCESS)
    return err_code;
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);

  ret_code_t err_code = indication_handler(BSP_INDICATE_FATAL_ERROR);
  if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
    NRF_LOG_INFO("indication_handler return with error %d", err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
ret_code_t services_init(nus_data_handler_t handler) {
  uint32_t err_code;
  ble_nus_init_t nus_init;
  nrf_ble_qwr_init_t qwr_init = {0};

  // Initialize Queued Write Module.
  qwr_init.error_handler = nrf_qwr_error_handler;

  err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  if (err_code != NRF_SUCCESS)
    return err_code;

  // Initialize NUS.
  memset(&nus_init, 0, sizeof(nus_init));

  nus_init.data_handler = handler;

  err_code = ble_nus_init(&m_nus, &nus_init);
  if (err_code != NRF_SUCCESS)
    return err_code;
}

uint32_t btn_ble_sleep_mode_prepare(void)
{
    uint32_t err_code;
    #ifdef BUTTON1
    err_code = bsp_wakeup_button_enable(0);
    if (err_code != NRF_SUCCESS) return err_code;
    //RETURN_ON_ERROR_NOT_NOT_SUPPORTED(err_code);
    #endif 

    #ifdef BUTTON2
    err_code = bsp_wakeup_button_enable(1);
    if (err_code != NRF_SUCCESS) return err_code;
    //RETURN_ON_ERROR_NOT_NOT_SUPPORTED(err_code);
    #endif

    #ifdef BUTTON3
    err_code = bsp_wakeup_button_enable(2);
    if (err_code != NRF_SUCCESS) return err_code;
    //RETURN_ON_ERROR_NOT_NOT_SUPPORTED(err_code);
    #endif

    return NRF_SUCCESS;
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void)
{
  uint32_t err_code = indication_handler(BSP_INDICATE_IDLE);
  if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
    NRF_LOG_INFO("indication_handler return with error %d", err_code);

  NRF_LOG_INFO("sleep_mode_enter sleep_status  %d", sleep_status);
  while (NRF_LOG_PROCESS());
  if (sleep_status)
  {
  }
  else
  {
    // Prepare wakeup buttons.
    err_code = btn_ble_sleep_mode_prepare();

    APP_ERROR_CHECK(err_code);
    lp55231_disable(ADDRESS0);
    lp55231_disable(ADDRESS1);
    lp55231_disable(ADDRESS2);

    i2c_deinit();


     // Go to system-off mode (this function will not return; wakeup will cause a reset).
     nrf_delay_ms(100);
     err_code = sd_power_system_off();
     //APP_ERROR_CHECK(err_code);
     NRF_LOG_INFO("sd_power_system_off done");
     while (NRF_LOG_PROCESS());
  }
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
  uint32_t err_code;

  switch (ble_adv_evt) {
  case BLE_ADV_EVT_FAST:
    err_code = indication_handler(BSP_INDICATE_ADVERTISING);
    if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
      NRF_LOG_INFO("on_adv_evt indication_handler return with error %d", err_code);
    break;
  case BLE_ADV_EVT_SLOW:
    err_code = indication_handler(BSP_INDICATE_ADVERTISING);
    if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
      NRF_LOG_INFO("on_adv_evt indication_handler return with error %d", err_code);
    break;
  case BLE_ADV_EVT_IDLE:
    NRF_LOG_INFO("on_adv_evt BLE_ADV_EVT_IDLE");
    //if (ble_state > 0)
    //{
    //  NRF_LOG_INFO("Start slow adverstising");
    //  slow_advertising_init();
    //  slow_advertising_start();
    //} else {
      ble_state = 0;
      sleep_mode_enter();
    //}
    break;
  default:
    break;
  }
}

/**@brief Function for initializing the Advertising functionality.
 */
ret_code_t advertising_init(void) {
  uint32_t err_code;
  ble_advertising_init_t init;

  memset(&init, 0, sizeof(init));

  init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance = false;
  //init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
  init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; //allow unlimited timeout

  init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  init.srdata.uuids_complete.p_uuids = m_adv_uuids;

  init.config.ble_adv_fast_enabled = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

  init.config.ble_adv_slow_enabled = false;

  init.evt_handler = on_adv_evt;

  err_code = ble_advertising_init(&m_advertising, &init);
  if (err_code != NRF_SUCCESS)
    return err_code;

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

ret_code_t slow_advertising_init(void) {
  uint32_t err_code;
  ble_advertising_init_t init;

  memset(&init, 0, sizeof(init));

  init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance = false;
  //init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
  init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; //allow unlimited timeout

  init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  init.srdata.uuids_complete.p_uuids = m_adv_uuids;

  init.config.ble_adv_slow_enabled = true;
  init.config.ble_adv_slow_interval = SLOW_APP_ADV_INTERVAL;
  init.config.ble_adv_slow_timeout = SLOW_APP_ADV_TIMEOUT_IN_SECONDS;

  init.evt_handler = on_adv_evt;

  err_code = ble_advertising_init(&m_advertising, &init);
  if (err_code != NRF_SUCCESS)
    return err_code;

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt) {
  uint32_t err_code;

  if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
    if (err_code != NRF_SUCCESS) {
      err_code = indication_handler(BSP_INDICATE_FATAL_ERROR);
      if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
        NRF_LOG_INFO("indication_handler return with error %d", err_code);
    }
  }
}

/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);
  ret_code_t err_code = indication_handler(BSP_INDICATE_FATAL_ERROR);
  if (err_code > NRF_ERROR_SVC_HANDLER_MISSING)
    NRF_LOG_INFO("conn_params_error_handler indication_handler return with error %d", err_code);
}

/**@brief Function for initializing the Connection Parameters module.
 */
ret_code_t conn_params_init(void) {
  uint32_t err_code;
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail = false;
  cp_init.evt_handler = on_conn_params_evt;
  cp_init.error_handler = conn_params_error_handler;

  err_code = ble_conn_params_init(&cp_init);
    return err_code;
}

/**@brief Function for starting advertising.
 */
ret_code_t advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    ble_state = 2;
    return err_code;
}

ret_code_t slow_advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_SLOW);
    ble_state = 0;
    return err_code;
}

ret_code_t send_data(uint8_t * p_data, uint16_t  length)
{
  return ble_nus_data_send(&m_nus, p_data, &length, m_conn_handle);
}

