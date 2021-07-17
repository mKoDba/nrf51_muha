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
 * @file    ble_ecgs.c
 * @author  mario.kodba
 * @brief   ADS1192 ECG custom BLE service source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "ble_ecgs.h"
#include "ble_muha.h"
#include "bsp_ecg_ADS1192.h"
#include "nrf51_muha.h"
#include "SEGGER_RTT.h"
#include <string.h>

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define BLE_ECGS_ON_WRITE_NOTIFICATION_BYTE_SIZE        (2u)    //!< Number of bytes received in CCCD handle write event for notification.

/***************************************************************************************************
 *                          GLOBAL VARIABLES
 **************************************************************************************************/


/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void BLE_ECGS_ecgDataCharAdd(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        BLE_ECGS_err_E *err);
static void BLE_ECGS_mpuDataCharAdd(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        BLE_ECGS_err_E *err);
static void BLE_ECGS_onConnect(BLE_ECGS_custom_S *customService, ble_evt_t const *p_ble_evt);
static void BLE_ECGS_onWrite(BLE_ECGS_custom_S *customService, ble_evt_t *p_ble_evt);
static void BLE_ECGS_onDisconnect(BLE_ECGS_custom_S *customService, ble_evt_t const *p_ble_evt);

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function initializes custom BLE service, along with its custom characteristics.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom service structure.
 * @param [in]  customInit      - Pointer to custom service initialization structure.
 * @param [out] err             - Pointer to error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
