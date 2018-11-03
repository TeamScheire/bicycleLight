/**
 ******************************************************************************
 * @file    LSM303AGRAccSensor.cpp
 * @author  CLab
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Implementation an LSM303AGR accelerometer sensor.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/

#include "LSM303AGRAccSensor.h"


/* Class Implementation ------------------------------------------------------*/

LSM303AGRAccSensor::LSM303AGRAccSensor(SPI *spi, PinName cs_pin, PinName int1_pin, PinName int2_pin) :
                                       _dev_spi(spi), _cs_pin(cs_pin), _int1_pin(int1_pin), _int2_pin(int2_pin)  // SPI3W ONLY
{
    assert (spi);
    if (cs_pin == NC) 
    {
        printf ("ERROR LSM303AGRAccSensor CS MUST NOT BE NC\n\r");      
        _dev_spi = NULL;
        _dev_i2c=NULL;
        return;
    }       
    _cs_pin = 0;     // enable SPI3W disable I2C
    _dev_i2c=NULL;    

  LSM303AGR_ACC_W_SPI_mode((void *)this, LSM303AGR_ACC_SIM_3_WIRES);  
}


/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 */
LSM303AGRAccSensor::LSM303AGRAccSensor(DevI2C *i2c, uint8_t address, PinName int1_pin, PinName int2_pin) : 
                                       _dev_i2c(i2c), _address(address), _cs_pin(NC), _int1_pin(int1_pin), _int2_pin(int2_pin)
{
    assert (i2c);
    _dev_spi = NULL;
};

/**
 * @brief     Initializing the component.
 * @param[in] init pointer to device specific initalization structure.
 * @retval    "0" in case of success, an error code otherwise.
 */
