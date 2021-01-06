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
 * @file    hal_spi.h
 * @author  mario.kodba
 * @brief   HAL layer for SPI module functionality header file.
 **************************************************************************************************/

#ifndef HAL_SPI_H_
#define HAL_SPI_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf51.h"
#include "drv_spi.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void HAL_SPI_setPins(NRF_SPI_Type *spiStruct, uint32_t sckPin, uint32_t mosiPin, uint32_t misoPin);
void HAL_SPI_setFrequency(NRF_SPI_Type *spiStruct, DRV_SPI_freq_E frequency);
void HAL_SPI_setSpiConfiguration(NRF_SPI_Type *spiStruct,
        DRV_SPI_mode_E mode,
        DRV_SPI_bitOrder_E bitOrder);
void HAL_SPI_interruptEnable(NRF_SPI_Type *spiStruct);
void HAL_SPI_enableSpi(NRF_SPI_Type *spiStruct);

#endif // #ifndef HAL_SPI_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
