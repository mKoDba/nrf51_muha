/*	header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include <stddef.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "cfg_nrf_drv_spi.h"
#include "cfg_nrf51_muha_pinout.h"
#include "bsp_ecg_ADS1192.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/
//! ADC constants
#define BSP_ECG_ADS1192_ADC_REF_VOLTAGE         (2.42f) //!< ADC reference voltage
#define BSP_ECG_ADS1192_ADC_MAX_VALUE           (32767) //!< Maximal ADC value (2^15 - 1)

//! ECG ADS1192 SPI commands
#define BSP_ECG_ADS1192_SPI_WAKEUP              (0x02u) //!< Wake-up from standby mode
#define BSP_ECG_ADS1192_SPI_STANDBY             (0x04u) //!< Enter standby mode
#define BSP_ECG_ADS1192_SPI_RESET               (0x06u) //!< Reset the device
#define BSP_ECG_ADS1192_SPI_START               (0x08u) //!< Start conversions
#define BSP_ECG_ADS1192_SPI_STOP                (0x0Au) //!< Stop conversion
#define BSP_ECG_ADS1192_SPI_OFFSETCAL           (0x1Au) //!< Channel offset calibration
#define BSP_ECG_ADS1192_SPI_RDATAC              (0x10u) //!< Read Data Continuous mode
#define BSP_ECG_ADS1192_SPI_SDATAC              (0x11u) //!< Stop Read Data Continuous
#define BSP_ECG_ADS1192_SPI_RDATA               (0x12u) //!< Read data by command

//! ECG ADS1192 SPI constants
#define BSP_ECG_ADS1192_SPI_OPCODE_SIZE_R       (2u)    //!< SPI Read command byte size
#define BSP_ECG_ADS1192_SPI_OPCODE_SIZE_W       (3u)    //!< SPI Write command byte size
#define BSP_ECG_ADS1192_SPI_READ_OP             (0x20u) //!< SPI read command
#define BSP_ECG_ADS1192_SPI_WRITE_OP            (0x40u) //!< SPI write command
#define BSP_ECG_ADS1192_SPI_MSG_MAX_SIZE        (14u)   //!< Maximum SPI message size
#define BSP_ECG_ADS1192_SPI_SINGLE_REG          (0x00u) //!< Single register read operation
#define BSP_ECG_ADS1192_SPI_READ_OFFSET         (1u)    //!< SPI read RX offset
#define BSP_ECG_ADS1192_SPI_REG_VALUE_INDEX     (2u)    //!< SPI register value index

//! ECG ADS1192 Time constants
#define BSP_ECG_ADS1192_WAIT_TIME_8_US          (8u)    //!< 8 us wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK   (8u)    //!< 4 tCLK time period (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_18_TCLK  (36u)   //!< 18 tCLK time period (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_1_FMOD   (8u)    //!< 1 fMOD wait time (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_9_FMOD   (72u)   //!< 9 fMOD wait time (us)
#define BSP_ECG_ADS1192_INIT_OSC_WAIT_TIME_US   (32u)   //!< Internal oscillator wait time (us)
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_1_MS     (1000u) //!< 1ms wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS   (200000u) //!< 200ms wait time
#define BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS  (310u)  //! Offset calibration wait time (ms)

//! ECG ADS1192 bit manipulation constants
#define BSP_ECG_ADS1192_LEAD_OFF_MASK           (0x0Fu) //!< Channel 1 and 2 Lead-off detection
#define BSP_ECG_ADS1192_BYTE_SHIFT              (8u)    //!< Byte shift value

//! ECG ADS1192 temperature constants
#define BSP_ECG_ADS1192_TEMP_CONST_1            (168)   //!< Temperature constant 1
#define BSP_ECG_ADS1192_TEMP_CONST_2            (394)   //!< Temperature constant 2
#define BSP_ECG_ADS1192_TEMP_CONST_3            (25)    //!< Temperature constant 3
#define BSP_ECG_ADS1192_TEMP_BIT_MASK           (8u)    //!< Temperature bit mask

//! ECG ADS1192 register values
#define BSP_ECG_ADS1192_MUX_TEST_SIGNAL         (5u)    //!< Channel MUX test signal value
#define BSP_ECG_ADS1192_LEAD_OFF_BOTH_CHANNELS  (0x0Fu) //!< Register value for lead-off detection
#define BSP_ECG_ADS1192_LEAD_OFF_THRESHOLD      (0x1Cu) //!< Threshold value for lead-off detection
#define BSP_ECG_ADS1192_RLD_OFF_BOTH_CHANNELS   (0x1Fu) //!< Register value for RLD-off detection

//! ECG ADS1192 register addresses
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

/*******************************************************************************
 *                          GLOBAL VARIABLES
 ******************************************************************************/
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

