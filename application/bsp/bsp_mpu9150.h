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
 * @file    bsp_mpu_9150.h
 * @author  mario.kodba
 * @brief   MPU-9150 MEMS header file.
 **************************************************************************************************/

#ifndef BSP_MPU9150_H_
#define BSP_MPU9150_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "nrf_drv_twi.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
//! If set to true uses twi_hw_master drivers (workaround in order for TWI to work with SoftDevice) instead of nrf_twi drivers
#define DEPRECATED_TWI      true

#define BSP_MPU9150_SENSOR_DATA_INT16_SIZE  (7u)    //!< Number of 16-bit (sensor) data stored to device data buffer

/**********************************************************
*    MPU-9150 Gyroscope and Accelerometer register map    *
**********************************************************/
// MPU-9150 self-test registers
#define BSP_MPU9150_REG_SELF_TEST_X         (0x0Du)
#define BSP_MPU9150_REG_SELF_TEST_Y         (0x0Eu)
#define BSP_MPU9150_REG_SELF_TEST_Z         (0x0Fu)
#define BSP_MPU9150_REG_SELF_TEST_A         (0x10u)

// MPU-9150 configuration registers
#define BSP_MPU9150_REG_SMPLRT_DIV          (0x19u)
#define BSP_MPU9150_REG_CONFIG              (0x1Au)
#define BSP_MPU9150_REG_GYRO_CONFIG         (0x1Bu)
#define BSP_MPU9150_REG_ACCEL_CONFIG        (0x1Cu)
#define BSP_MPU9150_REG_FIFO_EN             (0x23u)
#define BSP_MPU9150_REG_I2C_MST_CTRL        (0x24u)
#define BSP_MPU9150_REG_SIGNAL_PATH_RESET   (0x68u)
#define BSP_MPU9150_REG_USER_CTRL           (0x6Au)
#define BSP_MPU9150_REG_PWR_MGMT_1          (0x6Bu)
#define BSP_MPU9150_REG_PWR_MGMT_2          (0x6Cu)
#define BSP_MPU9150_REG_FIFO_COUNTH         (0x72u)
#define BSP_MPU9150_REG_FIFO_COUNTL         (0x73u)
#define BSP_MPU9150_REG_FIFO_R_W            (0x74u)
#define BSP_MPU9150_REG_WHO_AM_I            (0x75u)

// MPU-9150 I2C slaves (external sensors) registers
#define BSP_MPU9150_REG_I2C_SLV0_ADDR       (0x25u)
#define BSP_MPU9150_REG_I2C_SLV0_REG        (0x26u)
#define BSP_MPU9150_REG_I2C_SLV0_CTRL       (0x27u)
#define BSP_MPU9150_REG_I2C_SLV1_ADDR       (0x28u)
#define BSP_MPU9150_REG_I2C_SLV1_REG        (0x29u)
#define BSP_MPU9150_REG_I2C_SLV1_CTRL       (0x2Au)
#define BSP_MPU9150_REG_I2C_SLV2_ADDR       (0x2Bu)
#define BSP_MPU9150_REG_I2C_SLV2_REG        (0x2Cu)
#define BSP_MPU9150_REG_I2C_SLV2_CTRL       (0x2Du)
#define BSP_MPU9150_REG_I2C_SLV3_ADDR       (0x2Eu)
#define BSP_MPU9150_REG_I2C_SLV3_REG        (0x2Fu)
#define BSP_MPU9150_REG_I2C_SLV3_CTRL       (0x30u)
#define BSP_MPU9150_REG_I2C_SLV4_ADDR       (0x31u)
#define BSP_MPU9150_REG_I2C_SLV4_REG        (0x32u)
#define BSP_MPU9150_REG_I2C_SLV4_DO         (0x33u)
#define BSP_MPU9150_REG_I2C_SLV4_CTRL       (0x34u)
#define BSP_MPU9150_REG_I2C_SLV4_DI         (0x35u)
#define BSP_MPU9150_REG_I2C_MST_STATUS      (0x36u)
#define BSP_MPU9150_REG_I2C_SLV0_DO         (0x63u)
#define BSP_MPU9150_REG_I2C_SLV1_DO         (0x64u)
#define BSP_MPU9150_REG_I2C_SLV2_DO         (0x65u)
#define BSP_MPU9150_REG_I2C_SLV3_DO         (0x66u)
#define BSP_MPU9150_REG_I2C_MST_DELAY_CTRL  (0x67u)

