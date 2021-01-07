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
 * @file    drv_spi.h
 * @author  mario.kodba
 * @brief   NRF51 SPI module functionality header file.
 **************************************************************************************************/

#ifndef DRV_SPI_H_
#define DRV_SPI_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "sdk_common.h"
#include "sdk_config.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

#include "drv_common.h"

#include <stdint.h>
/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/
//! SPI error enumeration
typedef enum DRV_SPI_err_ENUM {
    DRV_SPI_err_NONE       = 0u,                        //!< SPI no error.
    DRV_SPI_err_NULL_PARAM                              //!< SPI null parameter error.
} DRV_SPI_err_E;

//! SPI driver frequency enumeration
typedef enum DRV_SPI_freq_ENUM {
    DRV_SPI_freq_125K = SPI_FREQUENCY_FREQUENCY_K125,   //!< 125 kbps.
    DRV_SPI_freq_250K = SPI_FREQUENCY_FREQUENCY_K250,   //!< 250 kbps.
    DRV_SPI_freq_500K = SPI_FREQUENCY_FREQUENCY_K500,   //!< 500 kbps.
    DRV_SPI_freq_1M   = SPI_FREQUENCY_FREQUENCY_M1,     //!< 1 Mbps.
    DRV_SPI_freq_2M   = SPI_FREQUENCY_FREQUENCY_M2,     //!< 2 Mbps.
    DRV_SPI_freq_4M   = SPI_FREQUENCY_FREQUENCY_M4,     //!< 4 Mbps.
    DRV_SPI_freq_8M   = (int)SPI_FREQUENCY_FREQUENCY_M8 //!< 8 Mbps.
} DRV_SPI_freq_E;

//! SPI driver mode enumeration
typedef enum DRV_SPI_mode_ENUM {
    DRV_SPI_mode_0 = 0u,                                //!< SCK active high, sample on leading edge of clock.
    DRV_SPI_mode_1,                                     //!< SCK active high, sample on trailing edge of clock.
    DRV_SPI_mode_2,                                     //!< SCK active low, sample on leading edge of clock.
    DRV_SPI_mode_3                                      //!< SCK active low, sample on trailing edge of clock.
} DRV_SPI_mode_E;

//! SPI driver bit order enumeration
typedef enum DRV_SPI_bitOrder_ENUM {
    DRV_SPI_bitOrder_MSB_FIRST = 0u,                    //!< Most significant bit shifted out first.
    DRV_SPI_bitOrder_LSB_FIRST                          //!< Least significant bit shifted out first.
} DRV_SPI_bitOrder_E;

//! SPI driver event enumeration
typedef enum DRV_SPI_event_ENUM {
    DRV_SPI_event_DONE = 0u                             //!< SPI event DONE.
} DRV_SPI_event_E;

//! SPI driver ID enumeration
typedef enum DRV_SPI_id_ENUM {
    DRV_SPI_id_0 = 0u,                                  //!< SPI0 instance ID.
    DRV_SPI_id_1,                                       //!< SPI1 instance ID.

    DRV_SPI_id_COUNT                                    //!< SPI instances count.
} DRV_SPI_id_E;

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! SPI callback function pointer
typedef void (*DRV_SPI_IRQHandler)(DRV_SPI_event_E *event, void *context);

//! SPI control block structure
typedef struct DRV_SPI_control_block_STRUCT {
    DRV_SPI_IRQHandler callbackFunction;                //!< Callback function for SPIx interrupt.
    void *context;                                      //!< Context passed to SPI callback function.

} DRV_SPI_control_block_S;

//! SPI configuration structure
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

//! SPI instance structure
typedef struct DRV_SPI_instance_STRUCT {
    DRV_SPI_config_S *config;                           //!< Pointer to SPI configuration structure.
    void *spiStruct;                                    //!< SPI structure with needed registers.
    IRQn_Type irq;                                      //!< SPI peripheral instance IRQ number.

    bool isInitialized;                                 //!< is SPI instance initialized.
} DRV_SPI_instance_S;

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
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
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
