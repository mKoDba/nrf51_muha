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
 * @file    bsp_mpu9150.c
 * @author  mario.kodba
 * @brief   MPU9150 MEMS source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stddef.h>

#include "bsp_mpu9150.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define BSP_MPU9150_READ_DATA_SIZE_IN_BYTES     (14u)           //!< Number of bytes to be read on single TWI transfer
#define BSP_MPU9150_ACC_DATA_TWI_OFFSET         (0u)            //!< Index (offset) of accelerometer data in TWI reading
#define BSP_MPU9150_TEMP_DATA_TWI_OFFSET        (6u)            //!< Index (offset) of temperature data in TWI reading
#define BSP_MPU9150_GYRO_DATA_TWI_OFFSET        (8u)            //!< Index (offset) of gyroscope data in TWI reading

#define BSP_MPU9150_WRITE_BYTE                  (0b11010000u)   //!< MPU-9150 I2C write byte
#define BSP_MPU9150_READ_BYTE                   (0b11010001u)   //!< MPU-9150 I2C read byte

#define BSP_MPU9150_TWI_TIMEOUT                 (10000u)        //!< I2C transfer timeout value
#define BSP_MPU9150_SINGLE_REG_MSG_SIZE         (2u)            //!< I2C message size for single register

#define BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT (32767.f)       //!< Max positive value for signed 16-bit (floating point)
#define BSP_MPU9150_16_BIT_SIGNED_MAX_VAL       (32767)         //!< Max positive value for signed 16-bit

#define BSP_MPU9150_TEMP_CONVERSION_CONST_1     (340)           //!< First constant for temperature conversion (datasheet)
#define BSP_MPU9150_TEMP_CONVERSION_CONST_2     (35)            //!< Second constant for temperature conversion (datasheet)

#define BSP_MPU9150_GYRO_RANGE_250DEG_CONST     (250.f)         //!< Constant for multiplication in gyro range +-250deg/s
#define BSP_MPU9150_GYRO_RANGE_500DEG_CONST     (500.f)         //!< Constant for multiplication in gyro range +-500deg/s
#define BSP_MPU9150_GYRO_RANGE_1000DEG_CONST    (1000.f)        //!< Constant for multiplication in gyro range +-1000deg/s
#define BSP_MPU9150_GYRO_RANGE_2000DEG_CONST    (2000.f)        //!< Constant for multiplication in gyro range +-2000deg/s

#define BSP_MPU9150_ACC_RANGE_2G_CONST          (2.f)           //!< Constant for multiplication in acc range +-2G
#define BSP_MPU9150_ACC_RANGE_4G_CONST          (4.f)           //!< Constant for multiplication in acc range +-4G
#define BSP_MPU9150_ACC_RANGE_8G_CONST          (8.f)           //!< Constant for multiplication in acc range +-8G
#define BSP_MPU9150_ACC_RANGE_16G_CONST         (16.f)          //!< Constant for multiplication in acc range +-16G

// pre-defined register values to send
#define BSP_MPU9150_RESET_SIGNAL_PATH_VALUE     (0x7u)          //!< Value to send in order to reset signal paths
#define BSP_MPU9150_PLL_REFERENCE_VALUE         (0x01u)         //!< Value to send to set PLL reference as X Gyroscope
#define BSP_MPU9150_DEVICE_ID_VALUE             (0b01101000u)   //!< Device ID that will verify correct device operation


/***************************************************************************************************
 *                          GLOBAL VARIABLES
 **************************************************************************************************/
static volatile bool twi_tx_done = false;
static volatile bool twi_rx_done = false;

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_waitRxTransferFinish(BSP_MPU9150_err_E *outErr);
__INLINE static void BSP_MPU9150_waitTxTransferFinish(BSP_MPU9150_err_E *outErr);
__INLINE static void BSP_MPU9150_calculateTemperature(const uint8_t *rawValue, int16_t *outTemp);
__INLINE static void BSP_MPU9150_calculateGyroValues(const BSP_MPU9150_device_S *inDevice,
        const uint8_t *rawValue,
        int16_t *outGyro);
