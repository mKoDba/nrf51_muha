/*
 * bsp_ecg_ADS1192.h
 *
 *  Created on: 21. lis 2020.
 *      Author: Mario
 */

#ifndef BSP_ECG_ADS1192_H_
#define BSP_ECG_ADS1192_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "nrf_drv_spi.h"
/*******************************************************************************
 *                              ENUMERATIONS
 ******************************************************************************/
typedef enum BSP_ECG_ADS1192_err_ENUM {
    BSP_ECG_ADS1192_err_NONE            = 0u,
    BSP_ECG_ADS1192_err_NULL_PARAM,
    BSP_ECG_ADS1192_err_INIT,
    BSP_ECG_ADS1192_err_SPI_READ_WRITE,
    BSP_ECG_ADS1192_err_RLD_OFF
} BSP_ECG_ADS1192_err_E;

typedef enum BSP_ECG_ADS1192_reg_ENUM {
    BSP_ECG_ADS1192_reg_ID          = 0u,
    BSP_ECG_ADS1192_reg_CONFIG_1    = 1u,
    BSP_ECG_ADS1192_reg_CONFIG_2    = 2u,
    BSP_ECG_ADS1192_reg_LOFF        = 3u,
    BSP_ECG_ADS1192_reg_CH1_SET     = 4u,
    BSP_ECG_ADS1192_reg_CH2_SET     = 5u,
    BSP_ECG_ADS1192_reg_RLD_SENS    = 6u,
    BSP_ECG_ADS1192_reg_LOFF_SENS   = 7u,
    BSP_ECG_ADS1192_reg_LOFF_STAT   = 8u,
    BSP_ECG_ADS1192_reg_MISC_1      = 9u,
    BSP_ECG_ADS1192_reg_MISC_2      =10u,
    BSP_ECG_ADS1192_reg_GPIO        =11u,

    BSP_ECG_ADS1192_reg_COUNT
} BSP_ECG_ADS1192_reg_E;

/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/
//!< Configuration Register 1 - configures both ADC channels sample rate.
typedef union BSP_ECG_ADS1192_config1Reg_UNION {
    uint8_t R;
    struct {
       uint8_t  singleShot  : 1;    /*!< Sets conversion mode
                                         [ 0 -> Continuous mode (default) ]
                                         [ 1 -> Single-shot mode ] */
       uint8_t              : 4;    //!< Reserved - must be set to 0
       uint8_t  dr2         : 1;    //!< DR[2:0] bits determine both channel 1
       uint8_t  dr1         : 1;    //!< and channel 2 over-sampling ratio
       uint8_t  dr0         : 1;
    } B;
} BSP_ECG_ADS1192_config1Reg_U;

//!< Configuration Register 2 - configures test signal, clock, reference, LOFF buffer.
typedef union BSP_ECG_ADS1192_config2Reg_UNION {
    uint8_t R;
    struct {
       uint8_t              : 1;    //!< Reserved - must be set to 1
       uint8_t  pdbLoffComp : 1;    //!< Lead-off comparator power-down
       uint8_t  pdbRefBuf   : 1;    //!< Reference buffer power-down
       uint8_t  vRef4V      : 1;    //!< Enables 4V reference
       uint8_t  clkEn       : 1;    //!< CLK connection
       uint8_t              : 1;    //!< Reserved - must be set to 0
       uint8_t  intTest     : 1;    //!< Test signal selection
       uint8_t  testFreq    : 1;    //!< Test signal frequency
    } B;
} BSP_ECG_ADS1192_config2Reg_U;

//!< Channel 1 and 2 settings - configures power mode, PGA, MUX settings.
typedef union BSP_ECG_ADS1192_chXsetReg_UNION {
    uint8_t R;
    struct {
       uint8_t  powerDown   : 1;    //!< Channel X power-down
       uint8_t  pga         : 3;    //!< Channel X PGA gain setting
       uint8_t  mux         : 4;    //!< Channel X input selection
    } B;
} BSP_ECG_ADS1192_chXsetReg_U;


//! ECG ADS1192 driver configuration structure
typedef struct BSP_ECG_ADS1192_config_STRUCT {
    nrf_drv_spi_t        *spiInstance;  //!< SPI master driver instance structure
    nrf_drv_spi_config_t *spiConfig;    //!< SPI master driver instance configuration

} BSP_ECG_ADS1192_config_S;


//! ECG ADS1192 driver device structure
typedef struct BSP_ECG_ADS1192_device_STRUCT {
    BSP_ECG_ADS1192_config_S    *config;        //!< Pointer to ECG driver configuration
    int16_t temperature;                        //!< Temperature of device

    bool isInitialized;                         //!< Is device initialized
} BSP_ECG_ADS1192_device_S;
/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/


/*******************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void BSP_ECG_ADS1192_init(BSP_ECG_ADS1192_device_S *inDevice,
                          BSP_ECG_ADS1192_config_S *inConfig,
                          BSP_ECG_ADS1192_err_E *outErr);

#endif // #ifndef BSP_ECG_ADS1192_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
