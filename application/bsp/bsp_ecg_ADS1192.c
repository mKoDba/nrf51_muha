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
#define BSP_ECG_ADS1192_SPI_OPCODE_SIZE         (2u)    //!< SPI command byte size
#define BSP_ECG_ADS1192_SPI_READ_OP             (0x1u)  //!< SPI read command
#define BSP_ECG_ADS1192_SPI_WRITE_OP            (0x40u) //!< SPI write command
#define BSP_ECG_ADS1192_SPI_MSG_MAX_SIZE        (14u)   //!< Maximum SPI message size
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK   (8u)    //!< 4 tCLK time period to us
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_8_US     (8u)    //!< nRESET pin wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_72_US    (72u)   //!< Reset command wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_1000_MS  (1000000u) //!< 1000ms wait time
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_36_US    (36u)   //!< 18 * t_clk wait time

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
static void BSP_ECG_ADS1192_sendSpiCommand(BSP_ECG_ADS1192_device_S *inDevice,
                                           const uint8_t inSpiCmd,
                                           BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_writeReg(BSP_ECG_ADS1192_device_S *inDevice,
                                     const BSP_ECG_ADS1192_reg_E inStartReg,
                                     const uint8_t inRegNumber,
                                     const uint8_t *inData,
                                     const uint8_t inDataLength,
                                     BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readReg(BSP_ECG_ADS1192_device_S *inDevice,
                                    const BSP_ECG_ADS1192_reg_E inStartReg,
                                    const uint8_t inRegNumber,
                                    uint8_t *outData,
                                    BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_readData(BSP_ECG_ADS1192_device_S *inDevice,
                                    const uint8_t inSize,
                                    uint8_t *outData,
                                    BSP_ECG_ADS1192_err_E *outErr);
static void BSP_ECG_ADS1192_initReset(BSP_ECG_ADS1192_device_S *inDevice,
                                      BSP_ECG_ADS1192_err_E *outErr);

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

        // send reset impulse
        BSP_ECG_ADS1192_initReset();

        // reset device registers to default values
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RESET, &ecgErr);
        // wait 9 * fMOD cycles for RESET command to finish executing
        nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_72_US);

        /* on power-up device defaults to Read Data Continous mode,
         * stop it to enable sending SPI commands */
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);
        // wait 4 * tCLK after sending SDATAC command
        nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_8_US);

        // set internal reference voltage
        BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0u };
        conf2Reg.R = 0x50u;
        BSP_ECG_ADS1192_writeReg(inDevice,
                                 BSP_ECG_ADS1192_reg_CONFIG_2,
                                 1u,
                                 (uint8_t *) &conf2Reg,
                                 1u,
                                 &ecgErr);
        // set 500SPS, continuous conversion
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_config1Reg_U conf1Reg = { .R = 0u };
            conf1Reg.B.dr1 = 1u;
            BSP_ECG_ADS1192_writeReg(inDevice,
                                     BSP_ECG_ADS1192_reg_CONFIG_1,
                                     1u,
                                     (uint8_t *) &conf1Reg,
                                     1u,
                                     &ecgErr);
        }
        // set both channels to input short (offset measurements)
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            uint8_t chRegs[2] = { 1u, 1u };
            BSP_ECG_ADS1192_writeReg(inDevice,
                                     BSP_ECG_ADS1192_reg_CH1_SET,
                                     2u,
                                     (uint8_t *) &chRegs,
                                     2u,
                                     &ecgErr);
        }

        uint32_t drdy = nrf_gpio_pin_read(ECG_DRDY);
        uint8_t outBuffer[16] = { 0u };
        // START pin is set to 1 already (pulled to VCC)
        // begin reading data from device
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATAC, &ecgErr);
        // nDRDY should be 0 now
        drdy = nrf_gpio_pin_read(ECG_DRDY);
        // read data shifted out from device
        // 16 status bits + 2 channels x 16 bits
        BSP_ECG_ADS1192_readData(inDevice, 16, &outBuffer[0], &ecgErr);
        for(uint8_t i = 0u; i<16; i++) {
            SEGGER_RTT_printf(0, "%x\n", outBuffer[i]);
        }
        drdy = nrf_gpio_pin_read(ECG_DRDY);
        // stop reading data
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);

        // now do test signal
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0u };
            conf2Reg.R = 0xA3u;
            BSP_ECG_ADS1192_writeReg(inDevice,
                                     BSP_ECG_ADS1192_reg_CONFIG_2,
                                     1u,
                                     (uint8_t *) &conf2Reg,
                                     1u,
                                     &ecgErr);
        }
        // set both channels to test signal
        if(ecgErr == BSP_ECG_ADS1192_err_NONE) {
            uint8_t chRegs[2] = { 5u, 5u };
            BSP_ECG_ADS1192_writeReg(inDevice,
                                     BSP_ECG_ADS1192_reg_CH1_SET,
                                     2u,
                                     &chRegs[0],
                                     2u,
                                     &ecgErr);
        }
        uint8_t outBuffer2[18] = { 0u };
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_RDATAC, &ecgErr);
        SEGGER_RTT_printf(0, "--------------\n");
        BSP_ECG_ADS1192_readData(inDevice, 18, &outBuffer2[0], &ecgErr);
        for(uint8_t i = 0u; i<18; i++) {
            SEGGER_RTT_printf(0, "%x\n", outBuffer2[i]);
        }
        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);

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

    if(inDevice == NULL) {
        ecgErr = BSP_ECG_ADS1192_err_NULL_PARAM;
    } else {
        uint32_t ret_code;
        ret_code = nrf_drv_spi_transfer(inDevice->config->spiInstance,
                                        &inSpiCmd,
                                        1u,
                                        NULL,
                                        0u);
        if(ret_code != NRF_SUCCESS) {
            ecgErr = BSP_ECG_ADS1192_err_SPI_READ_WRITE;
        }
    }

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 * @brief Function for writing to on or multiple registers of ADS1192.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [in]  inStartReg   - starting register address.
 * @param [in]  inRegNumber  - number of registers to be written to.
 * @param [in]  *inData      - pointer to data to be sent.
 * @param [in]  inDataLength - number of data bytes to send.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_writeReg(BSP_ECG_ADS1192_device_S *inDevice,
                                     const BSP_ECG_ADS1192_reg_E inStartReg,
                                     const uint8_t inRegNumber,
                                     const uint8_t *inData,
                                     const uint8_t inDataLength,
                                     BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    uint8_t opcode[BSP_ECG_ADS1192_SPI_OPCODE_SIZE] = { 0u };
    uint8_t regAddr = ecgADS1192RegAddr[inStartReg];
    uint32_t ret_code;

    if((inDevice != NULL) && (inData != NULL)) {
        // format for write opcode: | 010r rrrr | 000n nnnn |
        // set first byte of opcode for write operation
        opcode[0] = BSP_ECG_ADS1192_SPI_WRITE_OP | regAddr;
        // second byte represents number of registers we write to - 1
        opcode[1] = inRegNumber - 1;

        if(spi0Config.frequency <= NRF_DRV_SPI_FREQ_500K ) {
            /* when sending multi-byte commands, a 4 tCLK period must separate
             * the end of one byte (or opcode) and the next */

            // leave RX buffer empty, function allows it
            ret_code = nrf_drv_spi_transfer(&spi0Instance, &opcode[0], 1u, NULL, 0u);
            nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK);

            if(ret_code == NRF_SUCCESS) {
                ret_code = nrf_drv_spi_transfer(&spi0Instance, &opcode[1], 1u, NULL, 0u);
                nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK);
            }

            // if opcodes sent successfully, send data bytes
            if(ret_code == NRF_SUCCESS) {
                for(uint8_t i = 0u; i<inDataLength; i++){
                    ret_code = nrf_drv_spi_transfer(&spi0Instance, &inData[i], 1u, NULL, 0u);
                    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_4_TCLK);
                }
            }
        } else {
            // for higher SCLK frequencies no need for delay between SPI transfer
            uint8_t spiMsg[BSP_ECG_ADS1192_SPI_MSG_MAX_SIZE] = { 0u };
            // 2 bytes for opcode + input data length
            uint8_t msgSize = inDataLength + 2u;

            // assemble SPI message
            // OPCODE_1 | OPCODE_2 | DATA1 | DATA2 | DATA3 | ...
            spiMsg[0] = opcode[0];
            spiMsg[1] = opcode[1];
            (void *) memcpy((uint8_t *) &(spiMsg[2]), inData, inDataLength);
            // leave RX buffer empty, function allows it
            ret_code = nrf_drv_spi_transfer(&spi0Instance, spiMsg, msgSize, NULL, 0u);
        }

        if(ret_code != NRF_SUCCESS) {
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
 * @brief Function for reading from one or multiple registers of ADS1192.
 ******************************************************************************
 * @param [in]  *inDevice    - device structure for ECG driver.
 * @param [in]  inStartReg   - starting register address.
 * @param [in]  inRegNumber  - number of registers to read.
 * @param [out] *outData     - pointer to data to be read.
 * @param [out] *outErr      - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    21.10.2020
 ******************************************************************************/
static void BSP_ECG_ADS1192_readReg(BSP_ECG_ADS1192_device_S *inDevice,
                                    const BSP_ECG_ADS1192_reg_E inStartReg,
                                    const uint8_t inRegNumber,
                                    uint8_t *outData,
                                    BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    uint8_t opcode[BSP_ECG_ADS1192_SPI_OPCODE_SIZE] = { 0u };
    uint8_t regAddr = ecgADS1192RegAddr[inStartReg];

    if((inDevice != NULL) && (outData != NULL)) {
        // format for read opcode: | 001r rrr | 000n nnnn |
        // set first byte of opcode for read operation
        opcode[0] = (BSP_ECG_ADS1192_SPI_READ_OP << 5u) | regAddr;
        // set second byte of opcode to inRegNumber - 1
        opcode[1] = inRegNumber - 1;

        uint32_t ret_code;
        // start SPI transfer
        // TODO: check this
        // first 2 bytes from RX should be discarded?
        ret_code = nrf_drv_spi_transfer(&spi0Instance,
                                        opcode,
                                        BSP_ECG_ADS1192_SPI_OPCODE_SIZE,
                                        &outData[0],
                                        inRegNumber+2u);
        if(ret_code != NRF_SUCCESS) {
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
                                    const uint8_t inSize,
                                    uint8_t *outData,
                                    BSP_ECG_ADS1192_err_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    if((inDevice != NULL) && (outData != NULL)) {
        uint32_t ret_code;
        // start SPI transfer
        ret_code = nrf_drv_spi_transfer(&spi0Instance,
                                        NULL,
                                        0u,
                                        &outData[0],
                                        inSize);
        if(ret_code != NRF_SUCCESS) {
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
 * @brief Function activates reset impulse needed on device initialization.
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
    // wait 1s
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_1000_MS);

    /* hold nRESET low for 1 period of fMOD frequency,
     * according to datasheet, fMOD = 128kHz -> tMOD = 8us*/
    nrf_gpio_pin_clear(ECG_RST);
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_8_US);

    // set nRESET pin high again
    nrf_gpio_pin_set(ECG_RST);

    // wait 18 * tCLK, with fCLK = 512kHz -> tCLK = 2us
    nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_36_US);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
