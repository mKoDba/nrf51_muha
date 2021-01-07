/***********************************************************************************************//**
 * Copyright 2021 Mario Kodba
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ***************************************************************************************************
 * @file    bsp_ecg_ADS1192.h
 * @author  mario.kodba
 * @brief   ADS1192 ECG device functionality header file.
 **************************************************************************************************/

#ifndef BSP_ECG_ADS1192_H_
#define BSP_ECG_ADS1192_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "drv_spi.h"
#include "nrf_drv_gpiote.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define DEBUG false                             //!< DEBUG enable macro

/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/
//! ADS1192 error enumeration
typedef enum BSP_ECG_ADS1192_err_ENUM {
    BSP_ECG_ADS1192_err_NONE            = 0u,   //!< No error
    BSP_ECG_ADS1192_err_NULL_PARAM,             //!< NULL parameter error
    BSP_ECG_ADS1192_err_INIT,                   //!< Error on initialization
    BSP_ECG_ADS1192_err_SPI_READ_WRITE,         //!< SPI operation error
    BSP_ECG_ADS1192_err_LEAD_OFF,               //!< Channel 1 and 2 Lead-off error
    BSP_ECG_ADS1192_err_RLD_OFF                 //!< RLD off error
} BSP_ECG_ADS1192_err_E;

//! ADS1192 register enumeration
typedef enum BSP_ECG_ADS1192_reg_ENUM {
    BSP_ECG_ADS1192_reg_ID        = 0u,         //!< ADS1192 ID register
    BSP_ECG_ADS1192_reg_CONFIG_1  = 1u,         //!< ADS1192 CONFIG 1 register
    BSP_ECG_ADS1192_reg_CONFIG_2  = 2u,         //!< ADS1192 CONFIG 2 register
    BSP_ECG_ADS1192_reg_LOFF      = 3u,         //!< ADS1192 LOFF register
    BSP_ECG_ADS1192_reg_CH1_SET   = 4u,         //!< ADS1192 CH1 register
    BSP_ECG_ADS1192_reg_CH2_SET   = 5u,         //!< ADS1192 CH2 register
    BSP_ECG_ADS1192_reg_RLD_SENS  = 6u,         //!< ADS1192 RLD_SENS register
    BSP_ECG_ADS1192_reg_LOFF_SENS = 7u,         //!< ADS1192 LOFF_SENS register
    BSP_ECG_ADS1192_reg_LOFF_STAT = 8u,         //!< ADS1192 LOFF_STAT register
    BSP_ECG_ADS1192_reg_MISC_1    = 9u,         //!< ADS1192 MISC 1 register
    BSP_ECG_ADS1192_reg_MISC_2    = 10u,        //!< ADS1192 MISC 2 register
    BSP_ECG_ADS1192_reg_GPIO      = 11u,        //!< ADS1192 GPIO register

    BSP_ECG_ADS1192_reg_COUNT
} BSP_ECG_ADS1192_reg_E;

//! ADS1192 conversion rate enumeration
typedef enum BSP_ECG_ADS1192_convRate_ENUM {
    BSP_ECG_ADS1192_convRate_125_SPS  = 0u,     //!< 125 Samples per second conversion
    BSP_ECG_ADS1192_convRate_250_SPS  = 1u,     //!< 250 Samples per second conversion
    BSP_ECG_ADS1192_convRate_500_SPS  = 2u,     //!< 500 Samples per second conversion
    BSP_ECG_ADS1192_convRate_1000_SPS = 3u,     //!< 1000 Samples per second conversion
    BSP_ECG_ADS1192_convRate_2000_SPS = 4u,     //!< 2000 Samples per second conversion
    BSP_ECG_ADS1192_convRate_4000_SPS = 5u,     //!< 4000 Samples per second conversion
    BSP_ECG_ADS1192_convRate_8000_SPS = 6u      //!< 8000 Samples per second conversion
} BSP_ECG_ADS1192_convRate_E;

//! ADS1192 PGA enumeration
typedef enum BSP_ECG_ADS1192_pga_ENUM {
    BSP_ECG_ADS1192_pga_6X  = 0u,               //!< PGA gain setting for 6x
    BSP_ECG_ADS1192_pga_1X  = 1u,               //!< PGA gain setting for 1x
    BSP_ECG_ADS1192_pga_2X  = 2u,               //!< PGA gain setting for 2x
    BSP_ECG_ADS1192_pga_3X  = 3u,               //!< PGA gain setting for 3x
    BSP_ECG_ADS1192_pga_4X  = 4u,               //!< PGA gain setting for 4x
    BSP_ECG_ADS1192_pga_8X  = 5u,               //!< PGA gain setting for 8x
    BSP_ECG_ADS1192_pga_12X = 6u                //!< PGA gain setting for 12x
} BSP_ECG_ADS1192_pga_E;

