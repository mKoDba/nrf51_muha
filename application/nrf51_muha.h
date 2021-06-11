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
 * @file    nrf51_muha.h
 * @author  mario.kodba
 * @brief   Initialization and start functions for NRF51 MUHA board header file.
 **************************************************************************************************/

#ifndef NRF51_MUHA_H_
#define NRF51_MUHA_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>

#include "bsp_ecg_ADS1192.h"
#include "bsp_mpu9150.h"
#include "drv_timer.h"
#include "ble_ecgs.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
//TODO: increase this to 20 when Magnetometer data is included
//! Number of bytes to send for MPU9150 in each BLE connection event
#define NRF51_MUHA_MPU9150_BLE_BYTE_SIZE    (14u)
//! Number of bytes to send for ADS1192 in each BLE connection event
#define NRF51_MUHA_ADS1192_BLE_BYTE_SIZE    (BSP_ECG_ADS1192_CONNECTION_EVENT_SIZE * sizeof(int16_t))

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! MUHA board error enumeration
typedef enum ERR_ENUM {
    ERR_NONE                            = 0u,       //!< No error.
    ERR_NULL_PARAMETER,                             //!< Null parameter error.
    ERR_DRV_SPI_INIT_FAIL,                          //!< SPI driver initialization error.
    ERR_DRV_TWI_INIT_FAIL,                          //!< TWI driver initialization error.
    ERR_DRV_TIMER_INIT_FAIL,                        //!< TIMER driver initialization error.
    ERR_GPIO_INIT_FAIL,                             //!< GPIO initialization error.
    ERR_BLE_STACK_INIT_FAIL,                        //!< BLE stack/softdevice initialization error.
    ERR_BLE_GAP_INIT_FAIL,                          //!< BLE GAP parameters initialization error.
    ERR_BLE_SERVICE_INIT_FAIL,                      //!< BLE custom service initialization error.
    ERR_BLE_CONNECTIONS_INIT_FAIL,                  //!< BLE Connections parameters initialization error.
    ERR_BLE_ADVERTISING_DATA_INIT_FAIL,             //!< BLE Advertising data initialization error.
    ERR_BLE_ADVERTISING_START_FAIL,                 //!< BLE Advertising start error.
    ERR_ECG_ADS1192_START_FAIL,                     //!< ADS1192 device start error.
    ERR_MPU9150_START_FAIL,                         //!< MPU9150 device start error.
    ERR_HAL_WATCHDOG_INIT_FAIL,                     //!< WATCHDOG module initialization error.

    ERR_COUNT                                       //!< Total number of errors.
} ERR_E;

//! nRF51 MUHA structure containing pointers to needed references
typedef struct NRF51_MUHA_handle_STRUCT {
    BSP_ECG_ADS1192_device_S *ads1192;              //!< Pointer to ADS1192 structure.
    BSP_MPU9150_device_S     *mpu9150;              //!< Pointer to MPU9150 structure.
    BLE_ECGS_custom_S        *customService;        //!< Pointer to Custom Service structure.
    DRV_TIMER_instance_S     *timer1;               //!< Pointer to TIMER1 structure.

} NRF51_MUHA_handle_S;

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void NRF51_MUHA_init(NRF51_MUHA_handle_S *muha, ERR_E *outErr);
void NRF51_MUHA_start(NRF51_MUHA_handle_S *muha, ERR_E *error);

#endif // #ifndef NRF51_MUHA_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
