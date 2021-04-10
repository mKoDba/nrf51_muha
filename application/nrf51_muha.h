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


/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! MUHA board error enumeration
typedef enum ERR_ENUM {
    ERR_NONE                            = 0u,       //!< No error.
    ERR_DRV_SPI_INIT_FAIL,                          //!< SPI driver initialization error.
    ERR_DRV_TIMER_INIT_FAIL,                        //!< TIMER driver initialization error.
    ERR_GPIO_INIT_FAIL,                             //!< GPIO initialization error.
    ERR_BLE_STACK_INIT_FAIL,                        //!< BLE stack/softdevice initialization error.
    ERR_BLE_GAP_INIT_FAIL,                          //!< BLE GAP parameters initialization error.
    ERR_BLE_CONNECTIONS_INIT_FAIL,                  //!< BLE Connections parameters initialization error.
    ERR_BLE_ADVERTISING_DATA_INIT_FAIL,             //!< BLE Advertising data initialization error.
    ERR_BLE_ADVERTISING_START_FAIL,                 //!< BLE Advertising start error.
    ERR_ECG_ADS1192_START_FAIL,                     //!< ADS1192 device start error.
    ERR_HAL_WATCHDOG_INIT_FAIL,                     //!< WATCHDOG module initialization error.

    ERR_COUNT                                       //!< Total number of errors.
} ERR_E;

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void NRF51_MUHA_init(ERR_E *error);
void NRF51_MUHA_start(ERR_E *error);

#endif // #ifndef NRF51_MUHA_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
