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
 * @file    ble_ecgs.h
 * @author  mario.kodba
 * @brief   ADS1192 ECG custom BLE service header file.
 **************************************************************************************************/
#ifndef BLE_ECGS_H__
#define BLE_ECGS_H__

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "nrf51_muha.h"
#include "ble.h"
#include "ble_srv_common.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
//! ECG custom service base UUID - 21acbd5a-9fab-11eb-a8b3-0242ac130003
#define ECG_SERVICE_UUID_BASE           {0x03, 0x00, 0x13, 0xAC, 0x42, 0x02, 0xB3, 0xA8, \
                                            0xEB, 0x11, 0xAB, 0x9F, 0x5A, 0xBD, 0xAC, 0x21}
#define ECG_SERVICE_UUID                (0x1400)    //!< ECG service UUID
#define ECG_VALUE_CHAR_UUID             (0x1401)    //!< ECG value characteristic UUID

#define MPU_VALUE_CHAR_UUID             (0x1402)    //!< MPU9150 value characteristic UUID

/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/
//! Enum for types of events service can generate.
typedef enum BLE_ECGS_evtType_ENUM {
    BLE_ECGS_EVT_DISCONNECTED,          //!< Custom service disconnected event.
    BLE_ECGS_EVT_CONNECTED,             //!< Custom service connected event.
    BLE_CUS_EVT_NOTIFICATION_ENABLED,   //!< Custom value notification enabled event.
    BLE_CUS_EVT_NOTIFICATION_DISABLED,  //!< Custom value notification disabled event.
} BLE_ECGS_evtType_E;

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
typedef struct BLE_ECGS_custom_STRUCT BLE_ECGS_custom_S;

//! Custom service event structure
typedef struct BLE_ECGS_evt_STRUCT {
    BLE_ECGS_evtType_E evt_type;        //!< Type of event
} BLE_ECGS_evt_S;

//! Custom Service event handler type.
typedef void (*BLE_ECGS_evtHandler_T) (BLE_ECGS_custom_S *customService, BLE_ECGS_evt_S *event);

//! Custom Service initialization structure, contains all options and data needed for initialization of the service.
typedef struct BLE_ECGS_customInit_STRUCT {
    BLE_ECGS_evtHandler_T         evt_handler;                  //!< Event handler to be called for handling events in the Custom Service.
    ble_srv_cccd_security_mode_t  custom_value_char_attr_md;    //!< Initial security level for Custom characteristics attribute
} BLE_ECGS_customInit_S;

//! Custom Service structure, contains various status information for the service.
typedef struct BLE_ECGS_custom_STRUCT {
    BLE_ECGS_evtHandler_T         evt_handler;                  //!< Event handler to be called for handling events in the Custom Service.
    uint16_t                      service_handle;               //!< Handle of Custom Service (as provided by the BLE stack).
    ble_gatts_char_handles_t      custom_value_handles;         //!< Handles related to the Custom Value characteristic.
    ble_gatts_char_handles_t      mpu_handles;                  //!< Handles related to the MPU9150 characteristic.
    uint16_t                      conn_handle;                  //!< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).
    uint8_t                       uuid_type;                    //!< Type of UUID
} BLE_ECGS_custom_S;

extern BLE_ECGS_custom_S m_ecgs;

/***************************************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void BLE_ECGS_init(BLE_ECGS_custom_S *customService, const BLE_ECGS_customInit_S *customInit, ERR_E *err);
void BLE_ECGS_onBleEvt(const ble_evt_t *ble_evt, void *context);
void BLE_ECGS_customValueUpdate(BLE_ECGS_custom_S *customService, uint8_t *customValue, ERR_E *err);
void BLE_ECGS_mpuDataUpdate(BLE_ECGS_custom_S *customService, uint8_t *mpuData, ERR_E *err);

#endif // #ifndef BLE_ECGS_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
