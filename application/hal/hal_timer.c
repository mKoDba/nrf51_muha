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
 * @file    hal_timer.c
 * @author  mario.kodba
 * @brief   HAL layer for TIMER module functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "hal_timer.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define HAL_TIMER_PRESCALER_MASK        (0xFUL)             //!< TIMER prescaler mask
#define HAL_TIMER_BITMODE_MASK          (3UL)               //!< TIMER bitmode mask
#define HAL_TIMER_MODE_MASK             (1UL)               //!< TIMER mode mask
#define HAL_TIMER_INTENSET_MASK         (0x10000UL)         //!< TIMER interrupt set mask

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Sets prescaler value according to wanted tick frequency.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  frequency  - wanted tick frequency.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void HAL_TIMER_setFrequency(NRF_TIMER_Type *tInstance, DRV_TIMER_freq_E frequency) {
    tInstance->PRESCALER =
            (tInstance->PRESCALER & ~HAL_TIMER_PRESCALER_MASK) |
            ((uint8_t) frequency & HAL_TIMER_PRESCALER_MASK);
}

/***********************************************************************************************//**
 * @brief Sets timer bit width.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  bitWidth   - timer bit width.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void HAL_TIMER_setBitWidth(NRF_TIMER_Type *tInstance, DRV_TIMER_bitWidth_E bitWidth) {
    tInstance->BITMODE = (tInstance->BITMODE & ~HAL_TIMER_BITMODE_MASK) |
                       ((uint8_t) bitWidth & HAL_TIMER_BITMODE_MASK);
}

/***********************************************************************************************//**
 * @brief Sets timer mode.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  mode       - timer mode.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void HAL_TIMER_setMode(NRF_TIMER_Type *tInstance, DRV_TIMER_mode_E mode) {
    tInstance->MODE = (tInstance->MODE & ~HAL_TIMER_MODE_MASK) |
                       ((uint8_t) mode & HAL_TIMER_MODE_MASK);
}

/***********************************************************************************************//**
 * @brief Clears events registers for given timer instance.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void HAL_TIMER_clearEvents(NRF_TIMER_Type *tInstance) {
    for(uint8_t i = 0u; i < (uint8_t) DRV_TIMER_cc_CHANNEL_COUNT; i++) {
        HAL_TIMER_clearEvent(tInstance, i);
    }
}

/***********************************************************************************************//**
 * @brief Clears event register for given timer instance and CC channel.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  channel    - compare/capture channel id.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
void HAL_TIMER_clearEvent(NRF_TIMER_Type *tInstance, uint8_t channel) {
    *((volatile uint32_t *)((uint8_t *) tInstance + offsetof(NRF_TIMER_Type, EVENTS_COMPARE[channel]))) = 0x0UL;
}

/***********************************************************************************************//**
 * @brief Checks if the event is active for given timer instance and CC channel.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  channel    - compare/capture channel id.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
bool HAL_TIMER_checkEvent(NRF_TIMER_Type *tInstance, uint8_t channel) {
    return (bool) *(volatile uint32_t *)((uint32_t *) tInstance + offsetof(NRF_TIMER_Type, EVENTS_COMPARE[channel]));
}

/***********************************************************************************************//**
 * @brief Getter function for TIMERx compare[x] event.
 ***************************************************************************************************
 * @param [in]  channel    - compare/capture channel id.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    02.01.2021.
 **************************************************************************************************/
DRV_TIMER_event_E HAL_TIMER_getEvent(uint8_t channel) {
    return (DRV_TIMER_event_E) (offsetof(NRF_TIMER_Type, EVENTS_COMPARE[channel]));
}

/***********************************************************************************************//**
 * @brief Checks if the event interrupt is active for given timer instance
 *        and CC channel.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  channel    - compare/capture channel id.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
bool HAL_TIMER_checkIntEn(NRF_TIMER_Type *tInstance, uint8_t channel) {
    return (bool) (tInstance->INTENSET & (HAL_TIMER_INTENSET_MASK << channel));
}

/***********************************************************************************************//**
 * @brief Enables interrupt on COMPARE event for given channel.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  channel    - compare/capture channel id.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    02.01.2021.
 **************************************************************************************************/
void HAL_TIMER_enableInterrupt(NRF_TIMER_Type *tInstance, uint8_t channel) {
    tInstance->INTENSET |= (HAL_TIMER_INTENSET_MASK << channel);
}

/***********************************************************************************************//**
 * @brief Disables interrupt on COMPARE event for given channel.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  channel    - compare/capture channel id.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    02.01.2021.
 **************************************************************************************************/
void HAL_TIMER_disableInterrupt(NRF_TIMER_Type *tInstance, uint8_t channel) {
    tInstance->INTENSET &= ~(HAL_TIMER_INTENSET_MASK << channel);
}

/***********************************************************************************************//**
 * @brief Write compare value to compare channel which will trigger interrupt.
 ***************************************************************************************************
 * @param [in]  *tInstance   - pointer to timer instance.
 * @param [in]  channel      - compare/capture channel id.
 * @param [in]  compareValue - compare value.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    02.01.2021.
 **************************************************************************************************/
void HAL_TIMER_writeCompareValue(NRF_TIMER_Type *tInstance,
        uint8_t channel,
        uint32_t compareValue) {

    tInstance->CC[channel] = compareValue;
}

/***********************************************************************************************//**
 * @brief Starts given timer task.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  task       - timer task.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void HAL_TIMER_runTask(NRF_TIMER_Type *tInstance, DRV_TIMER_task_E task) {
    *((volatile uint32_t *) ((uint8_t *) tInstance + (uint32_t) task)) = 0x1UL;
}

/***********************************************************************************************//**
 * @brief Getter function for CC channel captured value.
 ***************************************************************************************************
 * @param [in]  *tInstance - pointer to timer instance.
 * @param [in]  channel    - timer CC channel.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
uint32_t HAL_TIMER_getValue(NRF_TIMER_Type *tInstance, DRV_TIMER_cc_E channel) {
    return (uint32_t) tInstance->CC[channel];
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