// MPU-9150 interrupt registers
#define BSP_MPU9150_REG_INT_PIN_CFG         (0x37u)
#define BSP_MPU9150_REG_INT_ENABLE          (0x38u)
#define BSP_MPU9150_REG_INT_STATUS          (0x3Au)

// Accelerometer measurement data registers
#define BSP_MPU9150_REG_ACCEL_XOUT_H        (0x3Bu)
#define BSP_MPU9150_REG_ACCEL_XOUT_L        (0x3Cu)
#define BSP_MPU9150_REG_ACCEL_YOUT_H        (0x3Du)
#define BSP_MPU9150_REG_ACCEL_YOUT_L        (0x3Eu)
#define BSP_MPU9150_REG_ACCEL_ZOUT_H        (0x3Fu)
#define BSP_MPU9150_REG_ACCEL_ZOUT_L        (0x40u)

// Temperature measurement data registers
#define BSP_MPU9150_REG_TEMP_OUT_H          (0x41u)
#define BSP_MPU9150_REG_TEMP_OUT_L          (0x42u)

// Gyroscope measurement data registers
#define BSP_MPU9150_REG_GYRO_XOUT_H         (0x43u)
#define BSP_MPU9150_REG_GYRO_XOUT_L         (0x44u)
#define BSP_MPU9150_REG_GYRO_YOUT_H         (0x45u)
#define BSP_MPU9150_REG_GYRO_YOUT_L         (0x46u)
#define BSP_MPU9150_REG_GYRO_ZOUT_H         (0x47u)
#define BSP_MPU9150_REG_GYRO_ZOUT_L         (0x48u)

/**********************************************************
*       MPU-9150 Magnetometer (AK8975C) register map      *
**********************************************************/
#define BSP_MPU9150_REG_MAG_WIA             (0x00u)
#define BSP_MPU9150_REG_MAG_INFO            (0x01u)
#define BSP_MPU9150_REG_MAG_ST1             (0x02u)

// Magnetometer measurement data registers
#define BSP_MPU9150_REG_MAG_HXL             (0x03u)
#define BSP_MPU9150_REG_MAG_HXH             (0x04u)
#define BSP_MPU9150_REG_MAG_HYL             (0x05u)
#define BSP_MPU9150_REG_MAG_HYH             (0x06u)
#define BSP_MPU9150_REG_MAG_HZL             (0x07u)
#define BSP_MPU9150_REG_MAG_HZH             (0x08u)

#define BSP_MPU9150_REG_MAG_ST2             (0x09u)
#define BSP_MPU9150_REG_MAG_CNTL            (0x0Au)
#define BSP_MPU9150_REG_MAG_ASTC            (0x0Cu)
#define BSP_MPU9150_REG_MAG_I2CDIS          (0x0Fu)

// Sensitivity adjustment registers - can be read only in Fuse access mode
#define BSP_MPU9150_REG_MAG_ASAX            (0x10u)
#define BSP_MPU9150_REG_MAG_ASAY            (0x11u)
#define BSP_MPU9150_REG_MAG_ASAZ            (0x12u)

/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/
//! MPU9150 error enumeration
typedef enum BSP_MPU9150_err_ENUM {
    BSP_MPU9150_err_NONE            = 0u,   //!< No error
    BSP_MPU9150_err_NULL_PARAM,             //!< NULL parameter error
    BSP_MPU9150_err_INIT,                   //!< Error on initialization
    BSP_MPU9150_err_I2C_READ_WRITE,         //!< I2C operation error
    BSP_MPU9150_err_I2C_TIMEOUT,            //!< I2C timeout error

} BSP_MPU9150_err_E;

