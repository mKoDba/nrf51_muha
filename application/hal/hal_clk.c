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
 * @file    hal_clk.c
 * @author  mario.kodba
 * @brief   HAL layer for clock module functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "hal_clk.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define HAL_CLK_HFCLKSTAT_STATE_MASK        (1UL << 16)         //!< HFCLK status state mask
#define HAL_CLK_HFCLKSTAT_SOURCE_MASK       (1UL)               //!< HFCLK status source mask

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static bool HAL_CLK_hfclkCheckState(void);
static bool HAL_CLK_hfclkCheckSrc(void);

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Starts HFCLK clock.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
void HAL_CLK_hfclkStart(void) {

    // start 16 MHz crystal oscillator
    // clear interrupt pending
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0u;
    // 32MHz crystal as source
    NRF_CLOCK->XTALFREQ = 0x00u;
    NRF_CLOCK->TASKS_HFCLKSTART = 1u;

    // wait for the external oscillator to start up - it will set the flag to 1
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    HAL_CLK_hfclkCheckSrc();
}

/***********************************************************************************************//**
 * @brief Stops HFCLK clock.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    26.12.2020.
 **************************************************************************************************/
void HAL_CLK_hfclkStop(void) {

    bool state = HAL_CLK_hfclkCheckState();

    // stop HFCLK if it's running
    if(state == true) {
        NRF_CLOCK->TASKS_HFCLKSTOP = 1UL;
    }
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Checks HFCLK state.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    26.12.2020.
 **************************************************************************************************/
static bool HAL_CLK_hfclkCheckState(void) {

    bool state = (NRF_CLOCK->HFCLKSTAT & HAL_CLK_HFCLKSTAT_STATE_MASK);

    return state;
}

/***********************************************************************************************//**
 * @brief Checks HFCLK source.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    26.12.2020.
 **************************************************************************************************/
static bool HAL_CLK_hfclkCheckSrc(void) {

    bool source = (NRF_CLOCK->HFCLKSTAT & HAL_CLK_HFCLKSTAT_SOURCE_MASK);

    return source;
}
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