void BLE_ECGS_init(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        BLE_ECGS_err_E *err) {

    BLE_ECGS_err_E localErr = BLE_ECGS_err_NONE;
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    if((customService != NULL) && (customInit != NULL)) {
        // initialize service structure
        customService->conn_handle = BLE_CONN_HANDLE_INVALID;
        // set application event handler
        customService->evt_handler = customInit->evt_handler;

        // add custom service UUID
        ble_uuid128_t base_uuid = { ECG_SERVICE_UUID_BASE };
        err_code = sd_ble_uuid_vs_add(&base_uuid, &customService->uuid_type);

        if(err_code != NRF_SUCCESS) {
            localErr = BLE_ECGS_err_INIT_FAIL;
        }

        if(localErr == BLE_ECGS_err_NONE) {
            ble_uuid.type = customService->uuid_type;
            ble_uuid.uuid = ECG_SERVICE_UUID;
            // add the custom service
            err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &customService->service_handle);
        }

        if (err_code != NRF_SUCCESS) {
            localErr = BLE_ECGS_err_RUNTIME_FAIL;
        }

        if(localErr == BLE_ECGS_err_NONE) {
            // add ECG data characteristics
            BLE_ECGS_ecgDataCharAdd(customService, customInit, &localErr);
        }

        if(localErr == BLE_ECGS_err_NONE) {
            // add MPU data characteristics
            BLE_ECGS_mpuDataCharAdd(customService, customInit, &localErr);
        }
    } else {
        localErr = BLE_ECGS_err_NULL_PARAM;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Callback function for custom service on BLE event.
 ***************************************************************************************************
 * @param [in]  ble_evt         - Pointer to BLE event received from BLE stack.
 * @param [in]  context         - Pointer to custom ECG service structure.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
void BLE_ECGS_onBleEvt(ble_evt_t *ble_evt, void *context) {

    BLE_ECGS_custom_S *customService = (BLE_ECGS_custom_S *) context;

    if((customService != NULL) && (ble_evt != NULL)) {
        switch (ble_evt->header.evt_id) {
            case BLE_GAP_EVT_CONNECTED:
                BLE_ECGS_onConnect(customService, ble_evt);
                break;

            case BLE_GAP_EVT_DISCONNECTED:
                BLE_ECGS_onDisconnect(customService, ble_evt);
                break;

            case BLE_GATTS_EVT_WRITE:
                BLE_ECGS_onWrite(customService, ble_evt);
                break;

            case BLE_EVT_TX_COMPLETE:
                // should set free at least one TX buffer
                muhaBleTxBufferAvailable = true;
                break;

            default:
                break;
        }
    }
}

/***********************************************************************************************//**
 * @brief Function for update custom BLE characteristic, meant for ECG ADC data.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom ECG service structure.
 * @param [in]  ecgData         - Pointer to array of data to be written to characteristic.
 * @return NRF error code.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
uint32_t BLE_ECGS_ecgDataUpdate(BLE_ECGS_custom_S *customService, uint8_t *ecgData) {

    uint32_t err_code = NRF_SUCCESS;
    uint16_t len = NRF51_MUHA_ADS1192_BLE_BYTE_SIZE;
    ble_gatts_hvx_params_t hvx_params;

    // send value if connected and peer enabled notifications for ECG characteristic
    if((customService->conn_handle != BLE_CONN_HANDLE_INVALID) &&
            (muhaEcgNotificationEnabled == true)) {

        hvx_params.handle = customService->custom_value_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0u;
        hvx_params.p_len  = &len;
        hvx_params.p_data = ecgData;

        err_code = sd_ble_gatts_hvx(customService->conn_handle, &hvx_params);
    } else {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}

/***********************************************************************************************//**
 * @brief Function for update custom BLE characteristic, meant for MPU sensor data.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom custom service structure.
 * @param [in]  mpuData         - Pointer to array of data to be written to characteristic.
 * @return NRF error code.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
uint32_t BLE_ECGS_mpuDataUpdate(BLE_ECGS_custom_S *customService, uint8_t *mpuData) {

    ble_gatts_hvx_params_t hvx_params;
    uint32_t err_code = NRF_SUCCESS;
    uint16_t len = NRF51_MUHA_MPU9150_BLE_BYTE_SIZE;

    // send value if connected and peer enabled notifications for MPU characteristic
    if((customService->conn_handle != BLE_CONN_HANDLE_INVALID) &&
            (muhaMpuNotificationEnabled == true)) {

        hvx_params.handle = customService->mpu_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0u;
        hvx_params.p_len  = &len;
        hvx_params.p_data = mpuData;

        err_code = sd_ble_gatts_hvx(customService->conn_handle, &hvx_params);
    } else {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function for adding the ECG data characteristic.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom custom service structure.
 * @param [in]  customInit      - Pointer to initialization custom service structure.
 * @param [out] err             - Pointer to error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_ECGS_ecgDataCharAdd(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        BLE_ECGS_err_E *err) {

    BLE_ECGS_err_E localErr = BLE_ECGS_err_NONE;
    uint32_t err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    // read operation on Cccd should be possible without authentication
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = customInit->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = customInit->custom_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    // variable length
    attr_md.vlen       = 0;

    ble_uuid.type = customService->uuid_type;
    ble_uuid.uuid = ECG_VALUE_CHAR_UUID;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    // sets initial length (in bytes) of characteristic data
    attr_char_value.init_len  = NRF51_MUHA_ADS1192_BLE_BYTE_SIZE;
    attr_char_value.init_offs = 0;
    // sets max length (in bytes) of characteristic data
    attr_char_value.max_len   = NRF51_MUHA_ADS1192_BLE_BYTE_SIZE;

    // add new characteristic to SoftDevice
    err_code = sd_ble_gatts_characteristic_add(customService->service_handle,
            &char_md,
            &attr_char_value,
            &customService->custom_value_handles);

    if (err_code != NRF_SUCCESS) {
       localErr = BLE_ECGS_err_CHARACTERISTIC_INIT_FAIL;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Function for adding the ECG MPU data characteristic.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom custom service structure.
 * @param [in]  customInit      - Pointer to initialization custom service structure.
 * @param [out] err             - Pointer to error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_ECGS_mpuDataCharAdd(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        BLE_ECGS_err_E *err) {

    BLE_ECGS_err_E localErr = BLE_ECGS_err_NONE;
    uint32_t err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    // read operation on Cccd should be possible without authentication
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = customInit->mpu_data_char_attr_md.read_perm;
    attr_md.write_perm = customInit->mpu_data_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    ble_uuid.type = customService->uuid_type;
    ble_uuid.uuid = MPU_VALUE_CHAR_UUID;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = NRF51_MUHA_MPU9150_BLE_BYTE_SIZE;
    attr_char_value.init_offs = 0;
    // sets max length (in bytes) of characteristic data
    attr_char_value.max_len   = NRF51_MUHA_MPU9150_BLE_BYTE_SIZE;

    err_code = sd_ble_gatts_characteristic_add(customService->service_handle,
            &char_md,
            &attr_char_value,
            &customService->mpu_handles);

    if(err_code != NRF_SUCCESS) {
       localErr = BLE_ECGS_err_CHARACTERISTIC_INIT_FAIL;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Function for handling the BLE Connect event.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom service structure.
 * @param [in]  bleEvt          - Pointer to BLE event received from BLE stack.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_ECGS_onConnect(BLE_ECGS_custom_S *customService, const ble_evt_t *ble_evt) {

    BLE_ECGS_evt_S evt;

    customService->conn_handle = ble_evt->evt.gap_evt.conn_handle;
    evt.evt_type = BLE_ECGS_EVT_CONNECTED;

    customService->evt_handler(customService, &evt);
}

/***********************************************************************************************//**
 * @brief Function for handling the BLE Disconnect event.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom service structure.
 * @param [in]  ble_evt         - Pointer to BLE event received from BLE stack.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_ECGS_onDisconnect(BLE_ECGS_custom_S *customService, const ble_evt_t *ble_evt) {

    BLE_ECGS_evt_S evt;

    customService->conn_handle = BLE_CONN_HANDLE_INVALID;
    evt.evt_type = BLE_ECGS_EVT_DISCONNECTED;

    customService->evt_handler(customService, &evt);
}

/***********************************************************************************************//**
 * @brief Function for handling the BLE Write event.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom service structure.
 * @param [in]  ble_evt         - Pointer to BLE event received from BLE stack.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.05.2021.
 **************************************************************************************************/
static void BLE_ECGS_onWrite(BLE_ECGS_custom_S *customService, ble_evt_t *ble_evt) {

    ble_gatts_evt_write_t *p_evt_write = &ble_evt->evt.gatts_evt.params.write;

    // if it has been written to MPU CCCD characteristic handle
    if(p_evt_write->handle == customService->mpu_handles.cccd_handle){
        if(p_evt_write->len == BLE_ECGS_ON_WRITE_NOTIFICATION_BYTE_SIZE) {
            // CCCD written, update notification state
            BLE_ECGS_evt_S evt;
            bool isNotificationEnabled = (p_evt_write->data[0] & BLE_GATT_HVX_NOTIFICATION);

            if(isNotificationEnabled == true) {
                evt.evt_type = BLE_ECGS_EVT_MPU_NOTIFICATION_ENABLED;
            } else {
                evt.evt_type = BLE_ECGS_EVT_MPU_NOTIFICATION_DISABLED;
            }

            customService->evt_handler(customService, &evt);
        }
    }
    // if it has been written to ECG CCCD characteristic handle
    else if(p_evt_write->handle == customService->custom_value_handles.cccd_handle) {
        if(p_evt_write->len == BLE_ECGS_ON_WRITE_NOTIFICATION_BYTE_SIZE) {
            // CCCD written, update notification state
            BLE_ECGS_evt_S evt;
            bool isNotificationEnabled = (p_evt_write->data[0] & BLE_GATT_HVX_NOTIFICATION);

            if(isNotificationEnabled == true) {
                evt.evt_type = BLE_ECGS_EVT_ECG_NOTIFICATION_ENABLED;
            } else {
                evt.evt_type = BLE_ECGS_EVT_ECG_NOTIFICATION_DISABLED;
            }

            customService->evt_handler(customService, &evt);
        }
    } else {
        ;
    }
}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
