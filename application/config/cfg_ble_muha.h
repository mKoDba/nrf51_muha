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
 * @file    cfg_ble_muha.h
 * @author  mario.kodba
 * @brief   Configuration for BLE functionality header file.
 **************************************************************************************************/

#ifndef CFG_BLE_MUHA_H_
#define CFG_BLE_MUHA_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf_sdm.h"
#include "ble.h"

/***************************************************************************************************
 *                                  CONSTANTS
 **************************************************************************************************/


/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
extern nrf_clock_lf_cfg_t bleLfClock;
extern ble_gap_adv_params_t bleAdvertisingParams;
extern ble_enable_params_t bleEnableParams;
extern ble_gap_conn_params_t gapConnectionParams;

/***************************************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/

#endif // #ifndef CFG_BLE_MUHA_H_ */
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/