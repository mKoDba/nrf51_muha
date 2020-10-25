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
#define BSP_ECG_ADS1192_SPI_WRITE_OP            (0x2u)  //!< SPI write command
#define BSP_ECG_ADS1192_SPI_MSG_MAX_SIZE        (14u)   //!< Maximum SPI message size
#define BSP_ECG_ADS1192_INIT_WAIT_TIME_8_US     (8u)    //!< nRESET pin wait time
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
#define BSP_ECG_ADS1192_REG_ADDR_ID             (0x0u)  //!<
// global settings across channels
#define BSP_ECG_ADS1192_REG_ADDR_CONFIG_1       (0x1u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_CONFIG_2       (0x2u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_LOFF           (0x3u)  //!<
// channel specific settings
#define BSP_ECG_ADS1192_REG_ADDR_CH1_SET        (0x4u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_CH2_SET        (0x5u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_RLD_SENS       (0x6u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_LOFF_SENS      (0x7u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_LOFF_STAT      (0x8u)  //!<
// GPIO and other settings
#define BSP_ECG_ADS1192_REG_ADDR_MISC_1         (0x9u)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_MISC_2         (0xAu)  //!<
#define BSP_ECG_ADS1192_REG_ADDR_GPIO           (0xBu)  //!<

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

        nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_1000_MS);
        // set nRESET pin
        nrf_gpio_pin_set(ECG_RST);
        // clear nRESET pin
        nrf_gpio_pin_clear(ECG_RST);
        /* hold nRESET low for 1 period of f_mod frequency,
         * according to datasheet, f_mod = 128kHz */
        nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_8_US);
        // set nRESET pin again
        nrf_gpio_pin_set(ECG_RST);
        // wait 18 * t_clk
        nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_36_US);

        BSP_ECG_ADS1192_sendSpiCommand(inDevice, BSP_ECG_ADS1192_SPI_SDATAC, &ecgErr);
        // wait 4 * t_clk after sending SDATAC
        nrf_delay_us(BSP_ECG_ADS1192_INIT_WAIT_TIME_8_US);
        // set internal reference voltage
        BSP_ECG_ADS1192_config2Reg_U conf2Reg = { .R = 0x80u };
        conf2Reg.B.pdbRefBuf = 1u;
        BSP_ECG_ADS1192_writeReg(inDevice,
                                 BSP_ECG_ADS1192_reg_CONFIG_2,
                                 1u,
                                 (uint8_t *) &conf2Reg,
                                 1u,
                                 &ecgErr);

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

    if(inDevice != NULL) {
        uint32_t ret_code;
        ret_code = nrf_drv_spi_transfer(inDevice->config->spiInstance,
                                        &inSpiCmd,
                                        1u,
                                        NULL,
                                        0u);
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
    uint8_t spiMsg[BSP_ECG_ADS1192_SPI_MSG_MAX_SIZE] = { 0u };
    // 2 bytes for opcode + input data length
    uint8_t msgSize = inDataLength + 2u;

    if((inDevice != NULL) && (inData != NULL)) {
        // format for write opcode: | 010r rrr | 000n nnnn |
        // set first byte of opcode for write operation
        opcode[0] = (BSP_ECG_ADS1192_SPI_WRITE_OP << 5u) | regAddr;
        // set second byte of opcode to inRegNumber - 1
        opcode[1] = inRegNumber - 1;

        // assemble SPI message
        // OPCODE_1 | OPCODE_2 | DATA1 | DATA2 | DATA3 | ...
        spiMsg[0] = opcode[0];
        spiMsg[1] = opcode[1];
        (void *) memcpy((uint8_t *) &(spiMsg[2]), inData, inDataLength);

        uint32_t ret_code;
        // leave RX buffer empty, function allows it
        ret_code = nrf_drv_spi_transfer(&spi0Instance, spiMsg, msgSize, NULL, 0u);
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
                                        outData,
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
 *                          END OF FILE
 ******************************************************************************/