__INLINE static void BSP_MPU9150_calculateAccValues(const BSP_MPU9150_device_S *inDevice,
        const uint8_t *rawValue,
        int16_t *outAcc);
__INLINE static void BSP_MPU9150_packDataToSend(const int16_t *gyroVals,
        const int16_t *accVals,
        const int16_t temp,
        int16_t *outBuffer);

static void BSP_MPU9150_writeSingleReg(BSP_MPU9150_device_S *inDevice,
        const uint8_t regAddr,
        const uint8_t regData,
        BSP_MPU9150_err_E *outErr);
static void BSP_MPU9150_readSingleReg(BSP_MPU9150_device_S *inDevice,
        const uint8_t regAddr,
        uint8_t *data,
        BSP_MPU9150_err_E *outErr);
static void BSP_MPU9150_readMultiReg(BSP_MPU9150_device_S *inDevice,
        const uint8_t startRegAddr,
        const uint8_t length,
        uint8_t *data,
        BSP_MPU9150_err_E *outErr);
static void BSP_MPU9150_configuration(BSP_MPU9150_device_S *inDevice, BSP_MPU9150_err_E *outErr);

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function initializes MEMS MPU-9150 component.
 ***************************************************************************************************
 * @param [in]  *inDevice - pointer to device structure for MPU-9150 driver.
 * @param [in]  *inConfig - configuration structure for MPU-9150 driver.
 * @param [out] *outErr   - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    15.05.2021
 **************************************************************************************************/
