/**
 ******************************************************************************
 * @file    LSM303AGRMagSensor.cpp
 * @author  CLab
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Implementation an LSM303AGR magnetometer sensor.
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

#include "LSM303AGRMagSensor.h"


/* Class Implementation ------------------------------------------------------*/
LSM303AGRMagSensor::LSM303AGRMagSensor(SPI *spi, PinName cs_pin, PinName intmag_pin) :
                                      _dev_spi(spi), _cs_pin(cs_pin), _intmag_pin(intmag_pin) // SPI3W ONLY
{
    assert (spi);
    if (cs_pin == NC) 
    {
        printf ("ERROR LSM303AGRMagSensor CS MUST NOT BE NC\n\r");        
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
LSM303AGRMagSensor::LSM303AGRMagSensor(DevI2C *i2c, uint8_t address, PinName intmag_pin) : 
                                       _dev_i2c(i2c), _address(address), _cs_pin(NC), _intmag_pin(intmag_pin)
{
    assert (i2c);
    _dev_spi = NULL;      
}

/**
 * @brief     Initializing the component.
 * @param[in] init pointer to device specific initalization structure.
 * @retval    "0" in case of success, an error code otherwise.
 */
int LSM303AGRMagSensor::init(void *init)
{
  /* Operating mode selection - power down */
  if ( LSM303AGR_MAG_W_MD( (void *)this, LSM303AGR_MAG_MD_IDLE1_MODE ) == MEMS_ERROR )
  {
    return 1;
  }
  
  /* Enable BDU */
  if ( LSM303AGR_MAG_W_BDU( (void *)this, LSM303AGR_MAG_BDU_ENABLED ) == MEMS_ERROR )
  {
    return 1;
  }
  
  if ( set_m_odr( 100.0f ) == 1 )
  {
    return 1;
  }
  
  if ( set_m_fs( 50.0f ) == 1 )
  {
    return 1;
  }

  if ( LSM303AGR_MAG_W_ST( (void *)this, LSM303AGR_MAG_ST_DISABLED ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief  Enable LSM303AGR magnetometer
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::enable(void)
{
  /* Operating mode selection */
  if ( LSM303AGR_MAG_W_MD( (void *)this, LSM303AGR_MAG_MD_CONTINUOS_MODE ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief  Disable LSM303AGR magnetometer
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::disable(void)
{
  /* Operating mode selection - power down */
  if ( LSM303AGR_MAG_W_MD( (void *)this, LSM303AGR_MAG_MD_IDLE1_MODE ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief  Read ID of LSM303AGR Magnetometer
 * @param  p_id the pointer where the ID of the device is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::read_id(uint8_t *id)
{
  if(!id)
  { 
    return 1; 
  }
 
  /* Read WHO AM I register */
  if ( LSM303AGR_MAG_R_WHO_AM_I( (void *)this, id ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}

/**
 * @brief  Read data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetometer data are stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::get_m_axes(int32_t *pData)
{
  int16_t pDataRaw[3];
  float sensitivity = 0;
  
  /* Read raw data from LSM303AGR output register. */
  if ( get_m_axes_raw( pDataRaw ) == 1 )
  {
    return 1;
  }
  
  /* Get LSM303AGR actual sensitivity. */
  if ( get_m_sensitivity( &sensitivity ) == 1 )
  {
    return 1;
  }
  
  /* Calculate the data. */
  pData[0] = ( int32_t )( pDataRaw[0] * sensitivity );
  pData[1] = ( int32_t )( pDataRaw[1] * sensitivity );
  pData[2] = ( int32_t )( pDataRaw[2] * sensitivity );
  
  return 0;
}

/**
 * @brief  Read Magnetometer Sensitivity
 * @param  pfData the pointer where the magnetometer sensitivity is stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::get_m_sensitivity(float *pfData)
{
  *pfData = 1.5f;
  
  return 0;
}

/**
 * @brief  Read raw data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::get_m_axes_raw(int16_t *pData)
{
  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};
  int16_t *regValueInt16;
  
  /* Read output registers from LSM303AGR_MAG_OUTX_L to LSM303AGR_MAG_OUTZ_H. */
  if ( LSM303AGR_MAG_Get_Raw_Magnetic( (void *)this, regValue ) == MEMS_ERROR )
  {
    return 1;
  }
  
  regValueInt16 = (int16_t *)regValue;
  
  /* Format the data. */
  pData[0] = regValueInt16[0];
  pData[1] = regValueInt16[1];
  pData[2] = regValueInt16[2];
  
  return 0;
}

/**
 * @brief  Read LSM303AGR Magnetometer output data rate
 * @param  odr the pointer to the output data rate
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::get_m_odr(float* odr)
{
  LSM303AGR_MAG_ODR_t odr_low_level;
  
  if ( LSM303AGR_MAG_R_ODR( (void *)this, &odr_low_level ) == MEMS_ERROR )
  {
    return 1;
  }
  
  switch( odr_low_level )
  {
    case LSM303AGR_MAG_ODR_10Hz:
      *odr = 10.000f;
      break;
    case LSM303AGR_MAG_ODR_20Hz:
      *odr = 20.000f;
      break;
    case LSM303AGR_MAG_ODR_50Hz:
      *odr = 50.000f;
      break;
    case LSM303AGR_MAG_ODR_100Hz:
      *odr = 100.000f;
      break;
    default:
      *odr = -1.000f;
      return 1;
  }  
  return 0;
}

/**
 * @brief  Set ODR
 * @param  odr the output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::set_m_odr(float odr)
{
  LSM303AGR_MAG_ODR_t new_odr;
  
  new_odr = ( odr <= 10.000f ) ? LSM303AGR_MAG_ODR_10Hz
          : ( odr <= 20.000f ) ? LSM303AGR_MAG_ODR_20Hz
          : ( odr <= 50.000f ) ? LSM303AGR_MAG_ODR_50Hz
          :                      LSM303AGR_MAG_ODR_100Hz;
            
  if ( LSM303AGR_MAG_W_ODR( (void *)this, new_odr ) == MEMS_ERROR )
  {
    return 1;
  }
  
  return 0;
}


/**
 * @brief  Read LSM303AGR Magnetometer full scale
 * @param  fullScale the pointer to the output data rate
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::get_m_fs(float* fullScale)
{
  *fullScale = 50.0f;
  
  return 0;
}

/**
 * @brief  Set full scale
 * @param  fullScale the full scale to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM303AGRMagSensor::set_m_fs(float fullScale)
{
  return 0;
}


/**
 * @brief Read magnetometer data from register
 * @param reg register address
 * @param data register data
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int LSM303AGRMagSensor::read_reg( uint8_t reg, uint8_t *data )
{
  if ( LSM303AGR_MAG_read_reg( (void *)this, reg, data ) == MEMS_ERROR )
  {
    return 1;
  }

  return 0;
}


/**
 * @brief Write magnetometer data to register
 * @param reg register address
 * @param data register data
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int LSM303AGRMagSensor::write_reg( uint8_t reg, uint8_t data )
{
  if ( LSM303AGR_MAG_write_reg( (void *)this, reg, data ) == MEMS_ERROR )
  {
    return 1;
  }

  return 0;
}

uint8_t LSM303AGR_MAG_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite )
{
  return ((LSM303AGRMagSensor *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

uint8_t LSM303AGR_MAG_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead )
{
  return ((LSM303AGRMagSensor *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}
