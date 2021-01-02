/*
 * drv_spi.h
 *
 *  Created on: 26.12.2020.
 *  Author: mario.kodba
 */

#ifndef DRV_SPI_H_
#define DRV_SPI_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "sdk_common.h"
#include "sdk_config.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

#include "drv_common.h"

#include <stdint.h>
/*******************************************************************************
 *                              ENUMERATIONS
 ******************************************************************************/
typedef enum DRV_SPI_err_ENUM {
    DRV_SPI_err_NONE       = 0u,                        //!< SPI no error.
    DRV_SPI_err_NULL_PARAM                              //!< SPI null parameter error.

} DRV_SPI_err_E;

typedef enum DRV_SPI_freq_ENUM {
    DRV_SPI_freq_125K = SPI_FREQUENCY_FREQUENCY_K125,   //!< 125 kbps.
    DRV_SPI_freq_250K = SPI_FREQUENCY_FREQUENCY_K250,   //!< 250 kbps.
    DRV_SPI_freq_500K = SPI_FREQUENCY_FREQUENCY_K500,   //!< 500 kbps.
    DRV_SPI_freq_1M   = SPI_FREQUENCY_FREQUENCY_M1,     //!< 1 Mbps.
    DRV_SPI_freq_2M   = SPI_FREQUENCY_FREQUENCY_M2,     //!< 2 Mbps.
    DRV_SPI_freq_4M   = SPI_FREQUENCY_FREQUENCY_M4,     //!< 4 Mbps.
    DRV_SPI_freq_8M   = (int)SPI_FREQUENCY_FREQUENCY_M8 //!< 8 Mbps.
} DRV_SPI_freq_E;

typedef enum DRV_SPI_mode_ENUM {
    DRV_SPI_mode_0 = 0u,                                //!< SCK active high, sample on leading edge of clock.
    DRV_SPI_mode_1,                                     //!< SCK active high, sample on trailing edge of clock.
    DRV_SPI_mode_2,                                     //!< SCK active low, sample on leading edge of clock.
    DRV_SPI_mode_3                                      //!< SCK active low, sample on trailing edge of clock.
} DRV_SPI_mode_E;

typedef enum DRV_SPI_bitOrder_ENUM {
    DRV_SPI_bitOrder_MSB_FIRST = 0u,                    //!< Most significant bit shifted out first.
    DRV_SPI_bitOrder_LSB_FIRST                          //!< Least significant bit shifted out first.
} DRV_SPI_bitOrder_E;

typedef enum DRV_SPI_event_ENUM {
    DRV_SPI_event_DONE = 0u                             //!< SPI event DONE.
} DRV_SPI_event_E;

typedef enum DRV_SPI_id_ENUM {
    DRV_SPI_id_0 = 0u,                                  //!< SPI0 instance ID.
    DRV_SPI_id_1,                                       //!< SPI1 instance ID.

    DRV_SPI_id_COUNT                                    //!< SPI instances count.
} DRV_SPI_id_E;

/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/
typedef void (*DRV_SPI_IRQHandler)(DRV_SPI_event_E *event, void *context);

typedef struct DRV_SPI_control_block_STRUCT {
    DRV_SPI_IRQHandler callbackFunction;                //!< Callback function for SPIx interrupt.
    void *context;                                      //!< Context passed to SPI callback function.

} DRV_SPI_control_block_S;

typedef struct DRV_SPI_config_STRUCT {
    uint8_t id;                                         //!< SPI instance ID.
    uint32_t sckPin;                                    //!< SCK pin number.
    uint32_t mosiPin;                                   //!< MOSI pin number.
    uint32_t misoPin;                                   //!< MISO pin number.
    uint32_t ssPin;                                     //!< Slave Select pin number.
    uint8_t irqPriority;                                //!< Interrupt priority.
    uint8_t orc;                                        //!< Over-run character.
                                                        /*!< This character is used when all bytes from the TX buffer are sent,
                                                             but the transfer continues due to RX. */
    DRV_SPI_freq_E frequency;                           //!< SPI frequency.
    DRV_SPI_mode_E mode;                                //!< SPI mode.
    DRV_SPI_bitOrder_E bitOrder;                        //!< SPI bit order.
    void *context;                                      //!< Context passed to interrupt handler (callback function).
} DRV_SPI_config_S;

typedef struct DRV_SPI_instance_STRUCT {
    DRV_SPI_config_S *config;                           //!< Pointer to SPI configuration structure.
    void *spiStruct;                                    //!< SPI structure with needed registers.
    IRQn_Type irq;                                      //!< SPI peripheral instance IRQ number.

    bool isInitialized;                                 //!< is SPI instance initialized.
} DRV_SPI_instance_S;


/*******************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void DRV_SPI_init(DRV_SPI_instance_S *spiInstance,
        DRV_SPI_config_S *spiConfig,
        DRV_SPI_IRQHandler irqHandler,
        DRV_SPI_err_E *outErr);
void DRV_SPI_masterTxRxBlocking(const DRV_SPI_instance_S *spiInstance,
        const uint8_t *inTxData,
        uint16_t inSize,
        uint8_t *outRxData,
        DRV_SPI_err_E *outErr);
void DRV_SPI_masterTxBlocking(const DRV_SPI_instance_S *spiInstance,
        const uint8_t *inTxData,
        uint16_t inSize,
        DRV_SPI_err_E *outErr);
void DRV_SPI_masterRxBlocking(const DRV_SPI_instance_S *spiInstance,
        uint16_t inSize,
        uint8_t *outRxData,
        DRV_SPI_err_E *outErr);

#endif // #ifndef DRV_SPI_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