/*******************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/
static inline void BSP_ECG_ADS1192_convertSignalToSignedVal(const uint8_t *inData,
        const uint16_t inDataSize,
        int16_t *outData,
        BSP_ECG_ADS1192_err_E *outErr);
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
static void BSP_ECG_ADS1192_startSignalRead(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readData(BSP_ECG_ADS1192_device_S *inDevice,
        const uint16_t inSize,
        uint8_t *outData,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_initReset(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_detectLeadOff(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_detectRldOff(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
#if (DEBUG == true)
static void BSP_ECG_ADS1192_updateTemperature(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readSupplyMeasurement(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readTestSignal(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr);
#endif // #if (DEBUG == true)

/*******************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Function initializes ECG ADS1192 component.
 ******************************************************************************
 * @param [in]  *inDevice - device structure for ECG driver
 * @param [in]  *inConfig - configuration structure for ECG driver.
 * @param [out] *outErr   - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 ******************************************************************************/
void BSP_ECG_ADS1192_init(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_config_S *inConfig,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    if((inDevice != NULL) && (inConfig != NULL)) {
        inDevice->config = inConfig;
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
            conf2.B.pdbRefBuf = 1u;
            BSP_ECG_ADS1192_writeSingleReg(inDevice,
                    BSP_ECG_ADS1192_reg_CONFIG_2,
                    (uint8_t *) &conf2,
                    &ecgErr);
            // wait for reference voltage to settle
            nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);
        }

        // set SPS, continuous conversion
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_setConversionRate(inDevice, BSP_ECG_ADS1192_convRate_125_SPS, &ecgErr);
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

        // check test signal
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_readTestSignal(inDevice, &ecgErr);
        }
#endif // #if (DEBUG == true)

//        // check if any Lead is off
//        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
//            BSP_ECG_ADS1192_detectLeadOff(inDevice, &ecgErr);
//        }
//
//        // check if RLD is off
//        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
//            BSP_ECG_ADS1192_detectRldOff(inDevice, &ecgErr);
//        }

        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_setNormalElectrodeRead(inDevice, &ecgErr);
        }

        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_startSignalRead(inDevice, &ecgErr);
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

/*******************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Function for converting output signal into usable value.
 * @details Converts 8-bit output signal array into correct 16-bit
 *          twos-complement values (amplitudes).
 ******************************************************************************
 * @param [in]  *inData    - pointer to input data array.
 * @param [in]  inDataSize - input data array size.
 * @param [out] *outData   - pointer to output data array.
 * @param [out] *outErr    - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    05.12.2020
 ******************************************************************************/
