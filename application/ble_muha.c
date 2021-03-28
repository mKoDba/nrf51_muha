//
//
//#include <stdint.h>
//#include <string.h>
//#include "nordic_common.h"
//#include "nrf.h"
//#include "ble_hci.h"
//#include "ble_advdata.h"
//#include "ble_advertising.h"
//#include "ble_conn_params.h"
//#include "softdevice_handler.h"
//#include "app_timer.h"
//#include "ble_nus.h"
//#include "app_uart.h"
//#include "app_timer.h"
//#include "app_util_platform.h"
//#include "nrf_delay.h"
//#include "ble_types.h"
//#include "ble_dis.h"
//#include "ble_srv_common.h"
//#include "nrf_gpio.h"
//
//#include "cfg_nrf51_muha_pinout.h"
//#include "cfg_ble_muha.h"
//#include "nrf51_muha.h"
//
//#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
//#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
//
//#define DEVICE_NAME                     "MUHA"                                      /**< Name of device. Will be included in the advertising data. */
//#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */
//#define MODEL_NAME                      "nRF51"                                     /**< Model name. */
//#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */
//
//#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
//#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */
//#define ADV_INTERVAL_IN_MS              1200
//
//#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
//#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */
//
//#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
//#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
//#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */
//
//#define ADV_INTERVAL                    MSEC_TO_UNITS(ADV_INTERVAL_IN_MS, UNIT_0_625_MS) /**< The advertising interval (in units of 0.625 ms. */
//#define ADV_TIMEOUT_IN_SECONDS          0                                           /**< The advertising timeout (in units of seconds). */
//#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
//#define APP_TIMER_MAX_TIMERS            1                                           /**< Maximum number of simultaneously created timers. */
//#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */
//#define ADVDATA_UPDATE_INTERVAL         APP_TIMER_TICKS(ADV_INTERVAL_IN_MS, APP_TIMER_PRESCALER)
//
//#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
//
//
///**@brief Function for initializing the Advertising functionality.
// *
// * @details Encodes the required advertising data and passes it to the stack.
// *          Also builds a structure to be passed to the stack when starting advertising.
// */
//static void advdata_update(void)
//{
////    uint32_t      err_code;
////    ble_advdata_t advdata;
////    uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
////
////    ble_advdata_service_data_t service_data[2];
////
////    uint8_t battery_data = 100u;
////    uint32_t temperature_data = 35u;
////
////    service_data[0].service_uuid = BLE_UUID_BATTERY_SERVICE;
////    service_data[0].data.size    = sizeof(battery_data);
////    service_data[0].data.p_data  = &battery_data;
////
////    service_data[1].service_uuid = BLE_UUID_HEALTH_THERMOMETER_SERVICE;
////    service_data[1].data.size    = sizeof(temperature_data);
////    service_data[1].data.p_data  = (uint8_t *) &temperature_data;
////
////    // Build and set advertising data
////    memset(&advdata, 0, sizeof(advdata));
////
////    advdata.name_type            = BLE_ADVDATA_FULL_NAME;
////    advdata.include_appearance   = false;
////    advdata.flags                = flags;
////    advdata.service_data_count   = 2;
////    advdata.p_service_data_array = service_data;
////
////    err_code = ble_advdata_set(&advdata, NULL);
////    APP_ERROR_CHECK(err_code);
//}
//
//APP_TIMER_DEF(m_advdata_update_timer);
//
///**@brief Function for starting timers.
//*/
//static void timers_start(void)
//{
//    uint32_t err_code = app_timer_start(m_advdata_update_timer, ADVDATA_UPDATE_INTERVAL, NULL);
//    APP_ERROR_CHECK(err_code);
//}
//
//void advdata_update_timer_timeout_handler(void * p_context)
//{
//    advdata_update();
//}
//
///**@brief Function for the Timer initialization.
// *
// * @details Initializes the timer module.
// */
//static void timers_init(void)
//{
//    // Initialize timer module, making it use the scheduler
//    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
//
////    uint32_t err_code = app_timer_create(m_advdata_update_timer, APP_TIMER_MODE_REPEATED, advdata_update_timer_timeout_handler);
////    APP_ERROR_CHECK(err_code);
//}
//
//
///**@brief Function for the application's SoftDevice event handler.
// *
// * @param[in] p_ble_evt SoftDevice event.
// */
//static void on_ble_evt(ble_evt_t * p_ble_evt)
//{
//    uint32_t err_code = NRF_SUCCESS;
//
//    switch (p_ble_evt->header.evt_id)
//    {
//        default:
//            break;
//    }
//
//}
//
///**@brief Function for handling advertising events.
// *
// * @details This function will be called for advertising events which are passed to the application.
// *
// * @param[in] ble_adv_evt  Advertising event.
// */
//static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
//{
//    uint32_t err_code;
//    static uint64_t i = 0u;
//
//    switch (ble_adv_evt)
//    {
//        case BLE_ADV_EVT_FAST:
//            if(i > 1000u) {
//                nrf_gpio_pin_clear(LD1);
//                i = 0u;
//            } else {
//                nrf_gpio_pin_set(LD1);
//                i++;
//            }
//            break;
//        default:
//            break;
//    }
//}
//
///**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
// *
// * @details This function is called from the scheduler in the main loop after a BLE stack
// *          event has been received.
// *
// * @param[in]   p_ble_evt   Bluetooth stack event.
// */
//static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
//{
//    on_ble_evt(p_ble_evt);
//}
//
//
//
//
///**@brief Function for the SoftDevice initialization.
// *
// * @details This function initializes the SoftDevice and the BLE event interrupt.
// */
//static void BLE_MUHA_bleStackInit(ERR_E *error) {
//
//    uint32_t err_code;
//    ble_enable_params_t ble_enable_params;
//
//    // initialize SoftDevice.
//    SOFTDEVICE_HANDLER_INIT(&bleLfClock, NULL);
//
//    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
//                                                    PERIPHERAL_LINK_COUNT,
//                                                    &ble_enable_params);
//    APP_ERROR_CHECK(err_code);
//
//    // Check the ram settings against the used number of links
//    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
//
//    // Enable BLE stack.
//    err_code = softdevice_enable(&ble_enable_params);
//    APP_ERROR_CHECK(err_code);
//
//    // Subscribe for BLE events.
//    err_code = softdevice_ble_evt_handler_set(on_adv_evt);
//    APP_ERROR_CHECK(err_code);
//}
//
///**@brief Function for the GAP initialization.
// *
// * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
// *          the device. It also sets the permissions and appearance.
// */
//static void BLE_MUHA_gapParamsInit(ERR_E *error) {
//
//    uint32_t err;
//    ble_gap_conn_sec_mode_t sec_mode;
//
//    // no security needed for now
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
//
//    // setting device name
//    err = sd_ble_gap_device_name_set(&sec_mode,
//            (const uint8_t *) DEVICE_NAME,
//            (uint16_t) strlen(DEVICE_NAME));
//
//    // setting appearance
//    if(err == NRF_SUCCESS) {
//        err = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_TAG);
//    }
//
//    // setting PPCP characteristic
//    if(err == NRF_SUCCESS) {
//        err = sd_ble_gap_ppcp_set(&gapConnectionParams);
//    }
//
//    enum {
//      FIRST_UPDATE_DELAY = APP_TIMER_TICKS(5000, 0),
//      NEXT_UPDATE_DELAY  = APP_TIMER_TICKS(30000, 0),
//      MAX_UPDATE_COUNT   = 3
//    };
//
//    ble_conn_params_init_t cp_init = {
//        .p_conn_params                  = NULL,     // connection parameters fetched from the host
//        .first_conn_params_update_delay = FIRST_UPDATE_DELAY,
//        .next_conn_params_update_delay  = NEXT_UPDATE_DELAY,
//        .max_conn_params_update_count   = MAX_UPDATE_COUNT,
//        .start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID,
//        .disconnect_on_fail             = true,
//        .evt_handler                    = NULL,
//        .error_handler                  = NULL
//    };
//
//    if(err == NRF_SUCCESS) {
//        err = ble_conn_params_init(&cp_init);
//    }
//
//    if(error != NULL) {
//        if(err != NRF_SUCCESS) {
//            *error = ERR_GAP_PARAMETERS_INIT_FAIL;
//        }
//    }
//}
//
//static void BLE_MUHA_servicesInit(ERR_E *error) {
//
//    uint32_t err;
//    ble_dis_init_t devInfoService;
//
//    memset(&devInfoService, 0, sizeof(devInfoService));
//
//    // Device information service
//    ble_srv_ascii_to_utf8(&devInfoService.manufact_name_str, MANUFACTURER_NAME);
//    ble_srv_ascii_to_utf8(&devInfoService.model_num_str, MODEL_NAME);
//
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&devInfoService.dis_attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&devInfoService.dis_attr_md.write_perm);
//
//    err = ble_dis_init(&devInfoService);
//
//    if(error != NULL) {
//        if(err != NRF_SUCCESS) {
//            *error = ERR_ADVERTISING_INIT_FAIL;
//        }
//    }
//}
//
//static void BLE_MUHA_advertisingInit(ERR_E *error) {
//
//    uint32_t err;
//    ble_advdata_t advData;
//    ble_adv_modes_config_t advOptions;
//
//    // bvild advertising data struct to pass into @ref ble_advertising_init.
//    memset(&advData, 0, sizeof(advData));
//    advData.name_type          = BLE_ADVDATA_FULL_NAME;
//    advData.include_appearance = true;
//    advData.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
//
//    memset(&advOptions, 0, sizeof(advOptions));
//    advOptions.ble_adv_fast_enabled  = true;
//    advOptions.ble_adv_fast_interval = APP_ADV_INTERVAL;
//    advOptions.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;
//
//    err = ble_advertising_init(&advData, NULL, &advOptions, on_adv_evt, NULL);
//
//    if(error != NULL) {
//        if(err != NRF_SUCCESS) {
//            *error = ERR_ADVERTISING_INIT_FAIL;
//        }
//    }
//}
//
//static void BLE_MUHA_advertisingStart(ERR_E *error) {
//
//    uint32_t err;
//
//    err = ble_advertising_start(BLE_ADV_MODE_FAST);
//
//    if(error != NULL) {
//        if(err != NRF_SUCCESS) {
//            *error = ERR_ADVERTISING_INIT_FAIL;
//        }
//    }
//}
//
//void BLE_MUHA_init(ERR_E *error) {
//
//    ERR_E localErr = ERR_NONE;
//
//    timers_init();
//
//    // STEP 1: BLE stack initialization
//    if(localErr == ERR_NONE) {
//        BLE_MUHA_bleStackInit(&localErr);
//    }
//
//    // STEP 2: GAP parameters initialization
//    if(localErr == ERR_NONE) {
//        BLE_MUHA_gapParamsInit(&localErr);
//    }
//
//    // STEP 3: Services initialization
//    if(localErr == ERR_NONE) {
//        BLE_MUHA_servicesInit(&localErr);
//    }
//
//    // STEP 4: Advertising initialization
//    if(localErr == ERR_NONE) {
//        BLE_MUHA_advertisingInit(&localErr);
//    }
//
//    // Peer Manager not needed I guess
//
////    timers_start();
//
//    nrf_gpio_pin_set(LD1);
//
//    // STEP 5: Start advertising
//    if(localErr == ERR_NONE) {
//        BLE_MUHA_advertisingStart(&localErr);
//    }
//
//    // Enter main loop.
//    for (;;)
//    {
//
//    }
//
//    if(error != NULL) {
//        *error = localErr;
//    }
//}


