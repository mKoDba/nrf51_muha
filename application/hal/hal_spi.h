/*
 * hal_spi.h
 *
 *  Created on: 27.12.2020.
 *  Author: mario.kodba
 */

#ifndef HAL_SPI_H_
#define HAL_SPI_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf51.h"
#include "drv_spi.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/

/*******************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void HAL_SPI_setPins(NRF_SPI_Type *spiStruct,
        uint32_t sckPin,
        uint32_t mosiPin,
        uint32_t misoPin);
void HAL_SPI_setFrequency(NRF_SPI_Type *spiStruct, DRV_SPI_freq_E frequency);
void HAL_SPI_setSpiConfiguration(NRF_SPI_Type *spiStruct,
        DRV_SPI_mode_E mode,
        DRV_SPI_bitOrder_E bitOrder);
void HAL_SPI_interruptEnable(NRF_SPI_Type *spiStruct);
void HAL_SPI_enableSpi(NRF_SPI_Type *spiStruct);

#endif // #ifndef HAL_SPI_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