static inline void BSP_ECG_ADS1192_convertSignalToSignedVal(const uint8_t *inData,
        const uint16_t inDataSize,
        int16_t *outData,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // should compile to single instruction
    for(uint16_t i = 0, j = 0; i < inDataSize - 1; i += 2, j++) {
        int16_t tmp = *(inData + i + 1) | (*(inData + i) << BSP_ECG_ADS1192_BYTE_SHIFT);
        // use this to convert into voltage amplitude
//        float ampl = ((float) tmp) * (BSP_ECG_ADS1192_ADC_REF_VOLTAGE / BSP_ECG_ADS1192_ADC_MAX_VALUE);
        memcpy((outData + j), &tmp, sizeof(int16_t));

//        // little endian implementation
//        uint16_t tmp = *(inData + i) | (*(inData + i + 1) << BSP_ECG_ADS1192_BYTE_SHIFT);
//        memcpy((outData + j), &tmp, 2);
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 * @brief Function sends predefined SPI command to the device.
 ******************************************************************************
 * @param [in]  *inDevice - device structure for ECG driver.
 * @param [in]  inSpiCmd  - SPI command.
 * @param [out] *outErr   - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    24.10.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_sendSpiCommand(BSP_ECG_ADS1192_device_S *inDevice,
        const uint8_t inSpiCmd,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint16_t txBufferLen = 1u;
    bool status = false;

    if(inDevice == NULL) {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    } else {
        status = DRV_SPI_masterTxBlocking(inDevice->config->spiInstance,
                     txBufferLen,
                     &inSpiCmd);
//        ret_code = nrf_drv_spi_transfer(inDevice->config->spiInstance,
//                           &inSpiCmd,
//                           txBufferLen,
//                           NULL,
//                           rxBufferLen);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(status != true) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 * @brief Function for writing to single register of ADS1192.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [in]  inReg        - register address.
 * @param [in]  *inData      - pointer to data to be sent.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_writeSingleReg(BSP_ECG_ADS1192_device_S *inDevice,
        const BSP_ECG_ADS1192_reg_E inReg,
        const uint8_t *inData,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t opcode[BSP_ECG_ADS1192_SPI_OPCODE_SIZE_W] = { 0u };
    uint8_t regAddr = ecgADS1192RegAddr[inReg];
//    uint8_t rxBufferLen = 0u;
    uint8_t txBufferLen = BSP_ECG_ADS1192_SPI_OPCODE_SIZE_W;
//    uint32_t ret_code;
//    uint8_t rxData[BSP_ECG_ADS1192_SPI_OPCODE_SIZE_W] = { 0 };
    bool status = false;

    if((inDevice != NULL) && (inData != NULL)) {
        /* byte format for Read opcode: | 001R RRRR | 000N NNNN | DATA |
         * RRRR - regAddr, NNNN - number of registers to write to */
        opcode[0] = BSP_ECG_ADS1192_SPI_WRITE_OP | regAddr;
        opcode[1] = BSP_ECG_ADS1192_SPI_SINGLE_REG;
        opcode[2] = *inData;

        status = DRV_SPI_masterTxBlocking(inDevice->config->spiInstance,
                         txBufferLen,
                         &opcode[0]);

//        // leave RX buffer empty, function allows it
//        ret_code = nrf_drv_spi_transfer(inDevice->config->spiInstance,
//                           &opcode[0],
//                           txBufferLen,
//                           NULL,
//                           rxBufferLen);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(status != true) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    } else {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 * @brief Function for reading from single register of ADS1192.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [in]  inReg        - starting register address.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 ******************************************************************************/
static uint8_t BSP_ECG_ADS1192_readSingleReg(BSP_ECG_ADS1192_device_S *inDevice,
        const BSP_ECG_ADS1192_reg_E inReg,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t retVal = 0xFFu;
//    uint32_t ret_code;
    uint8_t opcode[BSP_ECG_ADS1192_SPI_OPCODE_SIZE_R] = { 0u };
    uint8_t outData[BSP_ECG_ADS1192_SPI_OPCODE_SIZE_R + BSP_ECG_ADS1192_SPI_READ_OFFSET];
//    uint8_t txBufferLen = BSP_ECG_ADS1192_SPI_OPCODE_SIZE_R;
    uint8_t rxBufferLen = BSP_ECG_ADS1192_SPI_OPCODE_SIZE_R + BSP_ECG_ADS1192_SPI_READ_OFFSET;
    uint8_t regAddr = ecgADS1192RegAddr[inReg];
    bool status = false;

    if(inDevice != NULL) {
        /* byte format for Read opcode: | 001R RRRR | 000N NNNN |
         * RRRR - regAddr, NNNN - number of registers to read */
        opcode[0] = BSP_ECG_ADS1192_SPI_READ_OP | regAddr;
        opcode[1] = BSP_ECG_ADS1192_SPI_SINGLE_REG;

        // start SPI transfer
        status = DRV_SPI_MasterTxRxBlocking(inDevice->config->spiInstance,
                         &opcode[0],
                         rxBufferLen,
                         &outData[0]);

//        ret_code = nrf_drv_spi_transfer(inDevice->config->spiInstance,
//                           &opcode[0],
//                           txBufferLen,
//                           &outData[0],
//                           rxBufferLen);
        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(status != true) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        } else {
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

/*******************************************************************************
 * @brief Function enables PGA calibration for ADS1192.
 * @details If offset calibration enabled and OFFSETCAL SPI command is sent,
 *          it takes at least 305ms for device to calibrate properly.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    12.12.2020
 ******************************************************************************/
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

/*******************************************************************************
 * @brief Function sets conversion (SPS) rate for ADS1192.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [in]  inSps        - wanted conversion rate (SPS) for device.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    12.12.2020
 ******************************************************************************/
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

/*******************************************************************************
 * @brief Registers setup for normal electrode input.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    13.12.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_setNormalElectrodeRead(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    // set CONFIG 2 register for normal signal
    BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
    conf2Reg.B.pdbRefBuf = 1u;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_CONFIG_2,
            (uint8_t *) &conf2Reg,
            &ecgErr);
    //TODO: [mario.kodba 05-12-2020] check if this is needed
    // wait for reference voltage to settle
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
    // set both channels to normal input with PGA = 6
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

/*******************************************************************************
 * @brief Function for reading data shifted out of ADS1192.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [in]  inSize       - length (in bytes) of data to be read.
 * @param [out] *outData     - pointer to data to be read.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    01.11.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_readData(BSP_ECG_ADS1192_device_S *inDevice,
        const uint16_t inSize,
        uint8_t *outData,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
//    uint32_t ret_code;
    bool status = false;

    if((inDevice != NULL) && (outData != NULL)) {
//        // start SPI transfer
//        ret_code = nrf_drv_spi_transfer(inDevice->config->spiInstance,
//                           NULL,
//                           txBufferLen,
//                           outData,
//                           rxBufferLen);
        status = DRV_SPI_masterRxBlocking(inDevice->config->spiInstance,
                         inSize,
                         &outData[0]);

        // TODO: [mario.kodba 1.12.2020.] check this delay and value, seems to work correctly with it
        nrf_delay_us(BSP_ECG_ADS1192_WAIT_TIME_8_US);

        if(status != true) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    } else {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 * @brief Sends reset impulse on device initialization and resets registers.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    08.11.2020
 ******************************************************************************/
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


/*******************************************************************************
 * @brief Starts reading and outputting data received from channel inputs.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    13.12.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_startSignalRead(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t outBuffer[600] = { 0 };
    int16_t ecgData[300] = { 0 };

    // issue read continuous command
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATAC, &ecgErr);

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // wait for new data to be ready for conversion
        while(nrf_gpio_pin_read(ECG_DRDY) == true);

        // read data shifted out from device
        // 16 status bits + 2 channels x 16 bits
        BSP_ECG_ADS1192_readData(inDevice, 600, &outBuffer[0], &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // convert SPI data to proper format
        BSP_ECG_ADS1192_convertSignalToSignedVal(&outBuffer[0],
                600,
                &ecgData[0],
                &ecgErr);
    }

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

/*******************************************************************************
 * @brief Detects if any of the electrodes is disconnected.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    29.11.2020
 ******************************************************************************/
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
    conf2Reg.B.pdbLoffComp = 1u;
    conf2Reg.B.pdbRefBuf = 1u;
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

/*******************************************************************************
 * @brief Function detects if Right Leg Drive (RLD) electrode is disconnected.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    03.12.2020
 ******************************************************************************/
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
    conf2Reg.B.pdbLoffComp = 1u;
    conf2Reg.B.pdbRefBuf = 1u;
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

#if (DEBUG == true)
/*******************************************************************************
 * @brief Sets device MUX to read PCB temperature from device sensor.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    13.11.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_updateTemperature(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t outBuffer[6] = { 0u };

    // set channel 2 for temperature measurement
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
        // set PGA to 1
        chReg.B.pga = 1u;
        chReg.B.mux = 4u;
        BSP_ECG_ADS1192_writeSingleReg(inDevice,
                BSP_ECG_ADS1192_reg_CH2_SET,
                (uint8_t *) &chReg,
                &ecgErr);
    }

    // calibrate PGA
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_OFFSETCAL, &ecgErr);
    nrf_delay_ms(BSP_ECG_ADS1192_WAIT_TIME_OFFSETCAL_MS);

    // start reading data
    BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATA, &ecgErr);

    /* read data shifted out from device:
       16 status bits + 2 channels x 16 bits */
    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        BSP_ECG_ADS1192_readData(inDevice, 18, &outBuffer[0], &ecgErr);
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

/*******************************************************************************
 * @brief Function configures registers to output test signal.
 * @details Sets output of both channels to signal with amplitude
 *          +-(VREFP - VREFN) / 2420. In this case, VREFP = 2.42V, VREFN = 0V.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    13.11.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_readTestSignal(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t outBuffer[600] = { 0 };
    int16_t ecgData[300] = { 0 };

    // set CONFIG 2 register for test signal
    BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
    conf2Reg.B.pdbRefBuf = 1u;
    conf2Reg.B.intTest = 1u;
    conf2Reg.B.testFreq = 1u;
    BSP_ECG_ADS1192_writeSingleReg(inDevice,
            BSP_ECG_ADS1192_reg_CONFIG_2,
            (uint8_t *) &conf2Reg,
            &ecgErr);
    //TODO: [mario.kodba 05-12-2020] check if this is needed
    // wait for reference voltage to settle
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_200_MS);

    BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
    chReg.B.mux = BSP_ECG_ADS1192_MUX_TEST_SIGNAL;
    // set both channels to output test signal with PGA = 6
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

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // wait for new data to be ready for conversion
        while(nrf_gpio_pin_read(ECG_DRDY) == true);

        // read data shifted out from device
        // 16 status bits + 2 channels x 16 bits
        BSP_ECG_ADS1192_readData(inDevice, 600, &outBuffer[0], &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // convert SPI data to proper format
        BSP_ECG_ADS1192_convertSignalToSignedVal(&outBuffer[0],
                600,
                &ecgData[0],
                &ecgErr);
    }

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

/*******************************************************************************
 * @brief Sets configuration to output supply voltage values on output.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    03.12.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_readSupplyMeasurement(BSP_ECG_ADS1192_device_S *inDevice,
        BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;
    uint8_t outBuffer[6] = { 0 };
    int16_t supplyMeasurements[3] = { 0 };

    BSP_ECG_ADS1192_chXsetReg_U chReg = { .R = 0u };
    chReg.B.mux = 3u;
    chReg.B.pga = 1u;
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
        BSP_ECG_ADS1192_readData(inDevice, 6, &outBuffer[0], &ecgErr);
    }

    if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
        // convert SPI data to proper format
        BSP_ECG_ADS1192_convertSignalToSignedVal(&outBuffer[0],
                6,
                &supplyMeasurements[0],
                &ecgErr);
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
#endif // #if (DEBUG == true)

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
