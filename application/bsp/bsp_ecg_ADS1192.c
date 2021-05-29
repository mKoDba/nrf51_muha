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
 * @file    bsp_ecg_ADS1192.c
 * @author  mario.kodba
 * @brief   ADS1192 ECG device functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stddef.h>

#include "SEGGER_RTT.h"
#include "drv_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "cfg_drv_timer.h"
#include "cfg_drv_spi.h"
#include "cfg_nrf51_muha_pinout.h"
#include "cfg_bsp_ecg_ADS1192.h"
#include "bsp_ecg_ADS1192.h"

#include "ble_ecgs.h"
#include "ble_bas.h"
#include "ble_gatts.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
// ADC constants
#define BSP_ECG_ADS1192_ADC_REF_VOLTAGE         (2.42f) //!< ADC reference voltage
#define BSP_ECG_ADS1192_ADC_MAX_VALUE           (32767) //!< Maximal ADC value (2^15 - 1)

// ECG ADS1192 SPI commands
#define BSP_ECG_ADS1192_SPI_WAKEUP              (0x02u) //!< Wake-up from standby mode
#define BSP_ECG_ADS1192_SPI_STANDBY             (0x04u) //!< Enter standby mode
#define BSP_ECG_ADS1192_SPI_RESET               (0x06u) //!< Reset the device
#define BSP_ECG_ADS1192_SPI_START               (0x08u) //!< Start conversions
#define BSP_ECG_ADS1192_SPI_STOP                (0x0Au) //!< Stop conversion
#define BSP_ECG_ADS1192_SPI_OFFSETCAL           (0x1Au) //!< Channel offset calibration
#define BSP_ECG_ADS1192_SPI_RDATAC              (0x10u) //!< Read Data Continuous mode
#define BSP_ECG_ADS1192_SPI_SDATAC              (0x11u) //!< Stop Read Data Continuous
#define BSP_ECG_ADS1192_SPI_RDATA               (0x12u) //!< Read data by command

// ECG ADS1192 SPI constants
#define BSP_ECG_ADS1192_SPI_READ_CMD            (0x20u) //!< SPI read command
#define BSP_ECG_ADS1192_SPI_WRITE_CMD           (0x40u) //!< SPI write command
#define BSP_ECG_ADS1192_SPI_SIZE_READ           (2u)    //!< SPI Read command size (bytes)
#define BSP_ECG_ADS1192_SPI_SIZE_WRITE          (3u)    //!< SPI Write command size (bytes)
#define BSP_ECG_ADS1192_SPI_SIZE_SINGLE_BYTE    (1u)    //!< Single SPI byte size
#define BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME   (6u)    //!< Single SPI read - consists of 6 bytes
#define BSP_ECG_ADS1192_SPI_MSG_MAX_SIZE        (14u)   //!< Maximum SPI message size
#define BSP_ECG_ADS1192_SPI_SINGLE_REG          (0x00u) //!< Single register read operation
#define BSP_ECG_ADS1192_SPI_READ_OFFSET         (1u)    //!< SPI read RX offset
#define BSP_ECG_ADS1192_SPI_REG_VALUE_INDEX     (2u)    //!< SPI register value index


// ECG ADS1192 Time constants
#define BSP_ECG_ADS1192_WAIT_TIME_8_US          (8u)    //!< 8 us wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK   (8u)    //!< 4 tCLK time period (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_18_TCLK  (36u)   //!< 18 tCLK time period (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_1_FMOD   (8u)    //!< 1 fMOD wait time (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_9_FMOD   (72u)   //!< 9 fMOD wait time (us)
#define BSP_ECG_ADS1192_INIT_OSC_WAIT_TIME_US   (32u)   //!< Internal oscillator wait time (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_1_MS     (1000u) //!< 1ms wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS   (200000u) //!< 200ms wait time
#define BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS  (310u)  //!< Offset calibration wait time (ms)

// ECG ADS1192 bit manipulation constants
#define BSP_ECG_ADS1192_LEAD_OFF_MASK           (0x0Fu) //!< Channel 1 and 2 Lead-off detection
#define BSP_ECG_ADS1192_BYTE_SHIFT              (8u)    //!< Byte shift value

