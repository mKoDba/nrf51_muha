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
 * @file    ble_muha.c
 * @author  mario.kodba
 * @brief   BLE initialization and start functions for NRF51 MUHA board source file.
 **************************************************************************************************/
/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "ble_muha.h"
#include "nrf51_muha.h"
#include "hal_watchdog.h"
#include "SEGGER_RTT.h"
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_conn_params.h"
#include "ble_advertising.h"

#include "ble_bas.h"
#include "ble_ecgs.h"

#include "cfg_ble_muha.h"
#include "cfg_bsp_ecg_ADS1192.h"
#include "cfg_nrf51_muha_pinout.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define BLE_MUHA_CENTRAL_LINK_COUNT              (0)    /*!< Number of central links used by the application.
                                                             When changing this number remember to adjust the RAM settings */
#define BLE_MUHA_PERIPHERAL_LINK_COUNT           (1)    /*!< Number of peripheral links used by the application.
                                                             When changing this number remember to adjust the RAM settings */

ble_bas_t m_bas;                                        //!< Structure used to identify the battery service.

volatile uint8_t muhaConnected = false;                          //!< Flag which shows status of BLE connection of MUHA board.
volatile uint8_t muhaEcgNotificationEnabled = false;             //!< Flag which shows if the device connected to MUHA board has BLE ECG notification enabled.
volatile uint8_t muhaMpuNotificationEnabled = false;             //!< Flag which shows if the device connected to MUHA board has BLE MPU notification enabled.
volatile uint8_t muhaBleTxBufferAvailable   = true;              //!< Flag which shows if there is BLE TX buffer available.

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void BLE_MUHA_bleStackInit(ERR_E *err);
static void BLE_MUHA_gapInit(ERR_E *err);
static void BLE_MUHA_servicesInit(ERR_E *err);
static void BLE_MUHA_advertisingInit(ERR_E *err);
static void BLE_MUHA_onEcgsEvent(BLE_ECGS_custom_S *customService, BLE_ECGS_evt_S *event);

/***************************************************************************************************
 *                         PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/

/***********************************************************************************************//**
 * @brief Initializes everything needed for BLE functionality.
 ***************************************************************************************************
 * @param [out] *error - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void BLE_MUHA_init(ERR_E *error) {

    ERR_E localErr = ERR_NONE;

    // BLE stack + SoftDevice initialization
    BLE_MUHA_bleStackInit(&localErr);

    if(localErr == ERR_NONE) {
        // GAP parameters initialization
        BLE_MUHA_gapInit(&localErr);
    }

    if(localErr == ERR_NONE) {
        // services initialization
        BLE_MUHA_servicesInit(&localErr);
    }

    if(localErr == ERR_NONE) {
        // advertising parameters initialization
        BLE_MUHA_advertisingInit(&localErr);
    }

    if(error != NULL) {
        *error = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Function starts BLE advertising.
 ***************************************************************************************************
 * @param [out] *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
void BLE_MUHA_advertisingStart(ERR_E *err) {

    uint32_t nrfErrCode = NRF_SUCCESS;

    // if no error is returned will start BLE advertising
    nrfErrCode = sd_ble_gap_adv_start(&bleAdvertisingParams);

    if(err != NULL) {
        if(nrfErrCode != NRF_SUCCESS) {
            *err = ERR_BLE_ADVERTISING_START_FAIL;
        }
    }
}

/***********************************************************************************************//**
 * @brief Callback function for BLE event handling.
 ***************************************************************************************************
 * @param [in] *bleEvent - pointer to BLE event received from SoftDevice.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    10.04.2021.
 **************************************************************************************************/
void BLE_MUHA_bleEventCallback(ble_evt_t *bleEvent) {

    BLE_ECGS_onBleEvt(bleEvent, &customService);

    // in case of disconnect, start advertising again
    if(bleEvent->header.evt_id == BLE_GAP_EVT_DISCONNECTED) {
        BLE_MUHA_advertisingStart(NULL);
    }
}

/***************************************************************************************************
 *                          PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function initializes SoftDevice handler and BLE stack.
 ***************************************************************************************************
 * @param [out] *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static void BLE_MUHA_bleStackInit(ERR_E *err) {

    uint32_t nrfErrCode = NRF_SUCCESS;

    // initialize the SoftDevice handler module
    SOFTDEVICE_HANDLER_INIT(&bleLfClockXTAL, NULL);

    ble_enable_params_t ble_enable_params;
    nrfErrCode = softdevice_enable_get_default_config(BLE_MUHA_CENTRAL_LINK_COUNT,
            BLE_MUHA_PERIPHERAL_LINK_COUNT,
            &ble_enable_params);

    // enable BLE stack
    if(nrfErrCode == NRF_SUCCESS) {
        nrfErrCode = softdevice_enable(&ble_enable_params);
    }

    // setup callback on BLE event
    if(nrfErrCode == NRF_SUCCESS) {
        nrfErrCode = softdevice_ble_evt_handler_set(BLE_MUHA_bleEventCallback);
    }

    if(err != NULL) {
        if(nrfErrCode != NRF_SUCCESS) {
            *err = ERR_BLE_STACK_INIT_FAIL;
        }
    }
}

/***********************************************************************************************//**
 * @brief Function initializes GAP (Generic Access Profile).
 * @details Sets GAP parameters: device name, appearance, preferred connection parameters.
 ***************************************************************************************************
 * @param [out] *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static void BLE_MUHA_gapInit(ERR_E *err) {

    uint32_t nrfErrCode = NRF_SUCCESS;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    nrfErrCode = sd_ble_gap_device_name_set(&sec_mode,
            (const uint8_t *) DEVICE_NAME,
            strlen(DEVICE_NAME));

    if(nrfErrCode == NRF_SUCCESS) {
        // set appearance icon for BLE
        nrfErrCode = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_CYCLING);
    }

    if(nrfErrCode == NRF_SUCCESS) {
        // sets TX power level
        nrfErrCode = sd_ble_gap_tx_power_set((int8_t) BLE_MUHA_TX_POWER_LVL);
    }

    if(nrfErrCode == NRF_SUCCESS) {
        // sets preferred connection parameters
        nrfErrCode = sd_ble_gap_ppcp_set(&gapConnectionParams);
    }

    if(err != NULL) {
        if(nrfErrCode != NRF_SUCCESS) {
            *err = ERR_BLE_GAP_INIT_FAIL;
        }
    }
}

/***********************************************************************************************//**
 * @brief Function sets up BLE services used in application.
 * @details Sets Device Information Service (DIS).
 ***************************************************************************************************
 * @param [out] *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    10.04.2021.
 **************************************************************************************************/
