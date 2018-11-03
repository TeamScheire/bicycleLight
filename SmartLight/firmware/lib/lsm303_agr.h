#ifndef LSM303AGR_H__
#define LSM303AGR_H__

#include "LSM303AGR_acc_driver.h"

#include <stdint.h>
#include <stdbool.h>

mems_status_t lsm303agr_init();
mems_status_t lsm303agr_acc_set_x_odr(uint8_t odr);
mems_status_t lsm303agr_acc_set_x_fs(uint8_t fullScale);
mems_status_t lsm303agr_acc_set_low_power_mode(bool lp_mode, bool hr_mode);
mems_status_t lsm303agr_acc_get_acceleration(int32_t *buff);


#endif