//! MPU9150 accelerometer full scale range
typedef enum BSP_MPU9150_accFsRange_ENUM {
    BSP_MPU9150_accFsRange_2G   = 0u,       //!< Accelerometer +-2G full-scale range
    BSP_MPU9150_accFsRange_4G,              //!< Accelerometer +-4G full-scale range
    BSP_MPU9150_accFsRange_8G,              //!< Accelerometer +-8G full-scale range
    BSP_MPU9150_accFsRange_16G              //!< Accelerometer +-16G full-scale range
} BSP_MPU9150_accFsRange_E;

//! MPU9150 gyroscope full scale range
typedef enum BSP_MPU9150_gyroFsRange_ENUM {
    BSP_MPU9150_gyroFsRange_250degS   = 0u, //!< Gyroscope +-250 DPS full-scale range
    BSP_MPU9150_gyroFsRange_500degS,        //!< Gyroscope +-500 DPS full-scale range
    BSP_MPU9150_gyroFsRange_1000degS,       //!< Gyroscope +-1000 DPS full-scale range
    BSP_MPU9150_gyroFsRange_2000degS        //!< Gyroscope +-2000 DPS full-scale range
} BSP_MPU9150_gyroFsRange_E;

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! General configuration MPU9150 register
typedef union BSP_MPU9150_configReg_UNION {
    uint8_t R;                                  //!< Configuration register value
    struct {
       uint8_t  dlpfCfg     : 3;                //!< Digital Low Pass Filter setting
       uint8_t  extSyncSet  : 3;                //!< Configures FSYNC pin sampling setting
       uint8_t              : 2;                //!< Not used
    } B;                                        //!< Configuration register bits
} BSP_MPU9150_configReg_U;

//! Gyroscope configuration MPU9150 register
typedef union BSP_MPU9150_gyroConfigReg_UNION {
    uint8_t R;                                  //!< Gyroscope configuration register value
    struct {
       uint8_t              : 3;                //!< Not used
       uint8_t  fsSel       : 2;                //!< Configures gyroscope full-scale range
       uint8_t  zg_st       : 1;                //!< Set Z-axis gyroscope self-test bit
       uint8_t  yg_st       : 1;                //!< Set Y-axis gyroscope self-test bit
       uint8_t  xg_st       : 1;                //!< Set X-axis gyroscope self-test bit
    } B;                                        //!< Gyroscope configuration register bits
} BSP_MPU9150_gyroConfigReg_U;

//! Accelerometer configuration MPU9150 register
typedef union BSP_MPU9150_accConfigReg_UNION {
    uint8_t R;                                  //!< Accelerometer configuration register value
    struct {
       uint8_t              : 3;                //!< Not used
       uint8_t  afsSel      : 2;                //!< Configures accelerometer full-scale range
       uint8_t  za_st       : 1;                //!< Set Z-axis accelerometer self-test bit
       uint8_t  ya_st       : 1;                //!< Set Y-axis accelerometer self-test bit
       uint8_t  xa_st       : 1;                //!< Set X-axis accelerometer self-test bit
    } B;                                        //!< Accelerometer configuration register bits
} BSP_MPU9150_accConfigReg_U;

//! MPU9150 FIFO buffer configuration register
typedef union BSP_MPU9150_fifoConfigReg_UNION {
    uint8_t R;                                  //!< FIFO buffer configuration register value
    struct {
       uint8_t              : 3;                //!< Not used on nRF51 MUHA board
       uint8_t  accelFifoEn : 1;                //!< Set accelerometer FIFO enable
       uint8_t  zgFifoEn    : 1;                //!< Set Z-axis gyroscope FIFO enable
       uint8_t  ygFifoEn    : 1;                //!< Set Y-axis gyroscope FIFO enable
       uint8_t  xgFifoEn    : 1;                //!< Set X-axis gyroscope FIFO enable
       uint8_t  tempFifoEn  : 1;                //!< Set temperature FIFO enable
    } B;                                        //!< FIFO buffer configuration register bits
} BSP_MPU9150_fifoConfigReg_U;

