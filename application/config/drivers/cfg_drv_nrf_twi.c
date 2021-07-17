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
 * @file    cfg_drv_nrf_twi.c
 * @author  mario.kodba
 * @brief   Configuration for TWI peripheral source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf51.h"
#include "cfg_drv_nrf_twi.h"
#include "cfg_nrf51_muha_pinout.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
//! TWI 1 instance configuration structure
const nrf_drv_twi_config_t configTwi1 = {
        .scl = MPU_SCL,                                         //!< SCL pin number.
        .sda = MPU_SDA,                                         //!< SDA pin number.
        .frequency = TWI_FREQUENCY_FREQUENCY_K400,              //!< TWI frequency.
        .interrupt_priority = TWI_DEFAULT_CONFIG_IRQ_PRIORITY,  //!< Interrupt priority.
        .clear_bus_init = 0u,                                   //!< Clear bus during initialization.
        .hold_bus_uninit = 0u                                   //!< Hold pull up state on GPIO pins after un-initialization.
};

//!< TWI 1 instance
const nrf_drv_twi_t instanceTwi1 = NRF_DRV_TWI_INSTANCE(1);

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
