/*  header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "nrf51.h"
#include "cfg_drv_spi.h"

#include "cfg_nrf51_muha_pinout.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/
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

DRV_SPI_instance_S instanceSpi0 = {
        .spiStruct          = NRF_SPI0,             //!< SPI peripheral registers
        .irq                = SPI0_TWI0_IRQn,       //!< SPI instance IRQ number
        .isInitialized      = false
};

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
