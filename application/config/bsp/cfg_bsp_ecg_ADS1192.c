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
 * @file    cfg_bsp_ecg_ADS1192.c
 * @author  mario.kodba
 * @brief   Configuration for ECG ADS1192 device source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "bsp_ecg_ADS1192.h"
#include "cfg_drv_spi.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
//! ADS1192 device assignment structure
BSP_ECG_ADS1192_device_S ecgDevice;
//! ADS1192 configuration structure
BSP_ECG_ADS1192_config_S ecgDeviceConfig = {
        .spiInstance = &instanceSpi0,
        .spiConfig = &configSpi0,

        .samplingRate = BSP_ECG_ADS1192_convRate_500_SPS,
        .pgaSetting = BSP_ECG_ADS1192_pga_6X
};

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
