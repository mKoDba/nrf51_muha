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
 * @file    drv_common.c
 * @author  mario.kodba
 * @brief   Common functionality for NRF51 drivers source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "drv_common.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define DRV_COMMON_IRQ_PRIORITY_SHIFT_VALUE    (12U)        //!< IRQ priority shift value

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Sets priority and enables IRQ.
 ***************************************************************************************************
 * @param [in] *type    - describe what that something is.
 * @param [in] priority - describe what that something is.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.20.2020.
 **************************************************************************************************/
void DRV_COMMON_enableIRQPriority(void *type, uint8_t priority) {
    // TODO: [add check if priority is within range/allowed]
    uint8_t typeNum = (uint8_t)((uint32_t)type >> DRV_COMMON_IRQ_PRIORITY_SHIFT_VALUE);

    NVIC_SetPriority((IRQn_Type) typeNum, priority);
    NVIC_ClearPendingIRQ((IRQn_Type) typeNum);
    NVIC_EnableIRQ((IRQn_Type) typeNum);
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
