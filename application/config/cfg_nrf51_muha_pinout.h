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
 * @file    cfg_nrf51_muha_pinout.h
 * @author  mario.kodba
 * @brief   Pin configuration for NRF51 MUHA board header file.
 **************************************************************************************************/

#ifndef CFG_NRF51_MUHA_PINOUT_H_
#define CFG_NRF51_MUHA_PINOUT_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/

/***************************************************************************************************
 *                                  CONSTANTS
 **************************************************************************************************/
//! NRF51422-QFAC "MUHA" pin number and its function
#define ECG_RST         (1u)    //!< ECG Reset pin
#define ECG_CS          (3u)    //!< ECG Chip Select pin
#define ECG_DIN         (4u)    //!< ECG Data Input (MOSI) pin
#define ECG_CLK         (5u)    //!< ECG Clock signal pin
#define ECG_DOUT        (6u)    //!< ECG Data Output (MISO) pin
#define ECG_DRDY        (8u)    //!< ECG Data Ready pin
#define VIBRO_MOTOR     (7u)    //!< Vibro Motor pin
#define SD_CD           (9u)    //!< SD CD
#define LD1             (10u)   //!< LED 1 pin
#define SDA             (11u)   //!< SDA TWI pin
#define SCL             (12u)   //!< SCL TWI pin
#define MPU_INT         (15u)   //!< MPU INT
#define MPU_FSYNC       (16u)   //!< MPU FSYNC
#define SD_MOSI         (17u)   //!< SD card MOSI pin
#define SD_CS           (19u)   //!< SD card Chip Select pin
#define SD_CLK          (25u)   //!< SD card Clock signal pin
#define SD_MISO         (29u)   //!< SD card MISO pin

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/

#endif // #ifndef CFG_NRF51_MUHA_PINOUT_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
