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
 * @file    ble_muha.h
 * @author  mario.kodba
 * @brief   Implementation of BLE functionality for MUHA board header file.
 **************************************************************************************************/

#ifndef BLE_MUHA_H_
#define BLE_MUHA_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf51_muha.h"
#include "ble.h"

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
extern volatile uint8_t muhaConnected;
extern volatile uint8_t muhaMpuNotificationEnabled;
extern volatile uint8_t muhaEcgNotificationEnabled;
extern volatile uint8_t muhaBleTxBufferAvailable;

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void BLE_MUHA_init(ERR_E *error);
void BLE_MUHA_advertisingStart(ERR_E *err);
void BLE_MUHA_bleEventCallback(ble_evt_t *bleEvent);

#endif // #ifndef BLE_MUHA_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
