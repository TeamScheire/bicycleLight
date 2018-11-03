#include "lsm303_agr.h"

bool lsm303agr_acc_is_initialized = false;
void *lsm303agr_i2c = 0;

static LSM303AGR_ACC_ODR_t acc_current_odr = 0;
static LSM303AGR_ACC_FS_t acc_current_fs = 0;
static LSM303AGR_ACC_LPEN_t lp;
static LSM303AGR_ACC_HR_t hr = LSM303AGR_ACC_HR_DISABLED;
static uint8_t mode_shift = 8; // 8bit, 10 bit, 12 bit
static uint8_t op_mode = 2;
static uint8_t fs_mode = 0;

uint8_t acc_is_enabled = 0;

/*
 * Following is the table of sensitivity values for each case.
 * Values are espressed in ug/digit.
 */
const long long LSM303AGR_ACC_Sensitivity_List[3][4] = {
    /* HR 12-bit */
    {
       980, /* FS @2g */
       1950,    /* FS @4g */
       3900,    /* FS @8g */
      11720,    /* FS @16g */
    },

    /* Normal 10-bit */
    {
      3900, /* FS @2g */
      7820, /* FS @4g */
      15630,    /* FS @8g */
      46900,    /* FS @16g */
    },

    /* LP 8-bit */
    {
      15630,    /* FS @2g */
      31260,    /* FS @4g */
      62520,    /* FS @8g */
      187580,   /* FS @16g */
    },
};


