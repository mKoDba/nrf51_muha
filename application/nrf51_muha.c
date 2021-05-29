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
 * @file    nrf51_muha.c
 * @author  mario.kodba
 * @brief   Initialization and start functions for NRF51 MUHA board source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stddef.h>

#include "nrf_drv_gpiote.h"
#include "nrf_drv_twi.h"
#include "drv_timer.h"
#include "drv_spi.h"
#include "nrf51_muha.h"
#include "ble_muha.h"
#include "ble_ecgs.h"
#include "ble_bas.h"

#include "nrf_gpio.h"
#include "hal_clk.h"
#include "hal_watchdog.h"

#include "cfg_drv_spi.h"
#include "cfg_drv_timer.h"
#include "cfg_drv_nrf_twi.h"
#include "cfg_bsp_ecg_ADS1192.h"
#include "cfg_bsp_mpu9150.h"
#include "cfg_nrf51_muha_pinout.h"
#include "cfg_hal_watchdog.h"
#include "SEGGER_RTT.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define USE_HFCLK true

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void NRF51_MUHA_initGpio(ERR_E *outErr);
static void NRF51_MUHA_initDrivers(ERR_E *outErr);
static void NRF51_MUHA_initBsp(ERR_E *outErr);

/***************************************************************************************************
 *                         PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/

/***********************************************************************************************//**
 * @brief Initializes GPIOs, drivers, BSP needed for application.
 ***************************************************************************************************
 * @param [in, out] *outErr - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
void NRF51_MUHA_init(ERR_E *outErr) {

    ERR_E err = ERR_NONE;

    SEGGER_RTT_printf(0, "Initializing nRF51...\n");

    // initialize GPIOs
    NRF51_MUHA_initGpio(&err);

    if(err == ERR_NONE) {
        // initialize NRF peripheral drivers
        NRF51_MUHA_initDrivers(&err);
    }

    if(err == ERR_NONE) {
        BLE_MUHA_init(&err);
    }

    if(err == ERR_NONE) {
        // initialize BSP components
        NRF51_MUHA_initBsp(&err);
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Function starts main application.
 ***************************************************************************************************
 * @param [out]  *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
void NRF51_MUHA_start(ERR_E *error) {

    ERR_E localErr = ERR_NONE;
    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    BSP_MPU9150_err_E mpuErr = BSP_MPU9150_err_NONE;
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;
//    int16_t temp = 0;

static uint8_t battery_level = 0u;

    uint8_t mpuBuffer[14u] = { 0u };
    int16_t gyroVals[3u] = { 0 };
    float accVals[3] = { 0 };
    int16_t temp = 0;

//    while(true){
//        ;
//    }
//
//    if(localErr == ERR_NONE) {
//        HAL_WATCHDOG_start();
//    }


    if(localErr == ERR_NONE) {
        BSP_ECG_ADS1192_startEcgReading(&ecgDevice, &ecgErr);
    }

    if(ecgErr != BSP_ECG_ADS1192_err_NONE) {
        localErr = ERR_ECG_ADS1192_START_FAIL;
    }

//    if(localErr == ERR_NONE) {
//        BLE_MUHA_advertisingStart(&localErr);
//    }
//
//    // wait for established connection
//    while(m_ecgs.conn_handle == BLE_CONN_HANDLE_INVALID);

    DRV_TIMER_enableTimer(&instanceTimer1, &timerErr);
    uint32_t startTime = DRV_TIMER_captureTimer(&instanceTimer1, DRV_TIMER_cc_CHANNEL0, &timerErr);

    BSP_MPU9150_readMultiReg(&mpuDevice, BSP_MPU9150_REG_ACCEL_XOUT_H, 14u, &mpuBuffer[0], &mpuErr);

    uint32_t endTime = DRV_TIMER_captureTimer(&instanceTimer1, DRV_TIMER_cc_CHANNEL0, &timerErr);

    uint32_t timeDiff = DRV_TIMER_getTimeDiff(&instanceTimer1, &startTime, &endTime);


    //    SEGGER_RTT_printf(0u, "%d\n", BSP_MPU9150_calculateTemperature(&mpuBuffer[0]));
    BSP_MPU9150_calculateGyroValues(&mpuDevice, &mpuBuffer[0], &gyroVals[0]);
    BSP_MPU9150_calculateAccValues(&mpuDevice, &mpuBuffer[0], &accVals[0]);

    while(1) {

    }


    // main loop
    while(true){
        // if the buffer is filled, update BLE data
        if(ecgDevice.updateReady == true) {
            battery_level++;
            ble_bas_battery_level_update(&m_bas, battery_level);
            if(ecgDevice.changeBuffer == 0u) {
                // update MPU9150 data before ECG data, so the notification for ECG will be valid for MPU data also
                BLE_ECGS_mpuDataUpdate(&m_ecgs, (uint8_t *) &mpuBuffer[0], &localErr);
                BLE_ECGS_customValueUpdate(&m_ecgs, (uint8_t *) &ecgDevice.buffer1[0], &localErr);
            } else {
                // update MPU9150 data before ECG data, so the notification for ECG will be valid for MPU data also
                BLE_ECGS_mpuDataUpdate(&m_ecgs, (uint8_t *) &mpuBuffer[0], &localErr);
                BLE_ECGS_customValueUpdate(&m_ecgs, (uint8_t *) &ecgDevice.buffer2[0], &localErr);
            }
            ecgDevice.updateReady = false;
        }
    }

    if(error != NULL) {
        *error = localErr;
    }
}


/***************************************************************************************************
 *                          PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function initializes GPIO pins as inputs/outputs.
 * @details SPI pins are initialized in separate SPI initialization functions.
 ***************************************************************************************************
 * @param [out] *outErr - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
static void NRF51_MUHA_initGpio(ERR_E *outErr) {

    ret_code_t gpioErr = NRF_SUCCESS;
    // pin interrupt on transition high->low, since nDRDY is active low
    nrf_drv_gpiote_in_config_t inPinConfig = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    // initialize GPIO task event
    gpioErr = nrf_drv_gpiote_init();

    // configure interrupt pin for ECG_DRDY signal
    if(gpioErr == NRF_SUCCESS) {
       gpioErr = nrf_drv_gpiote_in_init(ECG_DRDY, &inPinConfig, BSP_ECG_ADS1192_DrdyPin_IRQHandler);
    }

    // initialize output pins
    nrf_gpio_cfg_output(LD1);
    nrf_gpio_cfg_output(ECG_RST);

    if(outErr != NULL) {
        if(gpioErr != NRF_SUCCESS) {
            *outErr = ERR_GPIO_INIT_FAIL;
        }
    }
}

/***********************************************************************************************//**
 * @brief Function initializes NRF51422 peripherals drivers.
 ***************************************************************************************************
 * @param [out] *outErr - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
static void NRF51_MUHA_initDrivers(ERR_E *outErr) {

    ERR_E drvInitErr = ERR_NONE;
    uint32_t err_code;
#if (USE_HFCLK == true)
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;
#endif // #if (USE_HFCLK == true)
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;

    // initialize LFCLK needed for BLE, WDT
    HAL_CLK_lfclkStart();

#if (USE_HFCLK == true)
    // initialize HFCLK needed for TIMER instances
    HAL_CLK_hfclkStart();

    // initialize TIMER1 instance
    DRV_TIMER_init(&instanceTimer1, &configTimer1, NULL, &timerErr);

#endif // #if (USE_HFCLK == true)

    // initialize watchdog timer
    HAL_WATCHDOG_init(&configWatchdog, NULL, &drvInitErr);

    // initialize SPI 0 peripheral
    DRV_SPI_init(&instanceSpi0, &configSpi0, NULL, &spiErr);

    // initialize TWI 1 peripheral
    err_code = nrf_drv_twi_init(&instanceTwi1, &configTwi1, nrf_drv_mpu_twi_event_handler, NULL);
    if(err_code != NRF_SUCCESS) {
        drvInitErr = ERR_DRV_SPI_INIT_FAIL;
    }

    nrf_drv_twi_enable(&instanceTwi1);

    if(outErr != NULL) {
        *outErr = drvInitErr;
    }
}

/***********************************************************************************************//**
 * @brief Function initializes BSP components used by NRF51422 on MUHA board.
 ***************************************************************************************************
 * @param [out] *outErr - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
static void NRF51_MUHA_initBsp(ERR_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    BSP_MPU9150_err_E mpuErr = BSP_MPU9150_err_NONE;
    ERR_E err = ERR_NONE;

    // initialize ADS1192 ECG device
    BSP_ECG_ADS1192_init(&ecgDevice, &ecgDeviceConfig, &ecgErr);

    if(ecgErr != BSP_ECG_ADS1192_err_NONE) {
        err = ERR_ECG_ADS1192_START_FAIL;
    }

    // initialize MPU-9150 device
    BSP_MPU9150_init(&mpuDevice, &mpuDeviceConfig, &mpuErr);

    if(mpuErr != BSP_MPU9150_err_NONE) {
        err = ERR_MPU9150_START_FAIL;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
