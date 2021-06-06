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
 * @file    cfg_ble_muha.c
 * @author  mario.kodba
 * @brief   Configuration for BLE functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stddef.h>

#include "cfg_ble_muha.h"
#include "ble_gap.h"
#include "app_util.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define BLE_MUHA_ADVERTISING_NO_TIMEOUT (0u)                                //!< Timeout in seconds after which advertising will timeout, 0 for no timeout.

#define BLE_MUHA_GAP_MIN_CONN_INTERVAL   MSEC_TO_UNITS(7.5, UNIT_1_25_MS)   //!< Minimum acceptable connection interval (7.5 milliseconds).
#define BLE_MUHA_GAP_MAX_CONN_INTERVAL   MSEC_TO_UNITS(8, UNIT_1_25_MS)     //!< Maximum acceptable connection interval (8 milliseconds).
#define BLE_MUHA_GAP_SLAVE_LATENCY       (0u)                               //!< Slave latency - if set to 0, no connection event will be skipped.
#define BLE_MUHA_CONN_SUP_TIMEOUT        MSEC_TO_UNITS(4000, UNIT_10_MS)    //!< Connection supervisory timeout (4 seconds).

/***************************************************************************************************
 *                              GLOBAL VARIABLES
 **************************************************************************************************/
nrf_clock_lf_cfg_t bleLfClockRC = {
        .source = NRF_CLOCK_LF_SRC_RC,                      // RC clock source
        .rc_ctiv = 1u,                                      // must be 0 if SRC_RC is NOT used as source
        .rc_temp_ctiv = 1u,                                 // must be 0 if SRC_RC is NOT used as source
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM // RC used
};

nrf_clock_lf_cfg_t bleLfClockXTAL = {
        .source = NRF_CLOCK_LF_SRC_XTAL,                    // LF crystal oscillator clock source
        .rc_ctiv = 0u,                                      // must be 0 if SRC_RC is NOT used as source
        .rc_temp_ctiv = 0u,                                 // must be 0 if SRC_RC is NOT used as source
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM  // as defined for crystal oscillator used
};

ble_enable_params_t bleEnableParams = {
        .common_enable_params = {
                .vs_uuid_count = 1u,        // some UUID bases (not sure)
                .p_conn_bw_counts = NULL    // default BW configuration parameters
        },
        .gap_enable_params = {
                .periph_conn_count = 1u,
                .central_conn_count = 0u
        },
        .gatts_enable_params = {
                .attr_tab_size    = 0u,
                .service_changed  = 0u
        }
};

ble_gap_adv_params_t bleAdvertisingParams = {
        .type = BLE_GAP_ADV_TYPE_ADV_IND,   // type of advertising type used
        .p_peer_addr = NULL,                // not used on BLE_GAP_ADV_TYPE_ADV_IND
        .fp = BLE_GAP_ADV_FP_ANY,           // allows scan requests and connect requests from any device.
        .p_whitelist = NULL,
        .interval = 160u,                   /*< Advertising interval between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s), see @ref BLE_GAP_ADV_INTERVALS.
                                                - If type equals @ref BLE_GAP_ADV_TYPE_ADV_DIRECT_IND, this parameter must be set to 0 for high duty cycle directed advertising.
                                                - If type equals @ref BLE_GAP_ADV_TYPE_ADV_DIRECT_IND, set @ref BLE_GAP_ADV_INTERVAL_MIN <= interval <= @ref BLE_GAP_ADV_INTERVAL_MAX for low duty cycle advertising.*/
        .timeout = BLE_MUHA_ADVERTISING_NO_TIMEOUT, // advertising timeout in seconds (if set to 0, no timeout will occur)
        .channel_mask = {
                .ch_37_off = 0u,            // channel 37 - advertising ON
                .ch_38_off = 0u,            // channel 38 - advertising ON
                .ch_39_off = 0u             // channel 39 - advertising ON
        }
};

ble_gap_conn_params_t gapConnectionParams = {
        .min_conn_interval = BLE_MUHA_GAP_MIN_CONN_INTERVAL,
        .max_conn_interval = BLE_MUHA_GAP_MAX_CONN_INTERVAL,
        .slave_latency     = BLE_MUHA_GAP_SLAVE_LATENCY,
        .conn_sup_timeout  = BLE_MUHA_CONN_SUP_TIMEOUT
};

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
