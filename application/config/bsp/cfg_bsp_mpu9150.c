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
 * @file    cfg_bsp_mpu9150.c
 * @author  mario.kodba
 * @brief   Configuration for MPU9150 device source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "bsp_mpu9150.h"
#include "cfg_drv_nrf_twi.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define BSP_MPU9150_I2C_ADDRESS         (0b1101000u)    //!< MPU-9150 device I2C address
#define BSP_MPU9150_MAG_I2C_ADDRESS     (0x0Cu)         //!< MPU-9150 magnetometer device I2C address

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
//! MPU-9150 device assignment structure
BSP_MPU9150_device_S mpuDevice = {
        .twiInstance = &instanceTwi1,
        .isInitialized = false,
        .dataReady = false
};

//! MPU-9150 configuration structure
BSP_MPU9150_config_S mpuDeviceConfig = {
        .mpuAddress = BSP_MPU9150_I2C_ADDRESS,
        .mpuMagAddress = BSP_MPU9150_MAG_I2C_ADDRESS,
        .gyroRange = BSP_MPU9150_gyroFsRange_2000degS,
        .accRange = BSP_MPU9150_accFsRange_16G
};

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
