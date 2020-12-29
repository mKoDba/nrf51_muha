/*  header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "hal_spi.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/
#define HAL_SPI_CPOL_ACTIVE_HIGH        (0UL)
#define HAL_SPI_CPOL_ACTIVE_LOW         (1UL)

#define HAL_SPI_CPHA_LEADING            (0UL)
#define HAL_SPI_CPHA_TRAILING           (1UL)

#define HAL_SPI_INTERRUPT_ENABLE        (4UL)
#define HAL_SPI_INSTANCE_ENABLE         (1UL)

/*******************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/


/*******************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Sets SPI's pin registers to correct values.
 ******************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 * @param [in]   sckPin         - SCK pin number.
 * @param [in]   mosiPin        - MOSI pin number.
 * @param [in]   misoPin        - MISO pin number.
 ******************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 ******************************************************************************/
void HAL_SPI_setPins(NRF_SPI_Type *spiStruct,
        uint32_t sckPin,
        uint32_t mosiPin,
        uint32_t misoPin) {

    spiStruct->PSELSCK  = sckPin;
    spiStruct->PSELMOSI = mosiPin;
    spiStruct->PSELMISO = misoPin;
}

/*******************************************************************************
 * @brief Sets SPI's frequency register to configured value.
 ******************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 * @param [in]   frequency      - wanted SPI frequency.
 ******************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 ******************************************************************************/
void HAL_SPI_setFrequency(NRF_SPI_Type *spiStruct, DRV_SPI_freq_E frequency) {

    spiStruct->FREQUENCY = frequency;
}

/*******************************************************************************
 * @brief Sets SPI's configuration.
 ******************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 * @param [in]   mode           - wanted SPI mode.
 * @param [in]   bitOrder       - wanted SPI bit order.
 ******************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 ******************************************************************************/
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

/*******************************************************************************
 * @brief Enables SPI interrupt on READY event.
 ******************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 ******************************************************************************
 * @author  mario.kodba
 * @date    27.12.2020.
 ******************************************************************************/
void HAL_SPI_interruptEnable(NRF_SPI_Type *spiStruct) {

    spiStruct->INTENSET = HAL_SPI_INTERRUPT_ENABLE;
}

/*******************************************************************************
 * @brief Enables SPI instance.
 ******************************************************************************
 * @param [in]   *spiStruct     - pointer to SPI registers structure.
 ******************************************************************************
 * @author  mario.kodba
 * @date    29.12.2020.
 ******************************************************************************/
void HAL_SPI_enableSpi(NRF_SPI_Type *spiStruct) {

    spiStruct->ENABLE = HAL_SPI_INSTANCE_ENABLE;
}

/*******************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/


/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
