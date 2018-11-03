/*
 * lp55231.h
 *
 */

#ifndef LP55231_H_
#define LP55231_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "nrf_drv_twi.h"
#include "nrf_twi_mngr.h"

#define address1 0x35 // 32 34
#define address2 0x32 // 32 34
#define address3 0x34 // 32 34
// register stuff
#define REG_CNTRL1 0x00
#define REG_CNTRL2 0x01
#define REG_RATIO_MSB 0x02
#define REG_RATIO_LSB 0x03
#define REG_OUTPUT_ONOFF_MSB 0x04
#define REG_OUTPUT_ONOFF_LSB 0x05

// Per LED control channels - fader channel assig, log dimming enable, temperature compensation
#define REG_D1_CTRL 0x06
#define REG_D2_CTRL 0x07
#define REG_D3_CTRL 0x08
#define REG_D4_CTRL 0x09
#define REG_D5_CTRL 0x0a
#define REG_D6_CTRL 0x0b
#define REG_D7_CTRL 0x0c
#define REG_D8_CTRL 0x0d
#define REG_D9_CTRL 0x0e

// 0x0f to 0x15 reserved

// Direct PWM control registers
#define REG_D1_PWM 0x16
#define REG_D2_PWM 0x17
#define REG_D3_PWM 0x18
#define REG_D4_PWM 0x19
#define REG_D5_PWM 0x1a
#define REG_D6_PWM 0x1b
#define REG_D7_PWM 0x1c
#define REG_D8_PWM 0x1d
#define REG_D9_PWM 0x1e

// 0x1f to 0x25 reserved

// Drive current registers
#define REG_D1_I_CTL 0x26
#define REG_D2_I_CTL 0x27
#define REG_D3_I_CTL 0x28
#define REG_D4_I_CTL 0x29
#define REG_D5_I_CTL 0x2a
#define REG_D6_I_CTL 0x2b
#define REG_D7_I_CTL 0x2c
#define REG_D8_I_CTL 0x2d
#define REG_D9_I_CTL 0x2e

// 0x2f to 0x35 reserved

#define REG_MISC 0x36
#define REG_PC1 0x37
#define REG_PC2 0x38
#define REG_PC3 0x39
#define REG_STATUS_IRQ 0x3A
#define REG_INT_GPIO 0x3B
#define REG_GLOBAL_VAR 0x3C
#define REG_RESET 0x3D
#define REG_TEMP_CTL 0x3E
#define REG_TEMP_READ 0x3F
#define REG_TEMP_WRITE 0x40
#define REG_TEST_CTL 0x41
#define REG_TEST_ADC 0x42

// 0x43 to 0x44 reserved

#define REG_ENGINE_A_VAR 0x45
#define REG_ENGINE_B_VAR 0x46
#define REG_ENGINE_C_VAR 0x47

#define REG_MASTER_FADE_1 0x48
#define REG_MASTER_FADE_2 0x49
#define REG_MASTER_FADE_3 0x4A

// 0x4b Reserved

#define REG_PROG1_START 0x4C
#define REG_PROG2_START 0x4D
#define REG_PROG3_START 0x4E
#define REG_PROG_PAGE_SEL 0x4f

// Memory is more confusing - there are 6 pages, sel by addr 4f
#define REG_PROG_MEM_BASE 0x50
//#define REG_PROG_MEM_SIZE   0x //
#define REG_PROG_MEM_END 0x6f

#define REG_ENG1_MAP_MSB 0x70
#define REG_ENG1_MAP_LSB 0x71
#define REG_ENG2_MAP_MSB 0x72
#define REG_ENG2_MAP_LSB 0x73
#define REG_ENG3_MAP_MSB 0x74
#define REG_ENG3_MAP_LSB 0x75

#define REG_GAIN_CHANGE 0x76


#define LP55231_ENABLE_TRANSFERS 6
#define LP55231_DISABLE_TRANSFERS 3

// fundamental operations
ret_code_t lp55231_enable(uint8_t address);
ret_code_t lp55231_disable(uint8_t address);
ret_code_t lp55231_reset();

// basic LED control functions
uint8_t lp55231_setBrightness(uint8_t channel, uint8_t value, uint8_t device);
//uint8_t lp55231_setLogBrightness(uint8_t channel,uint8_t device)
//uint8_t lp55231_setDriveCurrent(uint8_t channel, uint8_t value,uint8_t device)

uint8_t leds_set_all(uint8_t value);

#endif /* LP55231_H_ */