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
#include "app_fifo.h"
#include "ringbuffer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "twi_master.h"
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
#include "cfg_nrf51_muha_pinout.h"
#include "cfg_hal_watchdog.h"
#include "cfg_bsp_ecg_ADS1192.h"
#include "cfg_bsp_mpu9150.h"
#include "SEGGER_RTT.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define USE_HFCLK       true

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
 * @param [in]   *muha   - pointer to main handle structure.
 * @param [out]  *outErr - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
void NRF51_MUHA_init(NRF51_MUHA_handle_S *muha, ERR_E *outErr) {

    ERR_E err = ERR_NONE;

    // initialize GPIOs
    NRF51_MUHA_initGpio(&err);

#if (USE_HFCLK == true)
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;

    // BLE caused fault happens if HFCLK not initialized
    NRF51_MUHA_initClock();
    // initialize TIMER1 instance
    DRV_TIMER_init(muha->timer1, &configTimer1, NULL, &timerErr);

#endif // #if (USE_HFCLK == true)

    if(err == ERR_NONE) {
        // initialize BLE functionalities
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
 * @param [in]   *muha - pointer to main handle structure.
 * @param [out]  *err  - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
void NRF51_MUHA_start(NRF51_MUHA_handle_S *muha, ERR_E *error) {

    ERR_E localErr = ERR_NONE;
    uint32_t err_code = NRF_SUCCESS;
    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    BSP_MPU9150_err_E mpuErr = BSP_MPU9150_err_NONE;
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;
    // 16-bit data from ADS1192 goes here
    int16_t ecgData[3] = { 0 };

    // create FIFO structures for both ADS1192 and MPU-9150
    ring_buffer_t ecgFifoStruct;
    ring_buffer_t mpuFifoStruct;

    ring_buffer_init(&ecgFifoStruct);
    ring_buffer_init(&mpuFifoStruct);

    // initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // create timer for LED heartbeat
    err_code = app_timer_create(&m_led_timer_id,
            APP_TIMER_MODE_REPEATED,
            NRF51_MUHA_ledHeartbeatInterrupt);

//    DRV_TIMER_enableTimer(muha->timer1, &timerErr);

    if(localErr == ERR_NONE) {
        BSP_ECG_ADS1192_startEcgReading(muha->ads1192, &ecgErr);
    }

    if(ecgErr != BSP_ECG_ADS1192_err_NONE) {
        localErr = ERR_ECG_ADS1192_START_FAIL;
    }

    if(localErr == ERR_NONE) {
        nrf_drv_gpiote_in_event_enable(MPU_INT, true);
    }


//    uint32_t startTime = DRV_TIMER_captureTimer(&instanceTimer1, DRV_TIMER_cc_CHANNEL0, &timerErr);
//
//    uint32_t endTime = DRV_TIMER_captureTimer(&instanceTimer1, DRV_TIMER_cc_CHANNEL0, &timerErr);
//
//    uint32_t timeDiff = DRV_TIMER_getTimeDiff(&instanceTimer1, &startTime, &endTime);

    // start application timer
    app_timer_start(m_led_timer_id, LED_HEARTBEAT_INTERVAL, NULL);

    if(localErr == ERR_NONE) {
        BLE_MUHA_advertisingStart(&localErr);
    }


#if (DEBUG == true)
    muhaConnected = true;
#endif

    // main loop
    while(true){

        if(muhaConnected == true) {
            /*
             * new data ready to be read from ADS1192
             */
            if(muha->ads1192->dataReady == true) {

                BSP_ECG_ADS1192_readData(muha->ads1192, 6u, &ecgData[0], &ecgErr);
//                SEGGER_RTT_printf(0u, "%d\n", ecgData[2]);

                if(ring_buffer_is_full(&ecgFifoStruct) == 0u) {
                    ring_buffer_queue_arr(&ecgFifoStruct, (char *) &ecgData[2], 2u);
                }

                muha->ads1192->buffer[muha->ads1192->sampleIndex] = ecgData[2];
                muha->ads1192->sampleIndex++;

                // with BLE notification, only 20 user data bytes is allowed on nRF51422
                if(muha->ads1192->sampleIndex == BSP_ECG_ADS1192_CONNECTION_EVENT_SIZE) {
                    muha->ads1192->bufferFull = true;
                    muha->ads1192->sampleIndex = 0u;
                }

                muha->ads1192->dataReady = false;
            }

            // if the ECG buffer is filled, update ECG characteristic data in BLE custom service and push notification if there is TX buffer available
            if(muha->ads1192->bufferFull == true) {

                if(muhaBleTxBufferAvailable == true) {

                    ring_buffer_dequeue_arr(&ecgFifoStruct, (char *) &muha->ads1192->buffer[0], NRF51_MUHA_ADS1192_BLE_BYTE_SIZE);

                    err_code = BLE_ECGS_ecgDataUpdate(muha->customService, (uint8_t *) &muha->ads1192->buffer[0]);

                    if(err_code == BLE_ERROR_NO_TX_PACKETS) {
                        muhaBleTxBufferAvailable = false;
                    }
                }

                muha->ads1192->bufferFull = false;
            }


            /*
             * new data ready to be read from MPU-9150
             */
            if(muha->mpu9150->dataReady == true) {
                // read in new values from MPU
                BSP_MPU9150_updateValues(muha->mpu9150, &muha->mpu9150->dataBuffer[0], &mpuErr);

                if(ring_buffer_is_full(&mpuFifoStruct) == 0u) {
                    ring_buffer_queue_arr(&mpuFifoStruct, (char *) &muha->mpu9150->dataBuffer[0], NRF51_MUHA_MPU9150_BLE_BYTE_SIZE);
                }

//                SEGGER_RTT_printf(0u, "%d,%d,%d,%d,%d,%d\n", muha->mpu9150->dataBuffer[0],
//                        muha->mpu9150->dataBuffer[1],
//                        muha->mpu9150->dataBuffer[2], muha->mpu9150->dataBuffer[3],
//                        muha->mpu9150->dataBuffer[4], muha->mpu9150->dataBuffer[5]);

                // update MPU characteristic data in BLE custom service and push notification if there is TX buffer available
                if(muhaBleTxBufferAvailable == true) {

                    ring_buffer_dequeue_arr(&mpuFifoStruct, (char *) &muha->mpu9150->dataBuffer[0], NRF51_MUHA_MPU9150_BLE_BYTE_SIZE);

                    err_code = BLE_ECGS_mpuDataUpdate(muha->customService, (uint8_t *) &muha->mpu9150->dataBuffer[0]);
                    if(err_code == BLE_ERROR_NO_TX_PACKETS) {
                        muhaBleTxBufferAvailable = false;
                    }
                }

                muha->mpu9150->dataReady = false;
            }

            if(muha->mpu9150->twiRxDone == true) {
                muha->mpu9150->twiRxDone = false;
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

    if(muhaConnected) {
        // toggles LED 1
        nrf_gpio_pin_toggle(LD1);
    } else {
        // keep ON
        nrf_gpio_pin_clear(LD1);
    }
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