// ECG ADS1192 temperature constants
#define BSP_ECG_ADS1192_TEMP_CONST_1            (168)   //!< Temperature constant 1
#define BSP_ECG_ADS1192_TEMP_CONST_2            (394)   //!< Temperature constant 2
#define BSP_ECG_ADS1192_TEMP_CONST_3            (25)    //!< Temperature constant 3
#define BSP_ECG_ADS1192_TEMP_BIT_MASK           (8u)    //!< Temperature bit mask

// ECG ADS1192 register values
#define BSP_ECG_ADS1192_LEAD_OFF_BOTH_CHANNELS  (0x0Fu) //!< Register value for lead-off detection
#define BSP_ECG_ADS1192_LEAD_OFF_THRESHOLD      (0x1Cu) //!< Threshold value for lead-off detection
#define BSP_ECG_ADS1192_RLD_OFF_BOTH_CHANNELS   (0x1Fu) //!< Register value for RLD-off detection
#define BSP_ECG_ADS1192_LEAD_OFF_COMP_ENABLE    (1u)    //!< Enable lead-off comparator
#define BSP_ECG_ADS1192_LEAD_OFF_COMP_DISABLE   (0u)    //!< Disable lead-off comparator
#define BSP_ECG_ADS1192_INT_REF_BUFF_ENABLE     (1u)    //!< Enable internal reference buffer
#define BSP_ECG_ADS1192_INT_REF_BUFF_DISABLE    (0u)    //!< Disable internal reference buffer
#define BSP_ECG_ADS1192_TEST_SIGNAL_ENABLE      (1u)    //!< Enable test signal
#define BSP_ECG_ADS1192_TEST_SIGNAL_DISABLE     (0u)    //!< Disable test signal
#define BSP_ECG_ADS1192_TEST_SIGNAL_DC          (0u)    //!< Set test signal to DC
#define BSP_ECG_ADS1192_TEST_SIGNAL_1HZ         (1u)    //!< Set test signal to 1Hz

// ECG ADS1192 register addresses
#define BSP_ECG_ADS1192_REG_ADDR_ID             (0x0u)  //!< ID Control register
// global settings across channels
#define BSP_ECG_ADS1192_REG_ADDR_CONFIG_1       (0x1u)  //!< Configuration register 1
#define BSP_ECG_ADS1192_REG_ADDR_CONFIG_2       (0x2u)  //!< Configuration register 2
#define BSP_ECG_ADS1192_REG_ADDR_LOFF           (0x3u)  //!< Lead-off Control register
// channel specific settings
#define BSP_ECG_ADS1192_REG_ADDR_CH1_SET        (0x4u)  //!< Channel 1 settings
#define BSP_ECG_ADS1192_REG_ADDR_CH2_SET        (0x5u)  //!< Channel 2 settings
#define BSP_ECG_ADS1192_REG_ADDR_RLD_SENS       (0x6u)  //!< RLD Sense selection
#define BSP_ECG_ADS1192_REG_ADDR_LOFF_SENS      (0x7u)  //!< Lead-off Sense selection
#define BSP_ECG_ADS1192_REG_ADDR_LOFF_STAT      (0x8u)  //!< Lead-off Status
// GPIO and other settings
#define BSP_ECG_ADS1192_REG_ADDR_MISC_1         (0x9u)  //!< Miscellaneous Control register 1
#define BSP_ECG_ADS1192_REG_ADDR_MISC_2         (0xAu)  //!< Miscellaneous Control register 2
#define BSP_ECG_ADS1192_REG_ADDR_GPIO           (0xBu)  //!< General-Purpose IO register

/***************************************************************************************************
 *                          GLOBAL VARIABLES
 **************************************************************************************************/
static const uint8_t ecgADS1192RegAddr[BSP_ECG_ADS1192_reg_COUNT] = {
        BSP_ECG_ADS1192_REG_ADDR_ID,
        BSP_ECG_ADS1192_REG_ADDR_CONFIG_1,
        BSP_ECG_ADS1192_REG_ADDR_CONFIG_2,
        BSP_ECG_ADS1192_REG_ADDR_LOFF,
        BSP_ECG_ADS1192_REG_ADDR_CH1_SET,
        BSP_ECG_ADS1192_REG_ADDR_CH2_SET,
        BSP_ECG_ADS1192_REG_ADDR_RLD_SENS,
        BSP_ECG_ADS1192_REG_ADDR_LOFF_SENS,
        BSP_ECG_ADS1192_REG_ADDR_LOFF_STAT,
        BSP_ECG_ADS1192_REG_ADDR_MISC_1,
        BSP_ECG_ADS1192_REG_ADDR_MISC_2,
        BSP_ECG_ADS1192_REG_ADDR_GPIO
};

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static __INLINE void BSP_ECG_ADS1192_convertSignalToSignedVal(const uint8_t *inData,
        int16_t *outData);
