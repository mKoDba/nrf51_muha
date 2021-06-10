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

#include "app_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_twi.h"
#include "twi_master.h"
#include "drv_timer.h"
#include "drv_spi.h"
#include "nrf51_muha.h"
#include "ble_muha.h"
#include "ble_ecgs.h"
#include "ble_mpu.h"
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

#define APP_TIMER_PRESCALER            0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE        4                                           /**< Size of timer operation queues. */
#define LED_HEARTBEAT_INTERVAL         APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)   //!< Timer interrupt that toggles LED every 500 ms
APP_TIMER_DEF(m_led_timer_id);

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void NRF51_MUHA_initGpio(ERR_E *outErr);
static void NRF51_MUHA_initClock();
static void NRF51_MUHA_initDrivers(ERR_E *outErr);
static void NRF51_MUHA_initBsp(ERR_E *outErr);
static void NRF51_MUHA_mpuDataReadyInterrupt(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void NRF51_MUHA_ecgDataReadyInterrupt(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void NRF51_MUHA_ledHeartbeatInterrupt(void *context);

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

#if (USE_HFCLK == true)
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;

    NRF51_MUHA_initClock();
    // initialize TIMER1 instance
    DRV_TIMER_init(&instanceTimer1, &configTimer1, NULL, &timerErr);

#endif // #if (USE_HFCLK == true)

    if(err == ERR_NONE) {
        BLE_MUHA_init(&err);
    }

    if(err == ERR_NONE) {
        // initialize NRF peripheral drivers
        NRF51_MUHA_initDrivers(&err);
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
 * @brief Function starts main application and should stay here in main loop.
 ***************************************************************************************************
 * @param [out]  *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
void NRF51_MUHA_start(ERR_E *error) {

    ERR_E localErr = ERR_NONE;
    uint32_t err_code = NRF_SUCCESS;
    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    BSP_MPU9150_err_E mpuErr = BSP_MPU9150_err_NONE;
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;

//    if(localErr == ERR_NONE) {
//        HAL_WATCHDOG_start();
//    }


    // initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // create timer for LED heartbeat
    err_code = app_timer_create(&m_led_timer_id,
            APP_TIMER_MODE_REPEATED,
            NRF51_MUHA_ledHeartbeatInterrupt);

    DRV_TIMER_enableTimer(&instanceTimer1, &timerErr);

    if(localErr == ERR_NONE) {
        BSP_ECG_ADS1192_startEcgReading(&ecgDevice, &ecgErr);
    }

    if(ecgErr != BSP_ECG_ADS1192_err_NONE) {
        localErr = ERR_ECG_ADS1192_START_FAIL;
    }

    if(localErr == ERR_NONE) {
        nrf_drv_gpiote_in_event_enable(MPU_INT, true);
    }

    BSP_MPU9150_updateValues(&mpuDevice, &mpuDevice.dataBuffer[0], &mpuErr);

//    uint32_t startTime = DRV_TIMER_captureTimer(&instanceTimer1, DRV_TIMER_cc_CHANNEL0, &timerErr);
//
//    uint32_t endTime = DRV_TIMER_captureTimer(&instanceTimer1, DRV_TIMER_cc_CHANNEL0, &timerErr);
//
//    uint32_t timeDiff = DRV_TIMER_getTimeDiff(&instanceTimer1, &startTime, &endTime);

    if(localErr == ERR_NONE) {
        BLE_MUHA_advertisingStart(&localErr);
    }

    // start application timer
    app_timer_start(m_led_timer_id, LED_HEARTBEAT_INTERVAL, NULL);

#define DEBUG false
#if (DEBUG == true)
    muhaConnected = true;
#endif

    // main loop
    while(true){
        if(muhaConnected == true) {

            /*
             * new data ready to be read from ADS1192
             */
            if(ecgDevice.dataReady == true) {

                DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;
                uint8_t bytes[6] = { 0 };
                // converted 16-bits data
                int16_t outData[3] = { 0 };

//                BSP_ECG_ADS1192_readData(&ecgDevice, 6u, &outData[0], &ecgErr);

                // get data bytes from ADS1192
                DRV_SPI_masterRxBlocking(&instanceSpi0, 6u, &bytes[0], &spiErr);
                // convert data to correct format
                outData[0] = bytes[1] | (bytes[0] << 8u);
                outData[1] = bytes[2] | (bytes[1] << 8u);
                outData[2] = bytes[3] | (bytes[2] << 8u);

                // fill buffer
                if(ecgDevice.changeBuffer == true) {
                    ecgDevice.buffer2[ecgDevice.sampleIndex] = outData[2];
                } else {
                    ecgDevice.buffer1[ecgDevice.sampleIndex] = outData[2];
                }
                ecgDevice.sampleIndex++;

                // with BLE notification, only 20 user data bytes is allowed on nRF51422
                if(ecgDevice.sampleIndex == BSP_ECG_ADS1192_CONNECTION_EVENT_SIZE) {
                    ecgDevice.bufferFull = true;
                    ecgDevice.sampleIndex = 0u;
                    ecgDevice.changeBuffer ^= 1u;
                }

                ecgDevice.dataReady = false;
            }

            // if the ECG buffer is filled, update ECG characteristic data in BLE custom service and push notification if there is TX buffer available
            if(ecgDevice.bufferFull == true) {
                if(ecgDevice.changeBuffer == 0u) {
                    if(muhaBleTxBufferAvailable == true) {
                        err_code = BLE_ECGS_ecgDataUpdate(&m_ecgs, (uint8_t *) &ecgDevice.buffer1[0]);
                    }
                    if(err_code == BLE_ERROR_NO_TX_PACKETS) {
                        muhaBleTxBufferAvailable = false;
                    }
                } else {
                    err_code = BLE_ECGS_ecgDataUpdate(&m_ecgs, (uint8_t *) &ecgDevice.buffer2[0]);
                    if(err_code == BLE_ERROR_NO_TX_PACKETS) {
                        muhaBleTxBufferAvailable = false;
                    }
                }

                ecgDevice.bufferFull = false;
            }

            /*
             * new data ready to be read from MPU-9150
             */
            if(mpuDevice.dataReady == true) {
                // read in new values from MPU
                BSP_MPU9150_updateValues(&mpuDevice, &mpuDevice.dataBuffer[0], &mpuErr);
                // update MPU characteristic data in BLE custom service and push notification if there is TX buffer available
                if(muhaBleTxBufferAvailable == true) {
                    err_code = BLE_ECGS_mpuDataUpdate(&m_ecgs, (uint8_t *) &mpuDevice.dataBuffer[0]);
                }
                if(err_code == BLE_ERROR_NO_TX_PACKETS) {
                    muhaBleTxBufferAvailable = false;
                }
                mpuDevice.dataReady = false;
            }

            if(mpuDevice.twiRxDone == true) {
                mpuDevice.twiRxDone = false;
            }
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
    nrf_drv_gpiote_in_config_t inEcgPinConfig = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    // pin interrupt on transition low->high
    nrf_drv_gpiote_in_config_t inMpuPinConfig = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);


    // initialize GPIO task event
    gpioErr = nrf_drv_gpiote_init();

    // configure interrupt pin for ECG_DRDY signal
    if(gpioErr == NRF_SUCCESS) {
       gpioErr = nrf_drv_gpiote_in_init(ECG_DRDY, &inEcgPinConfig, NRF51_MUHA_ecgDataReadyInterrupt);
    }
    // configure interrupt pin for MPU_INT signal
    if(gpioErr == NRF_SUCCESS) {
       gpioErr = nrf_drv_gpiote_in_init(MPU_INT, &inMpuPinConfig, NRF51_MUHA_mpuDataReadyInterrupt);
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
 * @brief Function initializes HF/LF CLK.
 ***************************************************************************************************
 * @param [out] *outErr - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    10.06.2021.
 **************************************************************************************************/
static void NRF51_MUHA_initClock() {

    // initialize LFCLK needed for BLE, WDT
    HAL_CLK_lfclkStart();

    // initialize HFCLK needed for TIMER instances
    HAL_CLK_hfclkStart();
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
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;

    // initialize watchdog timer
    HAL_WATCHDOG_init(&configWatchdog, NULL, &drvInitErr);

    // initialize SPI 0 peripheral
    DRV_SPI_init(&instanceSpi0, &configSpi0, NULL, &spiErr);

#if (DEPRECATED_TWI == false)
    uint32_t err_code;
    // initialize TWI 1 peripheral
    err_code = nrf_drv_twi_init(&instanceTwi1, &configTwi1, nrf_drv_mpu_twi_event_handler, &mpuDevice);

    if(err_code != NRF_SUCCESS) {
        drvInitErr = ERR_DRV_SPI_INIT_FAIL;
    }

    nrf_drv_twi_enable(&instanceTwi1);
#else
    twi_master_init();
#endif

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

/***********************************************************************************************//**
 * @brief Callback for new data ready signal, called depending on sampling period of MPU-9150.
 * @details Sampling period is set on MPU-9150 initialization in BSP_MPU9150_configuration function.
 ***************************************************************************************************
 * @param [in]  pin    - GPIOTE pin number.
 * @param [in]  action - GPIOTE trigger action.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    06.06.2021.
 **************************************************************************************************/
static void NRF51_MUHA_mpuDataReadyInterrupt(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {

    (void) pin;
    (void) action;

    mpuDevice.dataReady = true;
}

/***********************************************************************************************//**
 * @brief Callback for new data ready signal, called depending on sampling period of ADS1192.
 * @details Sampling period is set on ADS1192 initialization.
 ***************************************************************************************************
 * @param [in]  pin    - GPIOTE pin number.
 * @param [in]  action - GPIOTE trigger action.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    06.06.2021.
 **************************************************************************************************/
static void NRF51_MUHA_ecgDataReadyInterrupt(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {

    (void) pin;
    (void) action;

    ecgDevice.dataReady = true;
}

/***********************************************************************************************//**
 * @brief Timer callback that will toggle LD1 pin (shows if device alive).
 ***************************************************************************************************
 * @param [in] context      - pointer to callback context (if given on initialization).
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    10.06.2021.
 **************************************************************************************************/
static void NRF51_MUHA_ledHeartbeatInterrupt(void *context) {

    (void) context;

    // toggles LED 1
    nrf_gpio_pin_toggle(LD1);
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