void BSP_MPU9150_init(BSP_MPU9150_device_S *inDevice,
        BSP_MPU9150_config_S *inConfig,
        BSP_MPU9150_err_E *outErr) {

    BSP_MPU9150_err_E err = BSP_MPU9150_err_NONE;
    uint8_t deviceId = 0u;

    if((inDevice != NULL) && (inConfig != NULL)) {
        inDevice->config = inConfig;

        // verify device ID by reading register value
        BSP_MPU9150_readSingleReg(inDevice,
                BSP_MPU9150_REG_WHO_AM_I,
                &deviceId,
                &err);

        // reset signal path for gyroscope, accelerometer and temperature sensor
        BSP_MPU9150_writeSingleReg(inDevice,
                BSP_MPU9150_REG_SIGNAL_PATH_RESET,
                BSP_MPU9150_RESET_SIGNAL_PATH_VALUE,
                &err);
        nrf_delay_ms(30u);

        if(err == BSP_MPU9150_err_NONE) {
            // set PLL with X axis gyroscope reference (for better clock stability)
            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_PWR_MGMT_1,
                    BSP_MPU9150_PLL_REFERENCE_VALUE,
                    &err);
            // wait 1ms for PLL to settle (datasheet)
            nrf_delay_ms(1u);
        }

        // verify device ID by reading register value
        BSP_MPU9150_readSingleReg(inDevice,
                BSP_MPU9150_REG_WHO_AM_I,
                &deviceId,
                &err);

        if(err == BSP_MPU9150_err_NONE) {
            if(deviceId != BSP_MPU9150_DEVICE_ID_VALUE) {
                // device ID value read is not correct
                err = BSP_MPU9150_err_INIT;
            }
        }

        // configure sensors before start of sampling
        if(err == BSP_MPU9150_err_NONE) {
            BSP_MPU9150_configuration(inDevice, &err);
        }
        // TODO: check if this is needed at all
        nrf_delay_ms(50u);

        if(err == BSP_MPU9150_err_NONE) {
            inDevice->isInitialized = true;
        }
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Updates temperature, accelerometer, gyroscope data to buffer.
 ***************************************************************************************************
 * @param [in]  *inDevice   - pointer to device structure for MPU-9150 driver.
 * @param [in]  *newValues  - pointer to buffer that contains new updated values.
 * @param [out] *outErr     - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    01.06.2021
 **************************************************************************************************/
void BSP_MPU9150_updateValues(BSP_MPU9150_device_S *inDevice,
        int16_t *newValues,
        BSP_MPU9150_err_E *outErr) {

    BSP_MPU9150_err_E mpuErr = BSP_MPU9150_err_NONE;
    // buffer for raw register values
    uint8_t byteBuffer[BSP_MPU9150_READ_DATA_SIZE_IN_BYTES];
    int16_t accVals[6];
    int16_t gyroVals[6];
    int16_t temp = 0;

    // has critical sections inside
    BSP_MPU9150_readMultiReg(inDevice,
            BSP_MPU9150_REG_ACCEL_XOUT_H,
            BSP_MPU9150_READ_DATA_SIZE_IN_BYTES,
            &byteBuffer[0],
            &mpuErr);

    if(mpuErr == BSP_MPU9150_err_NONE) {
        BSP_MPU9150_calculateAccValues(inDevice, &byteBuffer[BSP_MPU9150_ACC_DATA_TWI_OFFSET], &accVals[0]);
        BSP_MPU9150_calculateTemperature(&byteBuffer[BSP_MPU9150_TEMP_DATA_TWI_OFFSET], &temp);
        BSP_MPU9150_calculateGyroValues(inDevice, &byteBuffer[BSP_MPU9150_GYRO_DATA_TWI_OFFSET], &gyroVals[0]);
        BSP_MPU9150_packDataToSend(&gyroVals[0], &accVals[0], temp, &newValues[0]);
    }

    if(outErr != NULL) {
        *outErr = mpuErr;
    }
}


void nrf_drv_mpu_twi_event_handler(nrf_drv_twi_evt_t const * p_event, void * p_context) {

    switch(p_event->type) {
        case NRF_DRV_TWI_EVT_DONE:
            switch(p_event->xfer_desc.type) {
                case NRF_DRV_TWI_XFER_TX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXTX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_RX:
                    twi_rx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXRX:
                    twi_rx_done = true;
                    break;
                default:
                    break;
            }
            break;
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            break;
        default:
            break;
    }
}

/***************************************************************************************************
 *                          PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function for writing to single register of MPU9150.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [in]  regAddr      - register address.
 * @param [in]  regData      - data to be sent.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    15.05.2021
 **************************************************************************************************/
static void BSP_MPU9150_writeSingleReg(BSP_MPU9150_device_S *inDevice,
        const uint8_t regAddr,
        const uint8_t regData,
        BSP_MPU9150_err_E *outErr) {

    uint32_t err_code;
    BSP_MPU9150_err_E err = BSP_MPU9150_err_NONE;
    volatile uint32_t timeout = BSP_MPU9150_TWI_TIMEOUT;

    // assemble I2C data - 1 byte register address, 1 byte data
    uint8_t msg[BSP_MPU9150_SINGLE_REG_MSG_SIZE] = { regAddr, regData };

    err_code = nrf_drv_twi_tx(inDevice->twiInstance,
            inDevice->config->mpuAddress,
            &msg[0],
            BSP_MPU9150_SINGLE_REG_MSG_SIZE,
            false);

    if(err_code != NRF_SUCCESS) {
        err = BSP_MPU9150_err_I2C_READ_WRITE;
    }

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    while((!twi_tx_done) && --timeout);

    if(!timeout) {
        err = BSP_MPU9150_err_I2C_TIMEOUT;
    }

    twi_tx_done = false;

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Function for reading from single register of MPU9150.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [in]  regAddr      - register address.
 * @param [in]  *data        - pointer to data to read.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    15.05.2021
 **************************************************************************************************/
static void BSP_MPU9150_readSingleReg(BSP_MPU9150_device_S *inDevice,
        const uint8_t regAddr,
        uint8_t *data,
        BSP_MPU9150_err_E *outErr) {

    uint32_t err_code;
    BSP_MPU9150_err_E err = BSP_MPU9150_err_NONE;
    volatile uint32_t timeout = BSP_MPU9150_TWI_TIMEOUT;

    // after first START condition send |SLAVE_ADDR+W|REG_ADDR|
    err_code = nrf_drv_twi_tx(inDevice->twiInstance,
            inDevice->config->mpuAddress,
            &regAddr,
            1u,
            false);

    if(err_code != NRF_SUCCESS) {
        err = BSP_MPU9150_err_I2C_READ_WRITE;
    }

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    while((!twi_tx_done) && --timeout);

    if(!timeout) {
        err = BSP_MPU9150_err_I2C_TIMEOUT;
    }
    twi_tx_done = false;

    // after second START condition read in data from register
    err_code = nrf_drv_twi_rx(inDevice->twiInstance,
            inDevice->config->mpuAddress,
            data,
            1u);

    if(err_code != NRF_SUCCESS) {
        err = BSP_MPU9150_err_I2C_READ_WRITE;
    }

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    timeout = BSP_MPU9150_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);

    if(!timeout) {
        err = BSP_MPU9150_err_I2C_TIMEOUT;
    }
    twi_rx_done = false;

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Function for reading from multiple successive registers of MPU9150.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for MPU-9150 driver.
 * @param [in]  startRegAddr - starting register address.
 * @param [in]  length       - number of registers (bytes) to read.
 * @param [in]  *data        - pointer to data to read.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    15.05.2021
 **************************************************************************************************/
static void BSP_MPU9150_readMultiReg(BSP_MPU9150_device_S *inDevice,
        const uint8_t startRegAddr,
        const uint8_t length,
        uint8_t *data,
        BSP_MPU9150_err_E *outErr) {

    uint32_t err_code;
    BSP_MPU9150_err_E err = BSP_MPU9150_err_NONE;
    volatile uint32_t timeout = BSP_MPU9150_TWI_TIMEOUT;

    // after first START condition send |SLAVE_ADDR+W|START_REG_ADDR|
    err_code = nrf_drv_twi_tx(inDevice->twiInstance,
            inDevice->config->mpuAddress,
            &startRegAddr,
            1u,
            false);

    if(err_code != NRF_SUCCESS) {
        err = BSP_MPU9150_err_I2C_READ_WRITE;
    }

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    while((!twi_tx_done) && --timeout);

    if(!timeout) {
        err = BSP_MPU9150_err_I2C_TIMEOUT;
    }
    twi_tx_done = false;

    // after second START condition read in data from registers
    err_code = nrf_drv_twi_rx(inDevice->twiInstance,
            inDevice->config->mpuAddress,
            data,
            length);

    if(err_code != NRF_SUCCESS) {
        err = BSP_MPU9150_err_I2C_READ_WRITE;
    }

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    timeout = BSP_MPU9150_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);

    if(!timeout) {
        err = BSP_MPU9150_err_I2C_TIMEOUT;
    }
    twi_rx_done = false;

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Function calculates temperature value in degrees using datasheet conversion function.
 ***************************************************************************************************
 * @param  [in]  rawValue     -   pointer to raw temperature value, read from register.
 * @param  [out] *outTemp     -   pointer to output temperature data.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    25.05.2021
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_calculateTemperature(const uint8_t *rawValue, int16_t *outTemp) {

    int16_t retVal = 0;

    if((rawValue != NULL) && (outTemp != NULL)) {
        retVal = rawValue[1] + (rawValue[0] << 8u);
        retVal = retVal / BSP_MPU9150_TEMP_CONVERSION_CONST_1 + BSP_MPU9150_TEMP_CONVERSION_CONST_2;
        *outTemp = retVal;
    }
}

/***********************************************************************************************//**
 * @brief Function calculates gyroscope value using datasheet conversion function.
 ***************************************************************************************************
 * @param  [in]  *inDevice    - pointer to device structure for MPU-9150 driver.
 * @param  [in]  *rawValue    - pointer to raw gyroscope measurements, read from register in bytes.
 * @param  [out] *outGyro     - pointer to calculated gyroscope values in degree/seconds.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    25.05.2021
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_calculateGyroValues(const BSP_MPU9150_device_S *inDevice,
        const uint8_t *rawValue,
        int16_t *outGyro) {

    int16_t gyroX = 0;
    int16_t gyroY = 0;
    int16_t gyroZ = 0;

    float floatGyroX = 0.f;
    float floatGyroY = 0.f;
    float floatGyroZ = 0.f;

    if((inDevice != NULL) && (rawValue != NULL) && (outGyro != NULL)) {
        gyroX = rawValue[1] + (rawValue[0] << 8u);
        gyroY = rawValue[3] + (rawValue[2] << 8u);
        gyroZ = rawValue[5] + (rawValue[4] << 8u);

        // this will give the real (interpreted) value in degree/seconds
        switch(inDevice->config->gyroRange) {
            default:
            case BSP_MPU9150_gyroFsRange_250degS:
                floatGyroX = ((float) gyroX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_250DEG_CONST;
                floatGyroY = ((float) gyroY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_250DEG_CONST;
                floatGyroZ = ((float) gyroZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_250DEG_CONST;
                break;
            case BSP_MPU9150_gyroFsRange_500degS:
                floatGyroX = ((float) gyroX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_500DEG_CONST;
                floatGyroY = ((float) gyroY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_500DEG_CONST;
                floatGyroZ = ((float) gyroZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_500DEG_CONST;
                break;
            case BSP_MPU9150_gyroFsRange_1000degS:
                floatGyroX = ((float) gyroX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_1000DEG_CONST;
                floatGyroY = ((float) gyroY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_1000DEG_CONST;
                floatGyroZ = ((float) gyroZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_1000DEG_CONST;
                break;
            case BSP_MPU9150_gyroFsRange_2000degS:
                floatGyroX = ((float) gyroX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_2000DEG_CONST;
                floatGyroY = ((float) gyroY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_2000DEG_CONST;
                floatGyroZ = ((float) gyroZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_GYRO_RANGE_2000DEG_CONST;
                break;
        }

        // output it as 16-bit values
        gyroX = (int16_t) floatGyroX;
        gyroY = (int16_t) floatGyroY;
        gyroZ = (int16_t) floatGyroZ;

        // X-axis Gyro
        outGyro[0] = gyroX;
        // Y-axis Gyro
        outGyro[1] = gyroY;
        // Z-axis Gyro
        outGyro[2] = gyroZ;
    }
}

/***********************************************************************************************//**
 * @brief Function calculates accelerometer value using datasheet conversion function.
 ***************************************************************************************************
 * @param  [in]  *inDevice    - pointer to device structure for MPU-9150 driver.
 * @param  [in]  *rawValue    - pointer to raw gyroscope measurements, read from register in bytes.
 * @param  [out] *outAcc      - pointer to calculated accelerometer values in G.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    25.05.2021
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_calculateAccValues(const BSP_MPU9150_device_S *inDevice,
        const uint8_t *rawValue,
        int16_t *outAcc) {

    int16_t accX = 0;
    int16_t accY = 0;
    int16_t accZ = 0;

    float floatAccX = 0.f;
    float floatAccY = 0.f;
    float floatAccZ = 0.f;

    if((inDevice != NULL) && (rawValue != NULL) && (outAcc != NULL)) {
        accX = rawValue[1] + (rawValue[0] << 8u);
        accY = rawValue[3] + (rawValue[2] << 8u);
        accZ = rawValue[5] + (rawValue[4] << 8u);

        // this will give the real (interpreted) value in G
        switch(inDevice->config->accRange) {
            default:
            case BSP_MPU9150_accFsRange_2G:
                floatAccX = ((float) accX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_2G_CONST;
                floatAccY = ((float) accY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_2G_CONST;
                floatAccZ = ((float) accZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_2G_CONST;
                break;
            case BSP_MPU9150_accFsRange_4G:
                floatAccX = ((float) accX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_4G_CONST;
                floatAccY = ((float) accY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_4G_CONST;
                floatAccZ = ((float) accZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_4G_CONST;
                break;
            case BSP_MPU9150_accFsRange_8G:
                floatAccX = ((float) accX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_8G_CONST;
                floatAccY = ((float) accY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_8G_CONST;
                floatAccZ = ((float) accZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_8G_CONST;
                break;
            case BSP_MPU9150_accFsRange_16G:
                floatAccX = ((float) accX / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_16G_CONST;
                floatAccY = ((float) accY / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_16G_CONST;
                floatAccZ = ((float) accZ / BSP_MPU9150_16_BIT_SIGNED_MAX_VAL_FLOAT) * BSP_MPU9150_ACC_RANGE_16G_CONST;
                break;
        }

        // output values in mG, that way they can be stored as 16-bit
        accX = (int16_t) (floatAccX * 1000);
        accY = (int16_t) (floatAccY * 1000);
        accZ = (int16_t) (floatAccZ * 1000);

        // X-axis Accelerometer
        outAcc[0] = accX;
        // Y-axis Accelerometer
        outAcc[1] = accY;
        // Z-axis Accelerometer
        outAcc[2] = accZ;
    }
}

/***********************************************************************************************//**
 * @brief Function calculates temperature value in degrees using datasheet conversion function.
 ***************************************************************************************************
 * @param  [in]  *gyroVals      -   pointer to 16-bit interpreted gyro data (deg/s).
 * @param  [in]  *accVals       -   pointer to 16-bit interpreted accelerometer data (mG).
 * @param  [in]  temp           -   interpreted temperature value.
 * @param  [out] *outBuffer     -   pointer to output buffer which is sent over BLE.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    25.05.2021
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_packDataToSend(const int16_t *gyroVals,
        const int16_t *accVals,
        const int16_t temp,
        int16_t *outBuffer) {

    if((gyroVals != NULL) && (accVals != NULL) && (outBuffer != NULL)) {
        outBuffer[0] = gyroVals[0];
        outBuffer[1] = gyroVals[1];
        outBuffer[2] = gyroVals[2];

        outBuffer[3] = accVals[0];
        outBuffer[4] = accVals[1];
        outBuffer[5] = accVals[2];

        outBuffer[6] = temp;
    }
}

/***********************************************************************************************//**
 * @brief Function waits (blocking) for I2C RX transfer finish or timeout.
 ***************************************************************************************************
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    15.05.2021
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_waitRxTransferFinish(BSP_MPU9150_err_E *outErr) {

    uint32_t timeout = BSP_MPU9150_TWI_TIMEOUT;

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    while((!twi_rx_done) && --timeout);

    if(!timeout) {
        *outErr = BSP_MPU9150_err_I2C_TIMEOUT;
    }
}

/***********************************************************************************************//**
 * @brief Function waits (blocking) for I2C TX transfer finish or timeout.
 ***************************************************************************************************
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    15.05.2021
 **************************************************************************************************/
__INLINE static void BSP_MPU9150_waitTxTransferFinish(BSP_MPU9150_err_E *outErr) {

    uint32_t timeout = BSP_MPU9150_TWI_TIMEOUT;

    // TODO: add proper timeout check with timer
    // wait for transfer finish or timeout
    while((!twi_tx_done) && --timeout);

    if(!timeout) {
        *outErr = BSP_MPU9150_err_I2C_TIMEOUT;
    }
}

/***********************************************************************************************//**
 * @brief Function sets up configuration registers for MPU-9150.
 ***************************************************************************************************
 * @param [in]  *inDevice    - pointer to device structure for ECG driver.
 * @param [out] *outErr      - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.05.2021
 **************************************************************************************************/
static void BSP_MPU9150_configuration(BSP_MPU9150_device_S *inDevice, BSP_MPU9150_err_E *outErr) {

    BSP_MPU9150_err_E err = BSP_MPU9150_err_NONE;
    BSP_MPU9150_configReg_U configRegVal = { .R = 0u };
    BSP_MPU9150_gyroConfigReg_U gyroConfigReg = { .R = 0u };
    BSP_MPU9150_accConfigReg_U accConfigReg = { .R = 0u };
    BSP_MPU9150_intConfigReg_U intConfigReg = { .R = 0u };
    BSP_MPU9150_fifoConfigReg_U fifoConfigReg = { .R = 0u };
    BSP_MPU9150_intEnableReg_U intEnableReg = { .R = 0u };

    if(inDevice != NULL) {
        /*
         * Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV), where Gyroscope Output Rate
         * can be either 8kHz when DLPF is disabled, or 1kHz when DLPF is enabled
         */
        uint8_t samplingRateRegVal = 9u;
        BSP_MPU9150_writeSingleReg(inDevice,
                BSP_MPU9150_REG_SMPLRT_DIV,
                samplingRateRegVal,
                &err);

        /*
         * Configuration register sets up DLPF and sampling of External FSYNC pin
         */
        if(err == BSP_MPU9150_err_NONE ) {
            // enables/disables DLPF
            configRegVal.B.dlpfCfg = true;
            configRegVal.B.extSyncSet = false;

            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_CONFIG,
                    configRegVal.R,
                    &err);
        }

        /*
         * FIFO enable register sets which sensor output will be written to FIFO buffer
         */
        if(err == BSP_MPU9150_err_NONE ) {
//            fifoConfigReg.B.accelFifoEn = true;
//            fifoConfigReg.B.tempFifoEn = true;
//            fifoConfigReg.B.xgFifoEn = true;
//            fifoConfigReg.B.ygFifoEn = true;
//            fifoConfigReg.B.zgFifoEn = true;
//
            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_FIFO_EN,
                    fifoConfigReg.R,
                    &err);
        }

        /*
         * Interrupt configuration sets up interrupt behavior
         */
        if(err == BSP_MPU9150_err_NONE ) {
            intConfigReg.B.fsyncIntEn = false;
            intConfigReg.B.fsyncIntLevel = false;
            intConfigReg.B.i2cBypassEn = false;
            intConfigReg.B.intLevel = false;
            intConfigReg.B.intOpen = false;
            // clear interrupt status on any read operation
            intConfigReg.B.intRdClear = true;
            intConfigReg.B.latchIntEn = false;

            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_INT_PIN_CFG,
                    intConfigReg.R,
                    &err);
        }

        /*
         * Setup gyroscope configuration
         */
        if(err == BSP_MPU9150_err_NONE ) {
            gyroConfigReg.B.fsSel = (uint8_t) inDevice->config->gyroRange;
            gyroConfigReg.B.xg_st = false;
            gyroConfigReg.B.yg_st = false;
            gyroConfigReg.B.zg_st = false;

            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_GYRO_CONFIG,
                    gyroConfigReg.R,
                    &err);
        }

        /*
         * Setup accelerometer configuration
         */
        if(err == BSP_MPU9150_err_NONE ) {
            accConfigReg.B.afsSel = (uint8_t) inDevice->config->accRange;
            accConfigReg.B.xa_st = false;
            accConfigReg.B.ya_st = false;
            accConfigReg.B.za_st = false;

            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_ACCEL_CONFIG,
                    accConfigReg.R,
                    &err);
        }

        /*
         * Interrupt enable setup
         */
        if(err == BSP_MPU9150_err_NONE ) {
            intEnableReg.B.i2cMstIntEn = false;
            intEnableReg.B.fifoOverflowEn = false;
            // enable DATA READY interrupt signal on pin
            intEnableReg.B.dataRdyEn = true;

            BSP_MPU9150_writeSingleReg(inDevice,
                    BSP_MPU9150_REG_INT_ENABLE,
                    intEnableReg.R,
                    &err);
        }
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
