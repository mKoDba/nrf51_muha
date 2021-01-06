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
 * @file    cfg_drv_timer.c
 * @author  mario.kodba
 * @brief   Configuration for TIMER peripheral source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "cfg_drv_timer.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
DRV_TIMER_instance_S instanceTimer1;

DRV_TIMER_config_S configTimer1 = {
        .timerReg = NRF_TIMER1,
        .id = DRV_TIMER_id_1,
        .frequency = DRV_TIMER_freq_4MHz,
        .mode = DRV_TIMER_mode_NORMAL,
        .bitWidth = DRV_TIMER_bitWidth_16,
        .irqPriority = 3u
};

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