/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_beacon_main main.c
 * @{
 * @ingroup ble_sdk_app_beacon
 * @brief Beacon Transmitter Sample Application main file.
 *
 * This file contains the source code for an Beacon transmitter sample application.
 */

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
//#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "cfg_ble_muha.h"
#include "nrf51_muha.h"

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define APP_BEACON_INFO_LENGTH          0x17                              /**< Total length of information advertised by the Beacon. */
#define APP_ADV_DATA_LENGTH             0x15                              /**< Length of manufacturer specific data in the advertisement. */
#define APP_DEVICE_TYPE                 0x02                              /**< 0x02 refers to Beacon. */
#define APP_MEASURED_RSSI               0xC3                              /**< The Beacon's measured RSSI at 1 meter distance in dBm. */
#define APP_COMPANY_IDENTIFIER          0x0059                            /**< Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
#define APP_MAJOR_VALUE                 0x01, 0x02                        /**< Major value used to identify Beacons. */
#define APP_MINOR_VALUE                 0x03, 0x04                        /**< Minor value used to identify Beacons. */
#define APP_BEACON_UUID                 0x01, 0x12, 0x23, 0x34, \
                                        0x45, 0x56, 0x67, 0x78, \
                                        0x89, 0x9a, 0xab, 0xbc, \
                                        0xcd, 0xde, 0xef, 0xf0            /**< Proprietary UUID for Beacon. */

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

#if defined(USE_UICR_FOR_MAJ_MIN_VALUES)
#define MAJ_VAL_OFFSET_IN_BEACON_INFO   18                                /**< Position of the MSB of the Major Value in m_beacon_info array. */
#define UICR_ADDRESS                    0x10001080                        /**< Address of the UICR register used by this example. The major and minor versions to be encoded into the advertising data will be picked up from this location. */
#endif

static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */
static uint8_t m_beacon_info[APP_BEACON_INFO_LENGTH] =                    /**< Information advertised by the Beacon. */
{
    APP_DEVICE_TYPE,     // Manufacturer specific information. Specifies the device type in this
                         // implementation.
    APP_ADV_DATA_LENGTH, // Manufacturer specific information. Specifies the length of the
                         // manufacturer specific data in this implementation.
    APP_BEACON_UUID,     // 128 bit UUID value.
    APP_MAJOR_VALUE,     // Major arbitrary value that can be used to distinguish between Beacons.
    APP_MINOR_VALUE,     // Minor arbitrary value that can be used to distinguish between Beacons.
    APP_MEASURED_RSSI    // Manufacturer specific information. The Beacon's measured TX power in
                         // this implementation.
};

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    ble_advdata_manuf_data_t manuf_specific_data;

    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;

#if defined(USE_UICR_FOR_MAJ_MIN_VALUES)
    // If USE_UICR_FOR_MAJ_MIN_VALUES is defined, the major and minor values will be read from the
    // UICR instead of using the default values. The major and minor values obtained from the UICR
    // are encoded into advertising data in big endian order (MSB First).
    // To set the UICR used by this example to a desired value, write to the address 0x10001080
    // using the nrfjprog tool. The command to be used is as follows.
    // nrfjprog --snr <Segger-chip-Serial-Number> --memwr 0x10001080 --val <your major/minor value>
    // For example, for a major value and minor value of 0xabcd and 0x0102 respectively, the
    // the following command should be used.
    // nrfjprog --snr <Segger-chip-Serial-Number> --memwr 0x10001080 --val 0xabcd0102
    uint16_t major_value = ((*(uint32_t *)UICR_ADDRESS) & 0xFFFF0000) >> 16;
    uint16_t minor_value = ((*(uint32_t *)UICR_ADDRESS) & 0x0000FFFF);

    uint8_t index = MAJ_VAL_OFFSET_IN_BEACON_INFO;

    m_beacon_info[index++] = MSB_16(major_value);
    m_beacon_info[index++] = LSB_16(major_value);

    m_beacon_info[index++] = MSB_16(minor_value);
    m_beacon_info[index++] = LSB_16(minor_value);
#endif

    manuf_specific_data.data.p_data = (uint8_t *) m_beacon_info;
    manuf_specific_data.data.size   = APP_BEACON_INFO_LENGTH;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type             = BLE_ADVDATA_NO_NAME;
    advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

//    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
//    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

//    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LF_SRC_RC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&bleLfClock, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for application main entry.
 */
void BLE_MUHA_init(ERR_E *error) {

    uint32_t err_code;

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
//    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
//    APP_ERROR_CHECK(err_code);
    ble_stack_init();
    advertising_init();

    advertising_start();

    // Enter main loop.
    for (;; )
    {
        power_manage();
    }
}


/**
 * @}
 */




