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
 * @file    cfg_drv_spi.c
 * @author  mario.kodba
 * @brief   Configuration for SPI peripheral source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf51.h"
#include "cfg_drv_spi.h"
#include "cfg_nrf51_muha_pinout.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
//! SPI 0 instance configuration structure
DRV_SPI_config_S configSpi0 = {
        .id = DRV_SPI_id_0,
        .sckPin = ECG_CLK,
        .mosiPin = ECG_DIN,
        .misoPin = ECG_DOUT,
        .ssPin = ECG_CS,
        .irqPriority = 3u,
        .orc = 0x00u,
        .frequency = DRV_SPI_freq_1M,
        .mode = DRV_SPI_mode_1,
        .bitOrder = DRV_SPI_bitOrder_MSB_FIRST
};

//! SPI 0 instance assignment structure
DRV_SPI_instance_S instanceSpi0 = {
        .spiStruct          = NRF_SPI0,             //!< SPI peripheral registers
        .irq                = SPI0_TWI0_IRQn,       //!< SPI instance IRQ number
        .isInitialized      = false
};

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
