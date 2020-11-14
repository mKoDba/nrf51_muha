/*  header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "cfg_nrf_drv_spi.h"
#include "cfg_nrf51_muha_pinout.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/
nrf_drv_spi_t spi0Instance = {
        .p_registers        = NRF_SPI0,             //!< SPI peripheral registers
        .irq                = SPI0_IRQ,             //!< SPI instance IRQ number
        .drv_inst_idx       = SPI0_INSTANCE_INDEX,  //!< SPI instance index
        .use_easy_dma       = false,                //!< use EasyDMA
};

nrf_drv_spi_config_t spi0Config = {
        .sck_pin        = ECG_CLK,      //!< SCK pin number.
        .mosi_pin       = ECG_DIN,      //!< MOSI pin number (optional).
        .miso_pin       = ECG_DOUT,     //!< MISO pin number (optional).
        .ss_pin         = ECG_CS,       //!< Slave Select pin number (optional).
        .irq_priority   = SPI_DEFAULT_CONFIG_IRQ_PRIORITY, //!< Interrupt priority.
        .orc            = 0xFF,         //!< Over-run character.
                        /*!< This character is used when all bytes from the TX buffer are sent,
                             but the transfer continues due to RX. */
        .frequency      = NRF_DRV_SPI_FREQ_1M,           //!< SPI frequency.
        .mode           = NRF_DRV_SPI_MODE_1,              //!< SPI mode.
        .bit_order      = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST, //!< SPI bit order.
};

/*******************************************************************************
 *                         PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
void spiEventHandler(nrf_drv_spi_evt_t const * p_event) {
    //spi_xfer_done = true;
    //if (m_rx_buf[0] != 0) {
    //    NRF_LOG_INFO(" Received: \r\n");
    //   NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    //}
}

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