static void BLE_MUHA_servicesInit(ERR_E *err) {

    ERR_E localErr = ERR_NONE;
    BLE_ECGS_err_E customServiceErr = BLE_ECGS_err_NONE;
    uint32_t nrfErrCode = NRF_SUCCESS;
    BLE_ECGS_customInit_S ecgs_init;
    ble_bas_init_t bas_init;

    // initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));

    // here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    nrfErrCode = ble_bas_init(&m_bas, &bas_init);

    if(nrfErrCode != NRF_SUCCESS) {
        localErr = ERR_BLE_GAP_INIT_FAIL;
    }

     // initialize custom ECG service init structure to zero.
    memset(&ecgs_init, 0, sizeof(ecgs_init));

    ecgs_init.evt_handler = BLE_MUHA_onEcgsEvent;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ecgs_init.custom_value_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ecgs_init.custom_value_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ecgs_init.custom_value_char_attr_md.cccd_write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ecgs_init.mpu_data_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ecgs_init.mpu_data_char_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ecgs_init.mpu_data_char_attr_md.cccd_write_perm);

    if(localErr == ERR_NONE) {
        BLE_ECGS_init(&customService, &ecgs_init, &customServiceErr);
    }

    if(customServiceErr != BLE_ECGS_err_NONE) {
        localErr = ERR_BLE_SERVICE_INIT_FAIL;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Function initializes starting BLE advertising data.
 ***************************************************************************************************
 * @param [out] *err - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    03.04.2021.
 **************************************************************************************************/
static void BLE_MUHA_advertisingInit(ERR_E *err) {

    uint32_t nrfErrCode = NRF_SUCCESS;
    ble_advdata_t advdata;
    int8_t tx_power_lvl = BLE_MUHA_TX_POWER_LVL;

    // 31 bytes is maximum for advertising/scan response data (BLE_GAP_ADV_MAX_SIZE)
    uint8_t bytesLeft = BLE_GAP_ADV_MAX_SIZE;

    /*
     * Advertising Data element consists of:
     * - length byte (1 byte)
     * - AD type (1 byte)
     * - data itself (1 or more bytes)
     */
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    // "MK_MUHA" - 8 characters including null termination
    bytesLeft = bytesLeft - 2u - 8u;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    // flag is 1 data byte only
    bytesLeft = bytesLeft - 2u - 1u;
    advdata.p_tx_power_level = (int8_t *) &tx_power_lvl;
    // TX power is 1 data byte only
    bytesLeft = bytesLeft - 2u - 1u;
    advdata.include_appearance = true;

    // setting BLE advertising data, scan response not set
    nrfErrCode = ble_advdata_set(&advdata, NULL);

    if(err != NULL) {
        if(nrfErrCode != NRF_SUCCESS) {
            *err = ERR_BLE_ADVERTISING_DATA_INIT_FAIL;
        }
    }
}

/***********************************************************************************************//**
 * @brief Called on BLE custom service event.
 * @details Turns off LED 1 in case of BLE disconnect, turns on on BLE connect, toggles it if
 *          SoftDevice working and communicating (sending events) to user application.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom service structure.
 * @param [in]  event           - Pointer to custom service event type.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_MUHA_onEcgsEvent(BLE_ECGS_custom_S *customService, BLE_ECGS_evt_S *event) {

    switch(event->evt_type) {
        case BLE_ECGS_EVT_ECG_NOTIFICATION_ENABLED:
            muhaEcgNotificationEnabled = true;
            break;

        case BLE_ECGS_EVT_ECG_NOTIFICATION_DISABLED:
            muhaEcgNotificationEnabled = false;
            break;

        case BLE_ECGS_EVT_MPU_NOTIFICATION_ENABLED:
            muhaMpuNotificationEnabled = true;
            break;

        case BLE_ECGS_EVT_MPU_NOTIFICATION_DISABLED:
            muhaMpuNotificationEnabled = false;
            break;

        case BLE_ECGS_EVT_CONNECTED:
            muhaConnected = true;
            break;

        case BLE_ECGS_EVT_DISCONNECTED:
            muhaConnected = false;
            break;

        default:
            break;
    }
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/


