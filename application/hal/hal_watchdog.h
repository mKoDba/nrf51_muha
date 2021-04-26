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
 * @file    hal_watchdog.h
 * @author  mario.kodba
 * @brief   HAL layer for watchdog functionality header file.
 **************************************************************************************************/

#ifndef HAL_WATCHDOG_H_
#define HAL_WATCHDOG_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf51.h"
#include "nrf51_muha.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/
//! WATCHDOG event enumeration
typedef enum HAL_WATCHDOG_event_ENUM {
    HAL_WATCHDOG_event_TIMEOUT = 0u                     //!< WATCHDOG event TIMEOUT.
} HAL_WATCHDOG_event_E;

//! WATCHDOG behavior enumeration
typedef enum HAL_WATCHDOG_behavior_ENUM {
    HAL_WATCHDOG_behavior_PAUSE_SLEEP_HALT = 0u,        //!< WATCHDOG counter will pause on CPU SLEEP and HALT.
    HAL_WATCHDOG_behavior_PAUSE_HALT = 1u,              //!< WATCHDOG counter will pause on CPU HALT (runs on SLEEP).
    HAL_WATCHDOG_behavior_PAUSE_SLEEP = 8u,             //!< WATCHDOG counter will pause on CPU SLEEP (runs on HALT).
    HAL_WATCHDOG_behavior_RUN_CONTINUOUS = 9u           //!< WATCHDOG counter will run on CPU SLEEP and HALT.
} HAL_WATCHDOG_behavior_E;

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! WATCHDOG callback function pointer
typedef void (*HAL_WATCHDOG_IRQHandler)(HAL_WATCHDOG_event_E event, void *context);

//! WATCHDOG control block structure
typedef struct HAL_WATCHDOG_control_block_STRUCT {
    HAL_WATCHDOG_IRQHandler callbackFunction;           //!< Callback function for WATCHDOG interrupt.
    void *context;                                      //!< Context passed to WATCHDOG callback function.

} HAL_WATCHDOG_control_block_S;

//! WATCHDOG configuration structure
typedef struct HAL_WATCHDOG_config_STRUCT {
    HAL_WATCHDOG_behavior_E behavior;                   //!< Wanted watchdog counter behavior.
    uint32_t reloadValue;                               //!< Reload value from which watchdog down counts.
    uint32_t reloadRegNumber;                           //!< Number of reload registers to be enabled.
    IRQn_Type irq;                                      //!< WATCHDOG instance IRQ number.
    uint8_t irqPriority;                                //!< Interrupt priority. 0 - 3 for Cortex M0.
    void *context;                                      //!< Context passed to interrupt handler (callback function).
} HAL_WATCHDOG_config_S;


/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void HAL_WATCHDOG_init(HAL_WATCHDOG_config_S *watchdogConfig,
        HAL_WATCHDOG_IRQHandler irqHandler,
        ERR_E *error);
void HAL_WATCHDOG_feed(uint8_t reloadReg);
void HAL_WATCHDOG_start();

#endif // #ifndef HAL_WATCHDOG_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/

