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
 * @file    drv_spi.c
 * @author  mario.kodba
 * @brief   NRF51 SPI module functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "drv_spi.h"
#include "hal_spi.h"

#include "nrf_gpio.h"
/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                           GLOBAL VARIABLES
 **************************************************************************************************/
static DRV_SPI_control_block_S DRV_SPI_controlBlock[DRV_SPI_id_COUNT];

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void DRV_SPI_initPins(DRV_SPI_instance_S *spiInstance);

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Initializes SPI instance.
 ***************************************************************************************************
 * @param [in]   *spiInstance   - pointer to SPI instance structure.
 * @param [in]   *spiConfig     - pointer to SPI configuration structure.
 * @param [in]   irqHandler     - function pointer to user defined IRQ handler.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    26.12.2020.
 **************************************************************************************************/
void DRV_SPI_init(DRV_SPI_instance_S *spiInstance,
        DRV_SPI_config_S *spiConfig,
        DRV_SPI_IRQHandler irqHandler,
        DRV_SPI_err_E *outErr) {

    DRV_SPI_err_E err = DRV_SPI_err_NONE;

    if(spiInstance != NULL && spiConfig != NULL) {
        // set SPI configuration
        spiInstance->config = spiConfig;
        // set callback function and context, if given
        DRV_SPI_control_block_S *block = &DRV_SPI_controlBlock[spiInstance->config->id];
        block->callbackFunction = irqHandler;
        block->context = spiInstance->config->context;
        // SPI pins setup
        DRV_SPI_initPins(spiInstance);

        // configure actual SPI instance registers
        NRF_SPI_Type *spi = (NRF_SPI_Type *) spiInstance->spiStruct;

        HAL_SPI_setPins(spi,
                spiInstance->config->sckPin,
                spiInstance->config->mosiPin,
                spiInstance->config->misoPin);
        HAL_SPI_setFrequency(spi, spiInstance->config->frequency);
        HAL_SPI_setSpiConfiguration(spi,
                spiInstance->config->mode,
                spiInstance->config->bitOrder);

        if(irqHandler) {
            // enable interrupt on READY event
            HAL_SPI_interruptEnable(spi);
        }
        // enable SPI instance
        HAL_SPI_enableSpi(spi);

        if(irqHandler) {
            DRV_COMMON_enableIRQPriority(&spiInstance->irq, spiInstance->config->irqPriority);
        }
        if(err == DRV_SPI_err_NONE) {
            spiInstance->isInitialized = true;
        }
    } else {
        err = DRV_SPI_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Function starts SPI transfer, both shifting out data and
 *        reading received data from slave device.
 ***************************************************************************************************
 * @param [in]   *spiInstance   - pointer to SPI instance structure.
 * @param [in]   *inTxData      - pointer to TX input data.
 * @param [in]   inSize         - size in bytes of RX/TX data.
 * @param [out]  *outRxData     - pointer to RX output data.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 **************************************************************************************************/
void DRV_SPI_masterTxRxBlocking(const DRV_SPI_instance_S *spiInstance,
        const uint8_t *inTxData,
        uint16_t inSize,
        uint8_t *outRxData,
        DRV_SPI_err_E *outErr) {

    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;
    volatile uint32_t *SPI_DATA_READY;
    uint32_t tmp;

    if(spiInstance != NULL || inTxData != NULL || outRxData != NULL) {
        // which SPI instance to take
        NRF_SPI_Type *SPI = spiInstance->spiStruct;

        SPI_DATA_READY = &SPI->EVENTS_READY;

        // enable slave (slave select active low)
        nrf_gpio_pin_clear(spiInstance->config->ssPin);

        // clear the event to be ready to receive next messages
        *SPI_DATA_READY = 0;

        // send first byte
        SPI->TXD = (uint32_t)*inTxData++;
        tmp = (uint32_t)*inTxData++;

        while(--inSize) {
            SPI->TXD = tmp;
            tmp = (uint32_t)*inTxData++;
            // wait for the transaction complete or timeout (about 10ms - 20 ms)
            while (*SPI_DATA_READY == 0);
            // clear the event to be ready to receive next messages
            *SPI_DATA_READY = 0;
            // get RX byte from slave
            *outRxData++ = SPI->RXD;
        }
        // wait for the transaction complete or timeout (about 10ms - 20 ms)
        while (*SPI_DATA_READY == 0);
        // get last byte from slave
        *outRxData = SPI->RXD;

        // disable slave (slave select active low)
        nrf_gpio_pin_set(spiInstance->config->ssPin);
    } else {
        spiErr = DRV_SPI_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = spiErr;
    }
}

/***********************************************************************************************//**
 * @brief Function starts SPI transfer, shifts out data to slave device.
 ***************************************************************************************************
 * @param [in]   *spiInstance   - pointer to SPI instance structure.
 * @param [in]   *inTxData      - pointer to TX input data.
 * @param [in]   inSize         - size in bytes of TX data.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 **************************************************************************************************/
void DRV_SPI_masterTxBlocking(const DRV_SPI_instance_S *spiInstance,
        const uint8_t *inTxData,
        uint16_t inSize,
        DRV_SPI_err_E *outErr) {

    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;
    // bytes received are being ignored
    volatile uint32_t dummyRead;

    if(spiInstance != NULL || inTxData != NULL) {
        // which SPI instance to take
        NRF_SPI_Type *SPI = spiInstance->spiStruct;

        // enable slave (slave select active low)
        nrf_gpio_pin_clear(spiInstance->config->ssPin);

        // clear the event to be ready to receive next messages
        SPI->EVENTS_READY = 0u;
        SPI->TXD = (uint32_t)*inTxData++;

        while(--inSize) {
            SPI->TXD =  (uint32_t)*inTxData++;
            // wait for the transaction complete or timeout (about 10ms - 20 ms)
            while (SPI->EVENTS_READY == 0);
            // clear the event to be ready to receive next messages
            SPI->EVENTS_READY = 0;
            // read in byte RX
            dummyRead = SPI->RXD;
        }
        // wait for the final transaction to complete or timeout (about 10ms - 20 ms)
        while (SPI->EVENTS_READY == 0);
        // read in last byte
        dummyRead = SPI->RXD;

        // disable slave (set SS to high)
        nrf_gpio_pin_set(spiInstance->config->ssPin);
    } else {
        spiErr = DRV_SPI_err_NULL_PARAM;
    }

    if(outErr != NULL) {
       *outErr = spiErr;
    }
}

/***********************************************************************************************//**
 * @brief Function starts SPI transfer, shifts out data from device into master.
 ***************************************************************************************************
 * @param [in]   *spiInstance   - pointer to SPI instance structure.
 * @param [in]   inSize         - size in bytes of RX data.
 * @param [out]  *outRxData     - pointer to RX output data.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 **************************************************************************************************/
void DRV_SPI_masterRxBlocking(const DRV_SPI_instance_S *spiInstance,
        uint16_t inSize,
        uint8_t *outRxData,
        DRV_SPI_err_E *outErr) {

    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;

    if(spiInstance != NULL || outRxData != NULL) {
        // which SPI instance to take
        NRF_SPI_Type *SPI = spiInstance->spiStruct;

        // enable slave (slave select active low)
        nrf_gpio_pin_clear(spiInstance->config->ssPin);

        SPI->EVENTS_READY = 0;
        // send dummy zeros
        SPI->TXD = 0;

        while(--inSize) {
            SPI->TXD = 0;
            // wait for the transaction complete or timeout (about 10ms - 20 ms)
            while(SPI->EVENTS_READY == 0);
            // clear the event to be ready to receive next messages
            SPI->EVENTS_READY = 0;

            *outRxData++ = SPI->RXD;
        }
        // wait for the transaction complete or timeout (about 10ms - 20 ms)
        while (SPI->EVENTS_READY == 0);
        // read in last byte
        *outRxData = SPI->RXD;

        // disable slave (slave select active low)
        nrf_gpio_pin_set(spiInstance->config->ssPin);
    } else {
        spiErr = DRV_SPI_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = spiErr;
    }
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Initializes pins for given SPI instance.
 ***************************************************************************************************
 * @param [in]   *spiInstance   - pointer to SPI instance structure.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    26.12.2020.
 **************************************************************************************************/
static void DRV_SPI_initPins(DRV_SPI_instance_S *spiInstance) {

    // setup SCK pin
    if (spiInstance->config->mode <= DRV_SPI_mode_1) {
        // SCK active high
        nrf_gpio_pin_clear(spiInstance->config->sckPin);
    } else {
        // SCK active low
        nrf_gpio_pin_set(spiInstance->config->sckPin);
    }
    NRF_GPIO->PIN_CNF[spiInstance->config->sckPin] =
        (GPIO_PIN_CNF_DIR_Output        << GPIO_PIN_CNF_DIR_Pos)
      | (GPIO_PIN_CNF_INPUT_Connect     << GPIO_PIN_CNF_INPUT_Pos)
      | (GPIO_PIN_CNF_PULL_Disabled     << GPIO_PIN_CNF_PULL_Pos)
      | (GPIO_PIN_CNF_DRIVE_S0S1        << GPIO_PIN_CNF_DRIVE_Pos)
      | (GPIO_PIN_CNF_SENSE_Disabled    << GPIO_PIN_CNF_SENSE_Pos);

    // setup MOSI pin
    nrf_gpio_pin_clear(spiInstance->config->mosiPin);
    nrf_gpio_cfg_output(spiInstance->config->mosiPin);

    // setup MISO pin
    nrf_gpio_cfg_input(spiInstance->config->misoPin, NRF_GPIO_PIN_NOPULL);

    // set SS to inactive
    nrf_gpio_pin_set(spiInstance->config->ssPin);
    nrf_gpio_cfg_output(spiInstance->config->ssPin);
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