//! ADS1192 MUX enumeration
typedef enum BSP_ECG_ADS1192_mux_ENUM {
    BSP_ECG_ADS1192_mux_ELECTRODE_IN = 0u,      //!< Normal electrode input
    BSP_ECG_ADS1192_pga_SHORTED_IN   = 1u,      //!< Input shorted (offset measurements)
    BSP_ECG_ADS1192_mux_RLD_MEAS     = 2u,      //!< Right-Leg drive measurement
    BSP_ECG_ADS1192_mux_SUPPLY_MEAS  = 3u,      //!< Supply measurement
    BSP_ECG_ADS1192_mux_TEMP_SENS    = 4u,      //!< Temperature measurement
    BSP_ECG_ADS1192_mux_TEST_SIGNAL  = 5u,      //!< Test signal
    BSP_ECG_ADS1192_mux_RLD_DRP      = 6u,      //!< RLD_DRP
    BSP_ECG_ADS1192_mux_RLD_DRM      = 7u,      //!< RLD_DRM
    BSP_ECG_ADS1192_mux_RLD_DRPM_CH1 = 8u       //!< RLD_DRPM (for channel 2)
} BSP_ECG_ADS1192_mux_E;

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! Configuration Register 1 - configures both ADC channels sample rate.
typedef union BSP_ECG_ADS1192_config1Reg_UNION {
    uint8_t R;                                  //!< CONFIG 1 register value
    struct {
       uint8_t  dr          : 3;                /*!< DR[2:0] bits determine both channel 1
                                                     and channel 2 over-sampling ratio */
       uint8_t              : 4;                //!< Reserved - must be set to 0
       uint8_t  singleShot  : 1;                /*!< Sets conversion mode
                                                     [ 0 -> Continuous mode (default) ]
                                                     [ 1 -> Single-shot mode ] */
    } B;                                        //!< CONFIG 1 register bits
} BSP_ECG_ADS1192_config1Reg_U;

//! Configuration Register 2 - configures test signal, clock, reference, LOFF buffer.
typedef union BSP_ECG_ADS1192_config2Reg_UNION {
    uint8_t R;                                  //!< CONFIG 2 register value
    struct {
       uint8_t  testFreq    : 1;                //!< Test signal frequency
       uint8_t  intTest     : 1;                //!< Test signal selection
       uint8_t              : 1;                //!< Reserved - must be set to 0
       uint8_t  clkEn       : 1;                //!< CLK connection
       uint8_t  vRef4V      : 1;                //!< Enables 4V reference
       uint8_t  pdbRefBuf   : 1;                //!< Reference buffer power-down
       uint8_t  pdbLoffComp : 1;                //!< Lead-off comparator power-down
       uint8_t              : 1;                //!< Reserved - must be set to 1
    } B;                                        //!< CONFIG 2 register bits
} BSP_ECG_ADS1192_config2Reg_U;

//! Channel 1 and 2 settings - configures power mode, PGA, MUX settings.
typedef union BSP_ECG_ADS1192_chXsetReg_UNION {
    uint8_t R;                                  //!< Channel X register value
    struct {
       uint8_t  mux         : 4;                //!< Channel X input selection
       uint8_t  pga         : 3;                //!< Channel X PGA gain setting
       uint8_t  powerDown   : 1;                //!< Channel X power-down
    } B;                                        //!< Channel X register bits
} BSP_ECG_ADS1192_chXsetReg_U;

//! ECG ADS1192 driver configuration structure
typedef struct BSP_ECG_ADS1192_config_STRUCT {
    DRV_SPI_instance_S *spiInstance;            //!< SPI master driver instance structure
    DRV_SPI_config_S *spiConfig;                //!< SPI master driver instance configuration

    BSP_ECG_ADS1192_convRate_E samplingRate;    //!< Signal sampling rate
    BSP_ECG_ADS1192_pga_E      pgaSetting;      //!< PGA setting for normal electrode reading
} BSP_ECG_ADS1192_config_S;

//! ECG ADS1192 driver device structure
typedef struct BSP_ECG_ADS1192_device_STRUCT {
    BSP_ECG_ADS1192_config_S    *config;        //!< Pointer to ECG driver configuration
#if (DEBUG == true)
    int16_t temperature;                        //!< Temperature of device
    float digitalVddSupply;                     //!< Digital VDD supply
    float analogVddSupply;                      //!< Analog VDD supply
#endif // #if (DEBUG == true)
    bool isInitialized;                         //!< Is device initialized
} BSP_ECG_ADS1192_device_S;
/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/


/***************************************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void BSP_ECG_ADS1192_init(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_config_S *inConfig,
        BSP_ECG_ADS1192_err_E *outErr);
void BSP_ECG_ADS1192_startEcgReading(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
void BSP_ECG_ADS1192_stopEcgReading(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
void BSP_ECG_ADS1192_DrdyPin_IRQHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif // #ifndef BSP_ECG_ADS1192_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
