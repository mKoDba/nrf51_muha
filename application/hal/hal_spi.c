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
 * @file    hal_spi.c
 * @author  mario.kodba
 * @brief   HAL layer for SPI module functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "hal_spi.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define HAL_SPI_CPOL_ACTIVE_HIGH        (0UL)               //!< SPI CPOL active high
#define HAL_SPI_CPOL_ACTIVE_LOW         (1UL)               //!< SPI CPOL active low

#define HAL_SPI_CPHA_LEADING            (0UL)               //!< SPI CPHA on leading
#define HAL_SPI_CPHA_TRAILING           (1UL)               //!< SPI CPHA on trailing

#define HAL_SPI_INTERRUPT_ENABLE        (0x1UL << 2UL)      //!< SPI interrupt enable
#define HAL_SPI_INSTANCE_ENABLE         (0x01UL)            //!< SPI instance enable

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/


/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Sets SPI's pin registers to correct values.
 ***************************************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 * @param [in]   sckPin         - SCK pin number.
 * @param [in]   mosiPin        - MOSI pin number.
 * @param [in]   misoPin        - MISO pin number.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 **************************************************************************************************/
void HAL_SPI_setPins(NRF_SPI_Type *spiStruct, uint32_t sckPin, uint32_t mosiPin, uint32_t misoPin) {

    spiStruct->PSELSCK  = sckPin;
    spiStruct->PSELMOSI = mosiPin;
    spiStruct->PSELMISO = misoPin;
}

/***********************************************************************************************//**
 * @brief Sets SPI's frequency register to configured value.
 ***************************************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 * @param [in]   frequency      - wanted SPI frequency.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 **************************************************************************************************/
void HAL_SPI_setFrequency(NRF_SPI_Type *spiStruct, DRV_SPI_freq_E frequency) {

    spiStruct->FREQUENCY = frequency;
}

/***********************************************************************************************//**
 * @brief Sets SPI's configuration.
 ***************************************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 * @param [in]   mode           - wanted SPI mode.
 * @param [in]   bitOrder       - wanted SPI bit order.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 **************************************************************************************************/
void HAL_SPI_setSpiConfiguration(NRF_SPI_Type *spiStruct,
        DRV_SPI_mode_E mode,
        DRV_SPI_bitOrder_E bitOrder) {

    uint32_t config = (uint32_t) bitOrder;

    switch (mode) {
    default:
    case DRV_SPI_mode_0:
        config |= (HAL_SPI_CPOL_ACTIVE_HIGH << 2UL) |
                  (HAL_SPI_CPHA_LEADING << 1UL);
        break;
    case DRV_SPI_mode_1:
        config |= (HAL_SPI_CPOL_ACTIVE_HIGH << 2UL) |
                  (HAL_SPI_CPHA_TRAILING << 1UL);
        break;
    case DRV_SPI_mode_2:
        config |= (HAL_SPI_CPOL_ACTIVE_LOW << 2UL) |
                  (HAL_SPI_CPHA_LEADING << 1UL);
        break;
    case DRV_SPI_mode_3:
        config |= (HAL_SPI_CPOL_ACTIVE_LOW << 2UL) |
                  (HAL_SPI_CPHA_TRAILING << 1UL);
        break;
    }

    spiStruct->CONFIG = config;
}

/***********************************************************************************************//**
 * @brief Enables SPI interrupt on READY event.
 ***************************************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 **************************************************************************************************/
void HAL_SPI_interruptEnable(NRF_SPI_Type *spiStruct) {

    spiStruct->INTENSET = HAL_SPI_INTERRUPT_ENABLE;
}

/***********************************************************************************************//**
 * @brief Enables SPI instance.
 ***************************************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    29.12.2020.
 **************************************************************************************************/
void HAL_SPI_enableSpi(NRF_SPI_Type *spiStruct) {

    spiStruct->ENABLE = HAL_SPI_INSTANCE_ENABLE;
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/


/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