mems_status_t lsm303agr_init() {
  uint8_t buffer[10];

  if (LSM303AGR_ACC_R_WHO_AM_I((void *)lsm303agr_i2c, buffer)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  if (buffer[0] != LSM303AGR_ACC_WHO_AM_I)
  {
  return MEMS_ERROR;
  }

  /* reboot */
  if (LSM303AGR_ACC_W_RebootMemory((void *)lsm303agr_i2c, LSM303AGR_ACC_BOOT_REBOOT)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  /* Enable BDU */
  if (LSM303AGR_ACC_W_BlockDataUpdate((void *)lsm303agr_i2c, LSM303AGR_ACC_BDU_ENABLED)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  /* FIFO mode selection */
  if (LSM303AGR_ACC_W_FifoMode((void *)lsm303agr_i2c, LSM303AGR_ACC_FM_BYPASS)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  /* Output data rate selection - power down. */
  if (LSM303AGR_ACC_W_ODR((void *)lsm303agr_i2c, LSM303AGR_ACC_ODR_DO_PWR_DOWN)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  /* Full scale selection. */
  if (lsm303agr_acc_set_x_fs(2)   == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  /* Enable axes. */
  if (LSM303AGR_ACC_W_XEN((void *)lsm303agr_i2c, LSM303AGR_ACC_XEN_ENABLED)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  if (LSM303AGR_ACC_W_YEN((void *)lsm303agr_i2c, LSM303AGR_ACC_YEN_ENABLED)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  if (LSM303AGR_ACC_W_ZEN((void *)lsm303agr_i2c, LSM303AGR_ACC_ZEN_ENABLED)  == MEMS_ERROR) {
    return MEMS_ERROR;
  }

  /* Select default output data rate. */
  acc_current_odr = LSM303AGR_ACC_ODR_DO_PWR_DOWN;
  acc_current_fs = 2;
  acc_is_enabled = 0;

  lsm303agr_acc_is_initialized = true;

  return MEMS_SUCCESS;
}

mems_status_t lsm303agr_acc_set_x_odr(uint8_t odr) {
  acc_current_odr = (odr <= 1) ? LSM303AGR_ACC_ODR_DO_1Hz
                                  : (odr <= 10) ? LSM303AGR_ACC_ODR_DO_10Hz
                                                   : (odr <= 25) ? LSM303AGR_ACC_ODR_DO_25Hz
                                                                    : (odr <= 50) ? LSM303AGR_ACC_ODR_DO_50Hz
                                                                                     : (odr <= 100) ? LSM303AGR_ACC_ODR_DO_100Hz
                                                                                                       : (odr <= 200) ? LSM303AGR_ACC_ODR_DO_200Hz
                                                                                                                         : LSM303AGR_ACC_ODR_DO_400Hz;

  return LSM303AGR_ACC_W_ODR((void *)lsm303agr_i2c, acc_current_odr);
}

mems_status_t lsm303agr_acc_set_low_power_mode(bool lp_mode, bool hr_mode) {
  if (lp_mode)
    lp = LSM303AGR_ACC_LPEN_ENABLED;
  else
    lp = LSM303AGR_ACC_LPEN_DISABLED;

  if (hr_mode)
    hr = LSM303AGR_ACC_HR_ENABLED;
  else
    hr = LSM303AGR_ACC_HR_DISABLED;


  if (lp == LSM303AGR_ACC_LPEN_ENABLED && hr == LSM303AGR_ACC_HR_DISABLED) {
    /* op mode is LP 8-bit */
    op_mode = 2;
    mode_shift = 8;
  } else if (lp == LSM303AGR_ACC_LPEN_DISABLED && hr == LSM303AGR_ACC_HR_DISABLED) {
    /* op mode is Normal 10-bit */
    op_mode = 1;
    mode_shift = 6;
  } else if (lp == LSM303AGR_ACC_LPEN_DISABLED && hr == LSM303AGR_ACC_HR_ENABLED) {
    /* op mode is HR 12-bit */
    op_mode = 0;
    mode_shift = 4;
  }

    mems_status_t ret =  LSM303AGR_ACC_W_LOWPWR_EN((void *)lsm303agr_i2c, lp);
    ret = LSM303AGR_ACC_W_HiRes((void *)lsm303agr_i2c,  hr);
    return ret;
}

mems_status_t lsm303agr_acc_set_x_fs(uint8_t fullScale)
{
  acc_current_fs = ( fullScale <= 2) ? LSM303AGR_ACC_FS_2G
         : ( fullScale <= 4 ) ? LSM303AGR_ACC_FS_4G
         : ( fullScale <= 8 ) ? LSM303AGR_ACC_FS_8G
         :                         LSM303AGR_ACC_FS_16G;

 switch (acc_current_fs) {
  case LSM303AGR_ACC_FS_2G:
    fs_mode = 0;
    break;

  case LSM303AGR_ACC_FS_4G:
    fs_mode = 1;
    break;

  case LSM303AGR_ACC_FS_8G:
    fs_mode = 2;
    break;

  case LSM303AGR_ACC_FS_16G:
    fs_mode = 3;
    break;
  }

  return  LSM303AGR_ACC_W_FullScale( (void *)lsm303agr_i2c, acc_current_fs );
}

mems_status_t lsm303agr_acc_get_x_axes_raw(int16_t *pData)
{
  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};

  /* Read output registers from LSM303AGR_ACC_GYRO_OUTX_L_XL to LSM303AGR_ACC_GYRO_OUTZ_H_XL. */
  if (LSM303AGR_ACC_Get_Raw_Acceleration( (void *)lsm303agr_i2c, ( uint8_t* )regValue )  == MEMS_ERROR)
  {
    return MEMS_ERROR;
  }

  /* Format the data. */
  pData[0] = ( ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] ) >> mode_shift );
  pData[1] = ( ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] ) >> mode_shift );
  pData[2] = ( ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] ) >> mode_shift );

  return MEMS_SUCCESS;
}

/*
 * Values returned are espressed in mg.
 */
mems_status_t lsm303agr_acc_get_acceleration(int32_t *buff)
{
  Type3Axis16bit_U raw_data_tmp; 
  /* Read out raw accelerometer samples */
  if(!LSM303AGR_ACC_Get_Raw_Acceleration((void *)lsm303agr_i2c, raw_data_tmp.u8bit)) {
    return MEMS_ERROR;
  }

  /* Apply proper shift and sensitivity */
  buff[0] = ((raw_data_tmp.i16bit[0] >> mode_shift) * LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
  buff[1] = ((raw_data_tmp.i16bit[1] >> mode_shift) * LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
  buff[2] = ((raw_data_tmp.i16bit[2] >> mode_shift) * LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;

  return MEMS_SUCCESS;
}