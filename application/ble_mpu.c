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
#include "ble_mpu.h"
#include "bsp_ecg_ADS1192.h"
#include "nrf51_muha.h"
#include "SEGGER_RTT.h"
#include <string.h>

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                          GLOBAL VARIABLES
 **************************************************************************************************/


/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void BLE_MPU_mpuDataCharAdd(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        ERR_E *err);
static void BLE_MPU_onConnect(BLE_ECGS_custom_S *customService, ble_evt_t const *p_ble_evt);
static void BLE_MPU_onWrite(BLE_ECGS_custom_S *customService, ble_evt_t const *p_ble_evt);
static void BLE_MPU_onDisconnect(BLE_ECGS_custom_S *customService, ble_evt_t const *p_ble_evt);

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function initializes custom BLE service, meant for ECG ADC data.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom ECG service structure.
 * @param [in]  customInit      - Pointer to custom service initialization structure.
 * @param [out] err             - Pointer to error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
void BLE_MPU_init(BLE_ECGS_custom_S *customService, const BLE_ECGS_customInit_S *customInit, ERR_E *err) {

    ERR_E localErr = ERR_NONE;
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    if((customService != NULL) && (customInit != NULL)) {
        // initialize service structure
        customService->conn_handle = BLE_CONN_HANDLE_INVALID;
        customService->evt_handler = customInit->evt_handler;

        if(localErr == ERR_NONE) {
            ble_uuid.type = customService->uuid_type;
            // in order to work with another custom service
            ble_uuid.type = 2u;
            ble_uuid.uuid = MPU_SERVICE_UUID;
            // add the custom service
            err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &customService->service_handle);
        }

        if (err_code != NRF_SUCCESS) {
            localErr = ERR_BLE_CUSTOM_SERVICE_INIT_FAIL;
        }

        if(localErr == ERR_NONE) {
            // add MPU data characteristics
            BLE_MPU_mpuDataCharAdd(customService, customInit, &localErr);
        }
    } else {
        localErr = ERR_NULL_PARAMETER;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Function initializes custom BLE service, meant for ECG ADC data.
 ***************************************************************************************************
 * @param [in]  ble_evt         - Pointer to BLE event received from BLE stack. (Nordic)
 * @param [in]  context         - Pointer to custom ECG service structure.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
void BLE_MPU_onBleEvt(const ble_evt_t *ble_evt, void *context) {

    BLE_ECGS_custom_S *customService = (BLE_ECGS_custom_S *) context;

    if((customService != NULL) && (ble_evt != NULL)) {
        switch (ble_evt->header.evt_id) {
            case BLE_GAP_EVT_CONNECTED:
                BLE_MPU_onConnect(customService, ble_evt);
                break;

            case BLE_GAP_EVT_DISCONNECTED:
                BLE_MPU_onDisconnect(customService, ble_evt);
                break;

            case BLE_GATTS_EVT_WRITE:
                BLE_MPU_onWrite(customService, ble_evt);
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
 * @param [in]  mpuData         - Pointer to array of data to be written to characteristic.
 * @param [out] err             - Pointer to error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
void BLE_MPU_mpuDataUpdate(BLE_ECGS_custom_S *customService, uint8_t *mpuData, ERR_E *err) {

    ERR_E localErr;
    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    gatts_value.len     = NRF51_MUHA_MPU9150_BLE_BYTE_SIZE;
    gatts_value.offset  = 0u;
    gatts_value.p_value = mpuData;

//    // update database
//    err_code = sd_ble_gatts_value_set(customService->conn_handle,
//            customService->mpu_handles.value_handle,
//            &gatts_value);

    // send value if connected and notifying.
    if((customService->conn_handle != BLE_CONN_HANDLE_INVALID)) {
        ble_gatts_hvx_params_t hvx_params;

        hvx_params.handle = customService->mpu_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len  = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(customService->conn_handle, &hvx_params);
    } else {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    if(err_code != NRF_SUCCESS) {
        localErr = ERR_BLE_CUSTOM_SERVICE_RUNTIME_FAIL;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Function for adding the MPU9150 characteristic.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom ECG service structure.
 * @param [in]  customInit      - Pointer to initialization ECG service structure.
 * @param [out] err             - Pointer to error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    29.05.2021.
 **************************************************************************************************/
static void BLE_MPU_mpuDataCharAdd(BLE_ECGS_custom_S *customService,
        const BLE_ECGS_customInit_S *customInit,
        ERR_E *err) {

    ERR_E localErr = ERR_NONE;
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
    char_md.char_props.write  = 1;
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
    attr_md.vlen       = 0;

    // in order to work with another custom service
    ble_uuid.type = 2u;
    ble_uuid.uuid = MPU_VALUE_CHAR_UUID;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = NRF51_MUHA_MPU9150_BLE_BYTE_SIZE;
    attr_char_value.init_offs = 0;
    // sets length (in bytes) of characteristic data
    attr_char_value.max_len   = NRF51_MUHA_MPU9150_BLE_BYTE_SIZE;

    err_code = sd_ble_gatts_characteristic_add(customService->service_handle,
            &char_md,
            &attr_char_value,
            &customService->mpu_handles);

    if (err_code != NRF_SUCCESS) {
       localErr = ERR_BLE_CUSTOM_SERVICE_INIT_FAIL;
    }

    if(err != NULL) {
        *err = localErr;
    }
}

/***********************************************************************************************//**
 * @brief Function for handling the Connect event.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom ECG service structure.
 * @param [in]  bleEvt          - Pointer to BLE event received from BLE stack. (Nordic)
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_MPU_onConnect(BLE_ECGS_custom_S *customService, const ble_evt_t *ble_evt) {

    customService->conn_handle = ble_evt->evt.gap_evt.conn_handle;

    BLE_ECGS_evt_S evt;

    evt.evt_type = BLE_ECGS_EVT_CONNECTED;

    customService->evt_handler(customService, &evt);
}

/***********************************************************************************************//**
 * @brief Function for handling the Disconnect event.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom ECG service structure.
 * @param [in]  ble_evt         - Pointer to BLE event received from BLE stack. (Nordic)
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    16.04.2021.
 **************************************************************************************************/
static void BLE_MPU_onDisconnect(BLE_ECGS_custom_S *customService, const ble_evt_t *ble_evt) {

    (void) ble_evt;
    customService->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/***********************************************************************************************//**
 * @brief Function for handling the Write event.
 ***************************************************************************************************
 * @param [in]  customService   - Pointer to custom ECG service structure.
 * @param [in]  ble_evt         - Pointer to BLE event received from BLE stack. (Nordic)
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    13.05.2021.
 **************************************************************************************************/
static void BLE_MPU_onWrite(BLE_ECGS_custom_S *customService, const ble_evt_t *ble_evt) {


}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
