/*
 * ble_app.h
 *      Author: Maarten Weyn
 */

#ifndef BLE_APP_H_
#define BLE_APP_H_

#include <sdk_errors.h> 
#include <bsp.h> 
#include <ble_nus.h> 

typedef uint32_t (*app_indication_set_handler_t)(bsp_indication_t indicate);

typedef void (*nus_data_handler_t) (ble_nus_evt_t * p_evt);

ret_code_t ble_stack_init(app_indication_set_handler_t handler);
ret_code_t gap_params_init();
ret_code_t gatt_init();
ret_code_t services_init(nus_data_handler_t handler);
ret_code_t advertising_init();
ret_code_t conn_params_init();
ret_code_t advertising_start();
ret_code_t slow_advertising_init(void);
ret_code_t slow_advertising_start(void);
void sleep_mode_enter(void);

ret_code_t send_data(uint8_t * p_data, uint16_t  length);

#endif /* BLE_APP_H_ */