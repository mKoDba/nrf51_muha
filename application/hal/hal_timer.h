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
 * @file    hal_timer.h
 * @author  mario.kodba
 * @brief   HAL layer for TIMER module functionality header file.
 **************************************************************************************************/

#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_

/***************************************************************************************************
 *                              INCLUDE FILES
  **************************************************************************************************/
#include <stdint.h>
#include <stddef.h>

#include "nrf51.h"
#include "drv_timer.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void HAL_TIMER_setFrequency(NRF_TIMER_Type *tInstance, DRV_TIMER_freq_E frequency);
void HAL_TIMER_setBitWidth(NRF_TIMER_Type *tInstance, DRV_TIMER_bitWidth_E bitWidth);
void HAL_TIMER_setMode(NRF_TIMER_Type *tInstance, DRV_TIMER_mode_E mode);

void HAL_TIMER_clearEvents(NRF_TIMER_Type *tInstance);
void HAL_TIMER_clearEvent(NRF_TIMER_Type *tInstance, uint32_t channel);
void HAL_TIMER_runTask(NRF_TIMER_Type *tInstance, DRV_TIMER_task_E task);
DRV_TIMER_event_E HAL_TIMER_getEvent(uint8_t channel);
bool HAL_TIMER_checkEvent(NRF_TIMER_Type *tInstance, uint32_t channel);
bool HAL_TIMER_checkIntEn(NRF_TIMER_Type *tInstance, uint8_t channel);
void HAL_TIMER_enableInterrupt(NRF_TIMER_Type *tInstance, uint32_t channel);
void HAL_TIMER_disableInterrupt(NRF_TIMER_Type *tInstance, uint8_t channel);
void HAL_TIMER_writeCompareValue(NRF_TIMER_Type *tInstance,
        uint32_t channel,
        uint32_t compareValue);
uint32_t HAL_TIMER_getValue(NRF_TIMER_Type *tInstance, DRV_TIMER_cc_E channel);

#endif // #ifndef HAL_TIMER_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
