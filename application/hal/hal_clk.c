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
#define HAL_CLK_LFCLKSTAT_STATE_MASK        (1UL << 16)         //!< LFCLK status state mask
#define HAL_CLK_HFCLK_16_MHZ                (0xFFu)             //!< 16 MHz crystal used for HFCLK
#define HAL_CLK_HFCLK_32_MHZ                (0x00u)             //!< 32 MHz crystal used for HFCLK

#define HAL_CLK_HFCLKSTAT_SOURCE_MASK       (1UL)               //!< HFCLK status source mask
#define HAL_CLK_LFCLKSTAT_SOURCE_MASK       (3UL)               //!< LFCLK status source mask
#define HAL_CLK_LFCLK_RC                    (0x0u)              //!< RC oscillator as LFCLK source
#define HAL_CLK_LFCLK_XTAL                  (0x1u)              //!< External crystal as LFCLK source
#define HAL_CLK_LFCLK_SYNTH                 (0x2u)              //!< HFCLK synthesized as LFCLK source

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static bool HAL_CLK_hfclkCheckState(void);
static bool HAL_CLK_hfclkCheckSrc(void);

static bool HAL_CLK_lfclkCheckState(void);
static uint8_t HAL_CLK_lfclkCheckSrc(void);

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

    // clear interrupt pending
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0u;
    // 32MHz crystal as source
    NRF_CLOCK->XTALFREQ = HAL_CLK_HFCLK_32_MHZ;
    NRF_CLOCK->TASKS_HFCLKSTART = 1u;

    /*
     * Wait for the external oscillator to start up - it will set the flag to 1.
     * Takes about 750us according to nRF51422 PS v3.2
     */
    // TODO: [mario.kodba 03.04.2021] Add timeout
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

/***********************************************************************************************//**
 * @brief Starts LFCLK clock.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void HAL_CLK_lfclkStart(void) {

    // clear interrupt pending
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0u;
    // 32kHz external crystal as source
    NRF_CLOCK->LFCLKSRC = HAL_CLK_LFCLK_XTAL;
    NRF_CLOCK->TASKS_LFCLKSTART = 1u;

    /*
     * Wait for the external oscillator to start up - it will set the flag to 1.
     * Takes maximum of 1s according to nRF51422 PS v3.2
     */
    // TODO: [mario.kodba 03.04.2021] Add timeout
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);

    HAL_CLK_lfclkCheckSrc();
}

/***********************************************************************************************//**
 * @brief Stops LFCLK clock.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void HAL_CLK_lfclkStop(void) {

    bool state = HAL_CLK_lfclkCheckState();

    // stop LFCLK if it's running
    if(state == true) {
        NRF_CLOCK->TASKS_LFCLKSTOP = 1UL;
    }
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Checks HFCLK state.
 ***************************************************************************************************
 * @param [in]  - None.
 * @return Value of HFCLK source. 0 - RC, 1 - XTAL.
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

/***********************************************************************************************//**
 * @brief Checks LFCLK state.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static bool HAL_CLK_lfclkCheckState(void) {

    bool state = (NRF_CLOCK->LFCLKSTAT & HAL_CLK_LFCLKSTAT_STATE_MASK);

    return state;
}

/***********************************************************************************************//**
 * @brief Checks LFCLK source.
 ***************************************************************************************************
 * @param [in]  - None.
 * @return Value of LFCLK source. 0 - RC, 1 - XTAL, 2 - synthesized from HFCLK.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static uint8_t HAL_CLK_lfclkCheckSrc(void) {

    uint8_t source = (NRF_CLOCK->LFCLKSTAT & HAL_CLK_LFCLKSTAT_SOURCE_MASK);

    return source;
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
