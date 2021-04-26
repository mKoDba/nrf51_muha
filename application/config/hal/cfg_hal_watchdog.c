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
 * @file    cfg_hal_watchdog.c
 * @author  mario.kodba
 * @brief   Configuration for WATCHDOG peripheral source file.
 **************************************************************************************************/

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "cfg_hal_watchdog.h"

#include "nrf51.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define GET_TIMEOUT_PERIOD_IN_SEC(SECONDS)  ((SECONDS) * (32767u))

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
//! WATCHDOG configuration structure
HAL_WATCHDOG_config_S configWatchdog = {
        .behavior = HAL_WATCHDOG_behavior_PAUSE_SLEEP_HALT, //!< Wanted watchdog counter behavior.
        .reloadValue = GET_TIMEOUT_PERIOD_IN_SEC(5u),       //!< Reload value from which watchdog down counts.
        .reloadRegNumber = 1u,                              //!< Number of reload registers to be enabled.
        .irq = WDT_IRQn,                                    //!< WATCHDOG instance IRQ number.
        .irqPriority = 3u,                                  //!< Interrupt priority. 0 - 3 for Cortex M0.
        .context = NULL                                     //!< Context passed to interrupt handler (callback function).
};

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