static void BSP_ECG_ADS1192_sendSpiCommand(BSP_ECG_ADS1192_device_S *inDevice,
        const uint8_t inSpiCmd,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_writeSingleReg(BSP_ECG_ADS1192_device_S *inDevice,
        const BSP_ECG_ADS1192_reg_E inStartReg,
        const uint8_t *inData,
        BSP_ECG_ADS1192_err_E *outErr);
static uint8_t BSP_ECG_ADS1192_readSingleReg(BSP_ECG_ADS1192_device_S *inDevice,
        const BSP_ECG_ADS1192_reg_E inReg,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_enablePgaCalibration(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_setConversionRate(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_convRate_E inSps,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_setNormalElectrodeRead(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readData(BSP_ECG_ADS1192_device_S *inDevice,
        const uint16_t inSize,
        uint8_t *outData,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_initReset(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
#if (DEBUG == true)
static void BSP_ECG_ADS1192_updateTemperature(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readSupplyMeasurement(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readTestSignal(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_detectLeadOff(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_detectRldOff(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
#endif // #if (DEBUG == true)

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function initializes ECG ADS1192 component.
 ***************************************************************************************************
 * @param [in]  *inDevice - pointer to device structure for ECG driver.
 * @param [in]  *inConfig - configuration structure for ECG driver.
 * @param [out] *outErr   - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 **************************************************************************************************/
void BSP_ECG_ADS1192_init(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_config_S *inConfig,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    if((inDevice != NULL) && (inConfig != NULL)) {
        inDevice->config = inConfig;
        inDevice->sampleIndex = 0u;
        inDevice->isInitialized = false;

        // internal oscillator start-up time
        nrf_delay_us(BSP_ECG_ADS1192_INIT_OSC_WAIT_TIME_US);

        // reset the device and its registers on start-up
        BSP_ECG_ADS1192_initReset(inDevice, &ecgErr);

        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            /* on power-up device defaults to Read Data Continous mode,
             * stop it to enable sending SPI commands */
            BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);
            // wait 4 * tCLK after sending SDATAC command
            nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);
        }

        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            // set internal reference voltage (2.42V)
            BSP_ECG_ADS1192_config2Reg_U conf2 = { .R = 0x80u };
            conf2.B.pdbRefBuf = BSP_ECG_ADS1192_INT_REF_BUFF_ENABLE;
            BSP_ECG_ADS1192_writeSingleReg(inDevice,
                    BSP_ECG_ADS1192_reg_CONFIG_2,
                    (uint8_t *) &conf2,
                    &ecgErr);
            // wait for reference voltage to settle
            nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);
        }

        // set SPS, continuous conversion
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_setConversionRate(inDevice, inDevice->config->samplingRate, &ecgErr);
        }

        // enable calibrating PGA
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_enablePgaCalibration(inDevice, &ecgErr);
        }

#if (DEBUG == true)
        // update temperature
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_updateTemperature(inDevice, &ecgErr);
        }

        // read device supply voltage
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_readSupplyMeasurement(inDevice, &ecgErr);
        }

        // internally generated test signal check
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_readTestSignal(inDevice, &ecgErr);
        }

        // check if any Lead is off
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_detectLeadOff(inDevice, &ecgErr);
        }

        // check if RLD is off
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_detectRldOff(inDevice, &ecgErr);
        }
#endif // #if (DEBUG == true)

        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_setNormalElectrodeRead(inDevice, &ecgErr);
        }

        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            inDevice->isInitialized = true;
        }
    } else {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Starts reading and outputting data received from channel inputs.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.12.2020
 **************************************************************************************************/
void BSP_ECG_ADS1192_startEcgReading(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // issue read continuous command
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATAC, &ecgErr);

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // enable ECG_DRDY pin interrupt
        nrf_drv_gpiote_in_event_enable(ECG_DRDY, true);
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Stops reading and outputting data from channels and disables DRDY
 *        interrupt.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.12.2020
 **************************************************************************************************/
void BSP_ECG_ADS1192_stopEcgReading(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // issue stop read continuous command
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // disable ECG_DRDY pin interrupt
        nrf_drv_gpiote_in_event_disable(ECG_DRDY);
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function for converting output signal into usable value.
 * @details Converts 8-bit output signal array into correct 16-bit
 *          twos-complement values (amplitudes).
 ***************************************************************************************************
 * @param [in]  *inData    - pointer to input data array.
 * @param [in]  inDataSize - input data array size.
 * @param [out] *outData   - pointer to output data array.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    05.12.2020
 **************************************************************************************************/
static __INLINE void BSP_ECG_ADS1192_convertSignalToSignedVal(const uint8_t *inData,
        int16_t *outData) {

    outData[0] = inData[1] | (inData[0] << BSP_ECG_ADS1192_BYTE_SHIFT);
    outData[1] = inData[2] | (inData[1] << BSP_ECG_ADS1192_BYTE_SHIFT);
    outData[2] = inData[3] | (inData[2] << BSP_ECG_ADS1192_BYTE_SHIFT);

//    SEGGER_RTT_printf(0, ",%d,%d,%d\n", outData[0], outData[1], outData[2]);
}

/***********************************************************************************************//**
 * @brief Function sends predefined SPI command to the device.
 ***************************************************************************************************
 * @param [in]  *inDevice - pointer to device structure for ECG driver.
 * @param [in]  inSpiCmd  - SPI command.
 * @param [out] *outErr   - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    24.10.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_sendSpiCommand(BSP_ECG_ADS1192_device_S *inDevice,
        const uint8_t inSpiCmd,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;

    if(inDevice == NULL) {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    } else {
        DRV_SPI_masterTxBlocking(inDevice->config->spiInstance,
                &inSpiCmd,
                BSP_ECG_ADS1192_SPI_SIZE_SINGLE_BYTE,
                &spiErr);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(spiErr != DRV_SPI_err_NONE) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Function for writing to single register of ADS1192.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [in]  inReg        - register address.
 * @param [in]  *inData      - pointer to data to be sent.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_writeSingleReg(BSP_ECG_ADS1192_device_S *inDevice,
        const BSP_ECG_ADS1192_reg_E inReg,
        const uint8_t *inData,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;
    uint8_t opcode[BSP_ECG_ADS1192_SPI_SIZE_WRITE] = { 0u };
    uint8_t regAddr = ecgADS1192RegAddr[inReg];
    uint8_t txBufferLen = BSP_ECG_ADS1192_SPI_SIZE_WRITE;

    if((inDevice != NULL) && (inData != NULL)) {
        /* byte format for Read opcode: | 001R RRRR | 000N NNNN | DATA |
         * RRRR - regAddr, NNNN - number of registers to write to */
        opcode[0] = BSP_ECG_ADS1192_SPI_WRITE_CMD | regAddr;
        opcode[1] = BSP_ECG_ADS1192_SPI_SINGLE_REG;
        opcode[2] = *inData;

        DRV_SPI_masterTxBlocking(inDevice->config->spiInstance,
                &opcode[0],
                txBufferLen,
                &spiErr);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(spiErr != DRV_SPI_err_NONE) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    } else {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Function for reading from single register of ADS1192.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [in]  inReg        - register address to read from.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 **************************************************************************************************/
static uint8_t BSP_ECG_ADS1192_readSingleReg(BSP_ECG_ADS1192_device_S *inDevice,
        const BSP_ECG_ADS1192_reg_E inReg,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;
    uint8_t opcode[BSP_ECG_ADS1192_SPI_SIZE_READ] = { 0u };
    uint8_t outData[BSP_ECG_ADS1192_SPI_SIZE_READ + BSP_ECG_ADS1192_SPI_READ_OFFSET];
    uint8_t rxTxBufferSize = BSP_ECG_ADS1192_SPI_SIZE_READ + BSP_ECG_ADS1192_SPI_READ_OFFSET;
    uint8_t regAddr = ecgADS1192RegAddr[inReg];
    uint8_t retVal = 0xFFu;

    if(inDevice != NULL) {
        /* byte format for Read opcode: | 001R RRRR | 000N NNNN |
         * RRRR - regAddr, NNNN - number of registers to read */
        opcode[0] = BSP_ECG_ADS1192_SPI_READ_CMD | regAddr;
        opcode[1] = BSP_ECG_ADS1192_SPI_SINGLE_REG;

        // start SPI transfer
        DRV_SPI_masterTxRxBlocking(inDevice->config->spiInstance,
                &opcode[0],
                rxTxBufferSize,
                &outData[0],
                &spiErr);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(spiErr != DRV_SPI_err_NONE) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        } else {
            // return register value
            retVal = outData[BSP_ECG_ADS1192_SPI_REG_VALUE_INDEX];
        }
    } else {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }

    return retVal;

}

/***********************************************************************************************//**
 * @brief Function enables PGA calibration for ADS1192.
 * @details If offset calibration enabled and OFFSETCAL SPI command is sent,
 *          it takes at least 305ms for device to calibrate properly.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    12.12.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_enablePgaCalibration(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    uint8_t calib = 0x80u;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_MISC_2,
            (uint8_t *) &calib,
            &ecgErr);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Function sets conversion (SPS) rate for ADS1192.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [in]  inSps        - wanted conversion rate (SPS) for device.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    12.12.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_setConversionRate(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_convRate_E inSps,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    BSP_ECG_ADS1192_config1Reg_U conf1Reg = { .R = 0u };
    conf1Reg.B.dr = (uint8_t) inSps;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_CONFIG_1,
            (uint8_t *) &conf1Reg,
            &ecgErr);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Registers setup for normal electrode input.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.12.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_setNormalElectrodeRead(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
    BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };

    // set CONFIG 2 register for normal signal
    conf2Reg.B.pdbRefBuf = BSP_ECG_ADS1192_INT_REF_BUFF_ENABLE;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_CONFIG_2,
            (uint8_t *) &conf2Reg,
            &ecgErr);
    //TODO: [mario.kodba 05-12-2020] check if this is needed
    // wait for reference voltage to settle
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    // PGA setting defined in device configuration
    chReg.B.pga = (uint8_t) inDevice->config->pgaSetting;
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH1_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Function for reading data shifted out of ADS1192.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [in]  inSize       - length (in bytes) of data to be read.
 * @param [out] *outData     - pointer to data to be read.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    01.11.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_readData(BSP_ECG_ADS1192_device_S *inDevice,
        const uint16_t inSize,
        uint8_t *outData,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;

    if((inDevice != NULL) && (outData != NULL)) {
        DRV_SPI_masterRxBlocking(inDevice->config->spiInstance,
                inSize,
                &outData[0],
                &spiErr);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(spiErr != DRV_SPI_err_NONE) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    } else {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Sends reset impulse on device initialization and resets registers.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    08.11.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_initReset(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // set nRESET pin high
    nrf_gpio_pin_set(ECG_RST);
    // wait 200ms, according to datasheet power-up sequence
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    /* hold nRESET low for 1 period of fMOD frequency,
     * according to datasheet, fMOD = 128kHz -> tMOD = 8us */
    nrf_gpio_pin_clear(ECG_RST);
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_1_FMOD);

    // set nRESET pin high again
    nrf_gpio_pin_set(ECG_RST);

    // wait 18 * tCLK, with fCLK = 512kHz -> tCLK = 2us
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_18_TCLK);

    // reset device registers to default values
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RESET, &ecgErr);
    // wait 9 * fMOD cycles for RESET command to finish executing
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_9_FMOD);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Starts reading and outputting data received from channel inputs.
 ***************************************************************************************************
 * @param [in]  pin    - GPIOTE pin number.
 * @param [in]  action - GPIOTE trigger action.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.12.2020
 **************************************************************************************************/
