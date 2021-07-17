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
 * @file    cfg_drv_nrf_twi.h
 * @author  mario.kodba
 * @brief   Configuration for TWI peripheral header file.
 **************************************************************************************************/

#ifndef CFG_DRV_NRF_TWI_H_
#define CFG_DRV_NRF_TWI_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf_drv_twi.h"

/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
extern const nrf_drv_twi_config_t configTwi1;
extern const nrf_drv_twi_t instanceTwi1;

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/

#endif // #ifndef CFG_DRV_NRF_TWI_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
