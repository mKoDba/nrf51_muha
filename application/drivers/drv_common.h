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
 * @file    drv_common.h
 * @author  mario.kodba
 * @brief   Common functionality for NRF51 drivers header file.
 **************************************************************************************************/

#ifndef DRV_COMMON_H_
#define DRV_COMMON_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf.h"

#include <stdint.h>
/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void DRV_COMMON_enableIRQPriority(void *type, uint8_t priority);

#endif // #ifndef DRV_COMMON_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