int LSM303AGRAccSensor::init(void *init)
{
  /* Enable BDU */
  if ( LSM303AGR_ACC_W_BlockDataUpdate( (void *)this, LSM303AGR_ACC_BDU_ENABLED ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* FIFO mode selection */
  if ( LSM303AGR_ACC_W_FifoMode( (void *)this, LSM303AGR_ACC_FM_BYPASS ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Output data rate selection - power down. */
  if ( LSM303AGR_ACC_W_ODR( (void *)this, LSM303AGR_ACC_ODR_DO_PWR_DOWN ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Full scale selection. */
  if ( set_x_fs( 2.0f ) == 1 )
  {
    return 1;
  }
  
  /* Enable axes. */
  if ( LSM303AGR_ACC_W_XEN( (void *)this, LSM303AGR_ACC_XEN_ENABLED ) == MEMS_ERROR )
  {
    return 1;
  }
  
  if ( LSM303AGR_ACC_W_YEN ( (void *)this, LSM303AGR_ACC_YEN_ENABLED ) == MEMS_ERROR )
  {
    return 1;
  }
  
  if ( LSM303AGR_ACC_W_ZEN ( (void *)this, LSM303AGR_ACC_ZEN_ENABLED ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Select default output data rate. */
  _last_odr = 100.0f;
  
  _is_enabled = 0;
  
  return 0;
}

/**
 * @brief  Enable LSM303AGR Accelerator
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::enable(void)
{ 
  /* Check if the component is already enabled */
  if ( _is_enabled == 1 )
  {
    return 0;
  }
  
  /* Output data rate selection. */
  if ( set_x_odr_when_enabled( _last_odr ) == 1 )
  {
    return 1;
  }
  
  _is_enabled = 1;
  
  return 0;
}

/**
 * @brief  Disable LSM303AGR Accelerator
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::disable(void)
{ 
  /* Check if the component is already disabled */
  if ( _is_enabled == 0 )
  {
    return 0;
  }
  
  /* Store actual output data rate. */
  if ( get_x_odr( &_last_odr ) == 1 )
  {
    return 1;
  }
  
  /* Output data rate selection - power down. */
  if ( LSM303AGR_ACC_W_ODR( (void *)this, LSM303AGR_ACC_ODR_DO_PWR_DOWN ) == MEMS_ERROR )
  {
    return 1;
  }
  
  _is_enabled = 0;
  
  return 0;
}

/**
 * @brief  Read ID of LSM303AGR Accelerometer
 * @param  p_id the pointer where the ID of the device is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::read_id(uint8_t *id)
{
  if(!id)
  { 
    return 1; 
  }
 
  /* Read WHO AM I register */
  if ( LSM303AGR_ACC_R_WHO_AM_I( (void *)this, id ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief  Read data from LSM303AGR Accelerometer
 * @param  pData the pointer where the accelerometer data are stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_axes(int32_t *pData)
{
  int data[3];
  
  /* Read data from LSM303AGR. */
  if ( !LSM303AGR_ACC_Get_Acceleration((void *)this, data) )
  {
    return 1;
  }
  
  /* Calculate the data. */
  pData[0] = (int32_t)data[0];
  pData[1] = (int32_t)data[1];
  pData[2] = (int32_t)data[2];
  
  return 0;
}

/**
 * @brief  Read Accelerometer Sensitivity
 * @param  pfData the pointer where the accelerometer sensitivity is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_sensitivity(float *pfData)
{
  LSM303AGR_ACC_LPEN_t lp_value;
  LSM303AGR_ACC_HR_t hr_value;
  
  /* Read low power flag */
  if( LSM303AGR_ACC_R_LOWPWR_EN( (void *)this, &lp_value ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Read high performance flag */
  if( LSM303AGR_ACC_R_HiRes( (void *)this, &hr_value ) == MEMS_ERROR )
  {
    return 1;
  }
  
  if( lp_value == LSM303AGR_ACC_LPEN_DISABLED && hr_value == LSM303AGR_ACC_HR_DISABLED )
  {
    /* Normal Mode */
    return get_x_sensitivity_normal_mode( pfData );
  } else if ( lp_value == LSM303AGR_ACC_LPEN_ENABLED && hr_value == LSM303AGR_ACC_HR_DISABLED )
  {
    /* Low Power Mode */
    return get_x_sensitivity_lp_mode( pfData );
  } else if ( lp_value == LSM303AGR_ACC_LPEN_DISABLED && hr_value == LSM303AGR_ACC_HR_ENABLED )
  {
    /* High Resolution Mode */
    return get_x_sensitivity_hr_mode( pfData );
  } else
  {
    /* Not allowed */
    return 1;
  }
}

/**
 * @brief  Read Accelerometer Sensitivity in Normal Mode
 * @param  sensitivity the pointer where the accelerometer sensitivity is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_sensitivity_normal_mode( float *sensitivity )
{
  LSM303AGR_ACC_FS_t fullScale;
  
  /* Read actual full scale selection from sensor. */
  if ( LSM303AGR_ACC_R_FullScale( (void *)this, &fullScale ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Store the sensitivity based on actual full scale. */
  switch( fullScale )
  {
    case LSM303AGR_ACC_FS_2G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_2G_NORMAL_MODE;
      break;
    case LSM303AGR_ACC_FS_4G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_4G_NORMAL_MODE;
      break;
    case LSM303AGR_ACC_FS_8G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_8G_NORMAL_MODE;
      break;
    case LSM303AGR_ACC_FS_16G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_16G_NORMAL_MODE;
      break;
    default:
      *sensitivity = -1.0f;
      return 1;
  }
  
  return 0;
}

/**
 * @brief  Read Accelerometer Sensitivity in LP Mode
 * @param  sensitivity the pointer where the accelerometer sensitivity is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_sensitivity_lp_mode( float *sensitivity )
{
  LSM303AGR_ACC_FS_t fullScale;
  
  /* Read actual full scale selection from sensor. */
  if ( LSM303AGR_ACC_R_FullScale( (void *)this, &fullScale ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Store the sensitivity based on actual full scale. */
  switch( fullScale )
  {
    case LSM303AGR_ACC_FS_2G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_2G_LOW_POWER_MODE;
      break;
    case LSM303AGR_ACC_FS_4G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_4G_LOW_POWER_MODE;
      break;
    case LSM303AGR_ACC_FS_8G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_8G_LOW_POWER_MODE;
      break;
    case LSM303AGR_ACC_FS_16G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_16G_LOW_POWER_MODE;
      break;
    default:
      *sensitivity = -1.0f;
      return 1;
  }
  
  return 0;
}

/**
 * @brief  Read Accelerometer Sensitivity in HR Mode
 * @param  sensitivity the pointer where the accelerometer sensitivity is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_sensitivity_hr_mode( float *sensitivity )
{
  LSM303AGR_ACC_FS_t fullScale;
  
  /* Read actual full scale selection from sensor. */
  if ( LSM303AGR_ACC_R_FullScale( (void *)this, &fullScale ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Store the sensitivity based on actual full scale. */
  switch( fullScale )
  {
    case LSM303AGR_ACC_FS_2G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_2G_HIGH_RESOLUTION_MODE;
      break;
    case LSM303AGR_ACC_FS_4G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_4G_HIGH_RESOLUTION_MODE;
      break;
    case LSM303AGR_ACC_FS_8G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_8G_HIGH_RESOLUTION_MODE;
      break;
    case LSM303AGR_ACC_FS_16G:
      *sensitivity = ( float )LSM303AGR_ACC_SENSITIVITY_FOR_FS_16G_HIGH_RESOLUTION_MODE;
      break;
    default:
      *sensitivity = -1.0f;
      return 1;
  }
  
  return 0;
}

/**
 * @brief  Read raw data from LSM303AGR Accelerometer
 * @param  pData the pointer where the accelerometer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_axes_raw(int16_t *pData)
{
  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};
  u8_t shift = 0;
  LSM303AGR_ACC_LPEN_t lp;
  LSM303AGR_ACC_HR_t hr;
  
  /* Determine which operational mode the acc is set */
  if(!LSM303AGR_ACC_R_HiRes( (void *)this, &hr )) {
    return 1;
  }

  if(!LSM303AGR_ACC_R_LOWPWR_EN( (void *)this, &lp )) {
    return 1;
  }
  
  if (lp == LSM303AGR_ACC_LPEN_ENABLED && hr == LSM303AGR_ACC_HR_DISABLED) {
    /* op mode is LP 8-bit */
    shift = 8;
  } else if (lp == LSM303AGR_ACC_LPEN_DISABLED && hr == LSM303AGR_ACC_HR_DISABLED) {
    /* op mode is Normal 10-bit */
    shift = 6;
  } else if (lp == LSM303AGR_ACC_LPEN_DISABLED && hr == LSM303AGR_ACC_HR_ENABLED) {
    /* op mode is HR 12-bit */
    shift = 4;
  } else {
    return 1;
  }
  
  /* Read output registers from LSM303AGR_ACC_GYRO_OUTX_L_XL to LSM303AGR_ACC_GYRO_OUTZ_H_XL. */
  if (!LSM303AGR_ACC_Get_Raw_Acceleration( (void *)this, ( uint8_t* )regValue ))
  {
    return 1;
  }
  
  /* Format the data. */
  pData[0] = ( ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] ) >> shift );
  pData[1] = ( ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] ) >> shift );
  pData[2] = ( ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] ) >> shift );
  
  return 0;
}

/**
 * @brief  Read LSM303AGR Accelerometer output data rate
 * @param  odr the pointer to the output data rate
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_odr(float* odr)
{
  LSM303AGR_ACC_ODR_t odr_low_level;
  
  if ( LSM303AGR_ACC_R_ODR( (void *)this, &odr_low_level ) == MEMS_ERROR )
  {
    return 1;
  }
  
  switch( odr_low_level )
  {
    case LSM303AGR_ACC_ODR_DO_PWR_DOWN:
      *odr = 0.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_1Hz:
      *odr = 1.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_10Hz:
      *odr = 10.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_25Hz:
      *odr = 25.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_50Hz:
      *odr = 50.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_100Hz:
      *odr = 100.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_200Hz:
      *odr = 200.0f;
      break;
    case LSM303AGR_ACC_ODR_DO_400Hz:
      *odr = 400.0f;
      break;
    default:
      *odr = -1.0f;
      return 1;
  }
  
  return 0;
}

/**
 * @brief  Set ODR
 * @param  odr the output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::set_x_odr(float odr)
{
  if(_is_enabled == 1)
  {
    if(set_x_odr_when_enabled(odr) == 1)
    {
      return 1;
    }
  }
  else
  {
    if(set_x_odr_when_disabled(odr) == 1)
    {
      return 1;
    }
  }
  
  return 0;
}

/**
 * @brief  Set ODR when enabled
 * @param  odr the output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::set_x_odr_when_enabled(float odr)
{
  LSM303AGR_ACC_ODR_t new_odr;
  
  new_odr = ( odr <=    1.0f ) ? LSM303AGR_ACC_ODR_DO_1Hz
          : ( odr <=   10.0f ) ? LSM303AGR_ACC_ODR_DO_10Hz
          : ( odr <=   25.0f ) ? LSM303AGR_ACC_ODR_DO_25Hz
          : ( odr <=   50.0f ) ? LSM303AGR_ACC_ODR_DO_50Hz
          : ( odr <=  100.0f ) ? LSM303AGR_ACC_ODR_DO_100Hz
          : ( odr <=  200.0f ) ? LSM303AGR_ACC_ODR_DO_200Hz
          :                      LSM303AGR_ACC_ODR_DO_400Hz;
            
  if ( LSM303AGR_ACC_W_ODR( (void *)this, new_odr ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief  Set ODR when disabled
 * @param  odr the output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::set_x_odr_when_disabled(float odr)
{ 
  _last_odr = ( odr <=    1.0f ) ?  1.0f
           : ( odr <=   10.0f ) ? 10.0f
           : ( odr <=   25.0f ) ? 25.0f
           : ( odr <=   50.0f ) ? 50.0f
           : ( odr <=  100.0f ) ? 100.0f
           : ( odr <=  200.0f ) ? 200.0f
           :                      400.0f;
                                 
  return 0;
}


/**
 * @brief  Read LSM303AGR Accelerometer full scale
 * @param  fullScale the pointer to the full scale
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::get_x_fs(float* fullScale)
{
  LSM303AGR_ACC_FS_t fs_low_level;
  
  if ( LSM303AGR_ACC_R_FullScale( (void *)this, &fs_low_level ) == MEMS_ERROR )
  {
    return 1;
  }
  
  switch( fs_low_level )
  {
    case LSM303AGR_ACC_FS_2G:
      *fullScale =  2.0f;
      break;
    case LSM303AGR_ACC_FS_4G:
      *fullScale =  4.0f;
      break;
    case LSM303AGR_ACC_FS_8G:
      *fullScale =  8.0f;
      break;
    case LSM303AGR_ACC_FS_16G:
      *fullScale = 16.0f;
      break;
    default:
      *fullScale = -1.0f;
      return 1;
  }
  
  return 0;
}

/**
 * @brief  Set full scale
 * @param  fullScale the full scale to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRAccSensor::set_x_fs(float fullScale)
{
  LSM303AGR_ACC_FS_t new_fs;
  
  new_fs = ( fullScale <= 2.0f ) ? LSM303AGR_ACC_FS_2G
         : ( fullScale <= 4.0f ) ? LSM303AGR_ACC_FS_4G
         : ( fullScale <= 8.0f ) ? LSM303AGR_ACC_FS_8G
         :                         LSM303AGR_ACC_FS_16G;
           
  if ( LSM303AGR_ACC_W_FullScale( (void *)this, new_fs ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief Read accelerometer data from register
 * @param reg register address
 * @param data register data
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int LSM303AGRAccSensor::read_reg( uint8_t reg, uint8_t *data )
{

  if ( LSM303AGR_ACC_read_reg( (void *)this, reg, data ) == MEMS_ERROR )
  {
    return 1;
  }

  return 0;
}

/**
 * @brief Write accelerometer data to register
 * @param reg register address
 * @param data register data
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int LSM303AGRAccSensor::write_reg( uint8_t reg, uint8_t data )
{

  if ( LSM303AGR_ACC_write_reg( (void *)this, reg, data ) == MEMS_ERROR )
  {
    return 1;
  }

  return 0;
}

uint8_t LSM303AGR_ACC_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite )
{
  return ((LSM303AGRAccSensor *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

uint8_t LSM303AGR_ACC_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead )
{
  return ((LSM303AGRAccSensor *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}
