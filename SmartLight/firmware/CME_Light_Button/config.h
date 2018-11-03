#include <ble_types.h>

#include <app_timer.h>


#define TIMER_INTERVAL                  APP_TIMER_TICKS(1000)                       /**< interval (ticks). */

// BLE

#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define DEVICE_NAME                     "bicycle_CME"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define SLOW_APP_ADV_INTERVAL           3200
#define SLOW_APP_ADV_TIMEOUT_IN_SECONDS 0


// Buttons
//#define BUTTON1 9
//#define BUTTON2 10
//#define BUTTON3 26
//
//#define GREEN_BUTTON  BUTTON1
//#define RED_BUTTON    BUTTON3
//#define LIGHT_BUTTON  BUTTON2

#define BUTTON1 28
#define BUTTON2 29
#define BUTTON3 30
//#define BUTTON4 8

#define GREEN_BUTTON  BUTTON3
#define RED_BUTTON    BUTTON1

//#define ALARM_BUTTON  BUTTON2
#define LIGHT_BUTTON  BUTTON2


// LEDS

#define ADDRESS0 0x35 
#define ADDRESS1 0x32 
#define ADDRESS2 0x34 

#define LED8RED_ADDR    2
#define LED8RED_ID      7
#define LED8GREEN_ADDR  2
#define LED8GREEN_ID    2
#define LED8BLUE_ADDR   2
#define LED8BLUE_ID     3

#define LED7RED_ADDR    2
#define LED7RED_ID      8
#define LED7GREEN_ADDR  2
#define LED7GREEN_ID    4
#define LED7BLUE_ADDR   2
#define LED7BLUE_ID     5

#define LED6RED_ADDR    0
#define LED6RED_ID      6
#define LED6GREEN_ADDR  0
#define LED6GREEN_ID    0
#define LED6BLUE_ADDR   0
#define LED6BLUE_ID     1

#define LED5RED_ADDR    0
#define LED5RED_ID      7
#define LED5GREEN_ADDR  0
#define LED5GREEN_ID    2
#define LED5BLUE_ADDR   0
#define LED5BLUE_ID     3

#define LED4RED_ADDR    0
#define LED4RED_ID      8
#define LED4GREEN_ADDR  0
#define LED4GREEN_ID    4
#define LED4BLUE_ADDR   0
#define LED4BLUE_ID     5


#define LED3RED_ADDR    1
#define LED3RED_ID      7
#define LED3GREEN_ADDR  1
#define LED3GREEN_ID    2
#define LED3BLUE_ADDR   1
#define LED3BLUE_ID     3

#define LED2RED_ADDR    1
#define LED2RED_ID      6
#define LED2GREEN_ADDR  1
#define LED2GREEN_ID    0
#define LED2BLUE_ADDR   1
#define LED2BLUE_ID     1

#define LED1RED_ADDR    2
#define LED1RED_ID      6
#define LED1GREEN_ADDR  2
#define LED1GREEN_ID    0
#define LED1BLUE_ADDR   2
#define LED1BLUE_ID     1


#define LED_INDICATE_SENT_OK               1
#define LED_INDICATE_SEND_ERROR            1
#define LED_INDICATE_RCV_OK                1
#define LED_INDICATE_RCV_ERROR             1
#define LED_INDICATE_CONNECTED             0
#define LED_INDICATE_BONDING               0
#define LED_INDICATE_ADVERTISING_DIRECTED  0
#define LED_INDICATE_ADVERTISING_SLOW      0
#define LED_INDICATE_ADVERTISING_WHITELIST 0
#define LED_INDICATE_ADVERTISING  0

#define LED_INDICATE_TX  0

#define LED_INDICATE_BUTTON_STATE_01         1
#define LED_INDICATE_BUTTON_STATE_02         7

#define RED 1
#define GREEN 2
#define BLUE  4
#define BLACK  8
#define WHITE 7

#define RED_ID   0
#define GREEN_ID 1
#define BLUE_ID  2

#define LED_INDICATE_ADVERTISING_COLOR      BLUE
#define LED_INDICATE_CONNECTED_COLOR        GREEN
#define LED_INDICATE_TX_COLOR               GREEN
#define LED_INDICATE_BOOT                   GREEN
#define LED_INDICATE_BOOT_DEBUG             RED

#define LED_INDICATE_SENT_OK_COLOR         GREEN
#define LED_INDICATE_SENT_ERROR_COLOR         RED

#define LED_INDICATE_ADVERTISING_COLOR_ID      BLUE_ID
#define LED_INDICATE_CONNECTED_COLOR_ID        GREEN_ID
#define LED_INDICATE_BOOT_ID                   GREEN_ID
#define LED_INDICATE_BOOT_DEBUG_ID             RED_ID
#define LED_INDICATE_SENT_OK_COLOR_ID          GREEN_ID
#define LED_INDICATE_SENT_ERROR_COLOR_ID       RED_ID

#define LED_BRIGHTNESS  10


#define ADVERTISING_LED_ON_INTERVAL             100
#define ADVERTISING_LED_OFF_INTERVAL            4900

#define CONNECTED_LED_ON_INTERVAL               100
#define CONNECTED_LED_OFF_INTERVAL              3000

#define ADVERTISING_DIRECTED_LED_ON_INTERVAL   200
#define ADVERTISING_DIRECTED_LED_OFF_INTERVAL  200

#define ADVERTISING_WHITELIST_LED_ON_INTERVAL  200
#define ADVERTISING_WHITELIST_LED_OFF_INTERVAL 800

#define ADVERTISING_SLOW_LED_ON_INTERVAL       400
#define ADVERTISING_SLOW_LED_OFF_INTERVAL      4000

#define BONDING_INTERVAL                       100

#define SENT_OK_INTERVAL                       100
#define SEND_ERROR_INTERVAL                    500

#define RCV_OK_INTERVAL                        100
#define RCV_ERROR_INTERVAL                     500

#define ALERT_INTERVAL                         500

#define LIGHT_BUTTON_CHANGE_INTERVAL            1000
#define BUTTON_TIMEOUT_INTERVAL                 10000
#define BUTTON_ACK_FLASH_INTERVAL               500

extern uint8_t sleep_status;