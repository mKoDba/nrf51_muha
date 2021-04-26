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
 * @file    hal_watchdog.c
 * @author  mario.kodba
 * @brief   HAL layer for watchdog functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "hal_watchdog.h"
#include "drv_common.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define HAL_WATCHDOG_RELOAD_KEY     (0x6E524635UL)   //!< Watchdog reload key value.

/***************************************************************************************************
 *                           GLOBAL VARIABLES
 **************************************************************************************************/
static HAL_WATCHDOG_control_block_S HAL_WATCHDOG_controlBlock;

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void HAL_WATCHDOG_setReloadValue(uint32_t reloadValue);
static void HAL_WATCHDOG_setBehavior(HAL_WATCHDOG_behavior_E behaviour);
static void HAL_WATCHDOG_enableReloadRegs(uint32_t reloadRegs);

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***************************************************************************************************
 * @brief Initializes watchdog functionality.
 ***************************************************************************************************
 * @param [in]  *watchdogConfig   - Pointer to watchdog configuration structure.
 * @param [in]  irqHandler        - Function pointer to user defined IRQ handler.
 * @param [out] *error            - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void HAL_WATCHDOG_init(HAL_WATCHDOG_config_S *watchdogConfig,
        HAL_WATCHDOG_IRQHandler irqHandler,
        ERR_E *error) {

    ERR_E errLocal = ERR_NONE;

    if(watchdogConfig != NULL) {
        // set callback function and context, if given
        HAL_WATCHDOG_control_block_S *block = &HAL_WATCHDOG_controlBlock;
        block->callbackFunction = irqHandler;
        block->context = watchdogConfig->context;

        HAL_WATCHDOG_setBehavior(watchdogConfig->behavior);

        HAL_WATCHDOG_enableReloadRegs(watchdogConfig->reloadRegNumber);

        HAL_WATCHDOG_setReloadValue(watchdogConfig->reloadValue);

        DRV_COMMON_enableIRQPriority(&watchdogConfig->irq, watchdogConfig->irqPriority);
    } else {
        errLocal = ERR_HAL_WATCHDOG_INIT_FAIL;
    }

    if(error != NULL) {
        *error = errLocal;
    }
}

/***************************************************************************************************
 * @brief Feeds (restarts timer) watchdog.
 ***************************************************************************************************
 * @param [in] - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void HAL_WATCHDOG_feed(uint8_t reloadReg) {

    NRF_WDT->RR[reloadReg] = HAL_WATCHDOG_RELOAD_KEY;
}

/***************************************************************************************************
 * @brief Starts watchdog timer.
 ***************************************************************************************************
 * @param [in] - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void HAL_WATCHDOG_start() {

    NRF_WDT->TASKS_START = 1u;
}


/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***************************************************************************************************
 * @brief Sets watchdog timer register value from which it down counts.
 ***************************************************************************************************
 * @param [in]  reloadValue - Reload value of watchdog timer.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static void HAL_WATCHDOG_setReloadValue(uint32_t reloadValue) {
    NRF_WDT->CRV = reloadValue;
}

/***************************************************************************************************
 * @brief Sets watchdog timer behavior.
 ***************************************************************************************************
 * @param [in]  behavior - Behavior of watchdog timer.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static void HAL_WATCHDOG_setBehavior(HAL_WATCHDOG_behavior_E behavior) {
    NRF_WDT->CONFIG = (uint32_t) behavior;
}

/***************************************************************************************************
 * @brief Enables watchdog reload registers to be used.
 ***************************************************************************************************
 * @param [in]  reloadRegs - Number of reload registers to be enabled.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static void HAL_WATCHDOG_enableReloadRegs(uint32_t reloadRegs) {

    uint32_t value = 0u;

    for(uint32_t i = 0u; i < reloadRegs; i++) {
        value = value | 1 << i;
    }

    NRF_WDT->RREN = value;
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