void BSP_ECG_ADS1192_DrdyPin_IRQHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {

    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;
    uint8_t bytes[BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME] = { 0 };
    // converted 16-bits data
    int16_t outData[BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME/2] = { 0 };

    // get data bytes from ADS1192
    DRV_SPI_masterRxBlocking(&instanceSpi0, BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME, &bytes[0], &spiErr);
    // convert data to correct format
    BSP_ECG_ADS1192_convertSignalToSignedVal(&bytes[0], &outData[0]);

    // fill buffer
    if(ecgDevice.changeBuffer == true) {
        ecgDevice.buffer2[ecgDevice.sampleIndex] = outData[2];
    } else {
        ecgDevice.buffer1[ecgDevice.sampleIndex] = outData[2];
    }

    ecgDevice.sampleIndex++;

    if(ecgDevice.sampleIndex == BSP_ECG_ADS1192_CONNECTION_EVENT_SIZE) {
        ecgDevice.updateReady = true;
        ecgDevice.sampleIndex = 0u;
        ecgDevice.changeBuffer ^= 1u;
    }
}

#if (DEBUG == true)
/***********************************************************************************************//**
 * @brief Sets device MUX to read PCB temperature from device sensor.
 ***************************************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.11.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_updateTemperature(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t outBuffer[BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME] = { 0u };

    // set channel 2 for temperature measurement
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
        // set PGA to 1x
        chReg.B.pga = (uint8_t) BSP_ECG_ADS1192_pga_1X;
        chReg.B.mux = (uint8_t) BSP_ECG_ADS1192_mux_TEMP_SENS;
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    // issue read command
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATA, &ecgErr);

    /* read data shifted out from device:
       16 status bits + 2 channels x 16 bits */
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_readData(inDevice,
                BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME,
                &outBuffer[0],
                &ecgErr);
    }
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK);

    // get read temperature bytes from ADC
    int16_t readVal = outBuffer[4] << BSP_ECG_ADS1192_TEMP_BIT_MASK;
    readVal = readVal | outBuffer[5];
    // set device temperature
    inDevice->temperature = (((readVal - BSP_ECG_ADS1192_TEMP_CONST_1) / BSP_ECG_ADS1192_TEMP_CONST_2)
            + BSP_ECG_ADS1192_TEMP_CONST_3);

    // set channel 2 back to default settings
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Function configures registers to output test signal.
 * @details Sets output of both channels to signal with amplitude
 *          +-(VREFP - VREFN) / 2420. In this case, VREFP = 2.42V, VREFN = 0V.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.11.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_readTestSignal(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // set CONFIG 2 register for test signal
    BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
    conf2Reg.B.pdbRefBuf = BSP_ECG_ADS1192_INT_REF_BUFF_ENABLE;
    conf2Reg.B.intTest = BSP_ECG_ADS1192_TEST_SIGNAL_ENABLE;
    conf2Reg.B.testFreq = BSP_ECG_ADS1192_TEST_SIGNAL_1HZ;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_CONFIG_2,
            (uint8_t *) &conf2Reg,
            &ecgErr);
    //TODO: [mario.kodba 05-12-2020] check if this is needed
    // wait for reference voltage to settle
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
    chReg.B.mux = (uint8_t) BSP_ECG_ADS1192_mux_TEST_SIGNAL;
    chReg.B.pga = (uint8_t) BSP_ECG_ADS1192_pga_6X;
    // set both channels to output test signal with PGA = 6x
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH1_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    // issue read continuous command
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATAC, &ecgErr);

    // TODO: [mario.kodba 19-12-2020] add timer to output signal for 10 seconds

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // enable ECG_DRDY pin interrupt
        nrf_drv_gpiote_in_event_enable(ECG_DRDY, true);
    }

    // if the timer timed out, stop signal test

    // disable ECG_DRDY pin interrupt
    nrf_drv_gpiote_in_event_disable(ECG_DRDY);

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // issue stop read data continuous command
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);
        // wait 4 * tCLK after sending SDATAC command
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Sets configuration to output supply voltage values on output.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.12.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_readSupplyMeasurement(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t outBuffer[BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME] = { 0 };
    int16_t supplyMeasurements[BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME/2] = { 0 };

    BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
    chReg.B.mux = BSP_ECG_ADS1192_mux_SUPPLY_MEAS;
    chReg.B.pga = BSP_ECG_ADS1192_pga_1X;
    // set both channels to output test signal with PGA = 1
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH1_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    }
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // issue read continuous command
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATA, &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // read data shifted out from device
        // 16 status bits + 2 channels x 16 bits
        BSP_ECG_ADS1192_readData(inDevice,
                BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME,
                &outBuffer[0],
                &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // convert SPI data to proper format
        BSP_ECG_ADS1192_convertSignalToSignedVal(&outBuffer[0],
                BSP_ECG_ADS1192_SPI_SIZE_SINGLE_FRAME,
                &supplyMeasurements[0]);
    }

    // save device supply
    inDevice->analogVddSupply =
            ((float) supplyMeasurements[1] / BSP_ECG_ADS1192_ADC_MAX_VALUE ) * BSP_ECG_ADS1192_ADC_REF_VOLTAGE * 2u;

    inDevice->digitalVddSupply =
            ((float) supplyMeasurements[2] / BSP_ECG_ADS1192_ADC_MAX_VALUE ) * BSP_ECG_ADS1192_ADC_REF_VOLTAGE * 4u;

    // set channels back to default settings
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH1_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Detects if any of the electrodes is disconnected.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    29.11.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_detectLeadOff(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t regVal;

    // set Lead-off comparator threshold
    uint8_t loffReg = BSP_ECG_ADS1192_LEAD_OFF_THRESHOLD;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_LOFF,
            &loffReg,
            &ecgErr);

    // configure Configuration 2 register for lead-off detection
    BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
    conf2Reg.B.pdbLoffComp = BSP_ECG_ADS1192_LEAD_OFF_COMP_ENABLE;
    conf2Reg.B.pdbRefBuf = BSP_ECG_ADS1192_INT_REF_BUFF_ENABLE;
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CONFIG_2,
                (uint8_t *) &conf2Reg,
                &ecgErr);
    }
    // wait for reference voltage to settle
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    // configure LOFF_SENS register - both channels P+ and N- for lead-off detection
    uint8_t loffSensReg = BSP_ECG_ADS1192_LEAD_OFF_BOTH_CHANNELS;
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_LOFF_SENS,
                &loffSensReg,
                &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // issue read command
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATA, &ecgErr);
    }

    // read Lead-off Status register
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        regVal = BSP_ECG_ADS1192_readSingleReg(inDevice,
                         BSP_ECG_ADS1192_reg_LOFF_STAT,
                         &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // check if any LEAD_OFF bit is set
        if((regVal & BSP_ECG_ADS1192_LEAD_OFF_MASK) != false) {
            ecgErr = BSP_ECG_ADS1192_err_LEAD_OFF;
        }
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/***********************************************************************************************//**
 * @brief Function detects if Right Leg Drive (RLD) electrode is disconnected.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.12.2020
 **************************************************************************************************/
static void BSP_ECG_ADS1192_detectRldOff(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // set Lead-off comparator threshold
    uint8_t loffReg = BSP_ECG_ADS1192_LEAD_OFF_THRESHOLD;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_LOFF,
            &loffReg,
            &ecgErr);

    // configure Configuration 2 register for lead-off detection
    BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
    conf2Reg.B.pdbLoffComp = BSP_ECG_ADS1192_LEAD_OFF_COMP_ENABLE;
    conf2Reg.B.pdbRefBuf = BSP_ECG_ADS1192_INT_REF_BUFF_ENABLE;
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CONFIG_2,
                (uint8_t *) &conf2Reg,
                &ecgErr);
    }
    // wait for reference voltage to settle
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    // configure RLD_SENS register - both channels P+ and N- for RLD-off detection
    uint8_t rldSensReg = BSP_ECG_ADS1192_RLD_OFF_BOTH_CHANNELS;
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_RLD_SENS,
                &rldSensReg,
                &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // issue read command
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATA, &ecgErr);
    }

    // read Lead-off Status register
    uint8_t regVal;
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        regVal = BSP_ECG_ADS1192_readSingleReg(inDevice,
                         BSP_ECG_ADS1192_reg_LOFF_STAT,
                         &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // check if any LEAD_OFF bit is set
        if((regVal & BSP_ECG_ADS1192_LEAD_OFF_MASK) != false) {
            ecgErr = BSP_ECG_ADS1192_err_RLD_OFF;
        }
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

#endif // #if (DEBUG == true)

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
