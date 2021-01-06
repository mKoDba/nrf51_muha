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
 * @file    cfg_drv_timer.h
 * @author  mario.kodba
 * @brief   Configuration for TIMER peripheral header file.
 **************************************************************************************************/

#ifndef CFG_DRV_TIMER_H_
#define CFG_DRV_TIMER_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "drv_timer.h"

/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
extern DRV_TIMER_config_S configTimer1;         //!< Configuration structure of TIMER 1 instance
extern DRV_TIMER_instance_S instanceTimer1;     //!< Instance structure of TIMER 1

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/

#endif // #ifndef CFG_DRV_TIMER_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