//! MPU9150 Interrupt enable register
typedef union BSP_MPU9150_intEnableReg_UNION {
    uint8_t R;                                  //!< Interrupt enable register value
    struct {
       uint8_t  dataRdyEn      : 1;             //!< Enable interrupt when write operation to all sensor registers completed
       uint8_t                 : 2;             //!< Not used
       uint8_t  i2cMstIntEn    : 1;             //!< Enables any of the I2C Master interrupt sources to generate an interrupt
       uint8_t  fifoOverflowEn : 1;             //!< Enable interrupt on FIFO buffer overflow
       uint8_t                 : 3;             //!< Not used
    } B;                                        //!< Interrupt enable register bits
} BSP_MPU9150_intEnableReg_U;

//! MPU9150 Interrupt configuration register
typedef union BSP_MPU9150_intConfigReg_UNION {
    uint8_t R;                                  //!< Interrupt configuration register value
    struct {
       uint8_t                 : 1;             //!< Not used
       uint8_t  i2cBypassEn    : 1;             //!< Enables access of host CPU directly to MPU-9150 auxiliary I2C bus
       uint8_t  fsyncIntEn     : 1;             //!< When 0 - disables interrupt to host CPU, when 1 - enables it
       uint8_t  fsyncIntLevel  : 1;             //!< When 0 - logic level for the FSYNC pin is active high, when 1 - active low
       uint8_t  intRdClear     : 1;             //!< When 0 - interrupt status bits are cleared by reading INT_STATUS, when 1 - cleared by read operation
       uint8_t  latchIntEn     : 1;             //!< When 0 - INT pin emits a 50us long pulse, when 1 - INT pin is held high until the interrupt is cleared
       uint8_t  intOpen        : 1;             //!< When 0 - INT pin is configured as push-pull, when 1 - INT pin is configured as open drain
       uint8_t  intLevel       : 1;             //!< When 0 - logic level for the INT pin is active high, when 1 - logic level for the INT pin is active low
    } B;                                        //!< Interrupt configuration register bits
} BSP_MPU9150_intConfigReg_U;

//! MPU9150 driver configuration structure
typedef struct BSP_MPU9150_config_STRUCT {
    uint8_t mpuAddress;                     //!< MPU-9150 I2C address
    uint8_t mpuMagAddress;                  //!< MPU-9150 magnetometer I2C address
    BSP_MPU9150_gyroFsRange_E gyroRange;    //!< Gyroscope FS range
    BSP_MPU9150_accFsRange_E  accRange;     //!< Accelerometer FS range

} BSP_MPU9150_config_S;

//! MPU9150 driver device structure
typedef struct BSP_MPU9150_device_STRUCT {
    BSP_MPU9150_config_S    *config;        //!< Pointer to MPU9150 driver configuration
    nrf_drv_twi_t           *twiInstance;   //!< Pointer to TWI instance used
    int16_t dataBuffer[BSP_MPU9150_SENSOR_DATA_INT16_SIZE];  //!< Data buffer for sensor data
    bool isInitialized;                     //!< Is device initialized
    volatile bool dataReady;                //!< Is new data ready flag
    volatile bool twiTxDone;                //!< Is TWI TX transfer finished
    volatile bool twiRxDone;                //!< Is TWI RX transfer finished

} BSP_MPU9150_device_S;
/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/


/***************************************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void BSP_MPU9150_init(BSP_MPU9150_device_S *inDevice,
        BSP_MPU9150_config_S *inConfig,
        BSP_MPU9150_err_E *outErr);
void BSP_MPU9150_updateValues(BSP_MPU9150_device_S *inDevice,
        int16_t *newValues,
        BSP_MPU9150_err_E *outErr);

void nrf_drv_mpu_twi_event_handler(nrf_drv_twi_evt_t const *p_event, void *p_context);

void BSP_MPU9150_readSingleMagReg(BSP_MPU9150_device_S *inDevice,
        uint8_t regAddr,
        uint8_t *data,
        BSP_MPU9150_err_E *outErr);
void BSP_MPU9150_readMultiMagReg(BSP_MPU9150_device_S *inDevice,
        uint8_t startRegAddr,
        const uint8_t length,
        uint8_t *data,
        BSP_MPU9150_err_E *outErr);

#endif // #ifndef BSP_MPU9150_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
