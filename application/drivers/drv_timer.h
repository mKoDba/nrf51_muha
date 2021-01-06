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
 * @file    drv_timer.h
 * @author  mario.kodba
 * @brief   NRF51 TIMER module functionality header file.
 **************************************************************************************************/

#ifndef DRV_TIMER_H_
#define DRV_TIMER_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "sdk_common.h"
#include "sdk_config.h"
#include "nrf51.h"
#include "drv_common.h"

#include <stdint.h>
/***************************************************************************************************
 *                              ENUMERATIONS
 **************************************************************************************************/
//! TIMER driver errors enumeration
typedef enum DRV_TIMER_err_ENUM {
    DRV_TIMER_err_NONE      = 0u,           //!< No error.
    DRV_TIMER_err_NULL_PARAM,               //!< NULL parameter error.
    DRV_TIMER_err_ALREADY_RUNNING,          //!< TIMER already running error.
    DRV_TIMER_err_NO_TIMER_INSTANCE         //!< Invalid TIMER instance error.
} DRV_TIMER_err_E;

//! TIMER driver frequency enumeration
typedef enum DRV_TIMER_freq_ENUM {
    DRV_TIMER_freq_16MHz = 0,               //!< Timer frequency 16MHz.
    DRV_TIMER_freq_8MHz,                    //!< Timer frequency 8MHz.
    DRV_TIMER_freq_4MHz,                    //!< Timer frequency 4MHz.
    DRV_TIMER_freq_2MHz,                    //!< Timer frequency 2MHz.
    DRV_TIMER_freq_1MHz,                    //!< Timer frequency 1MHz.
    DRV_TIMER_freq_500KHz,                  //!< Timer frequency 500kHz.
    DRV_TIMER_freq_250KHz,                  //!< Timer frequency 250kHz.
    DRV_TIMER_freq_125KHz,                  //!< Timer frequency 125kHz.
    DRV_TIMER_freq_62500Hz,                 //!< Timer frequency 62500Hz.
    DRV_TIMER_freq_31250Hz                  //!< Timer frequency 31250Hz.
} DRV_TIMER_freq_E;

//! TIMER driver mode enumeration
typedef enum DRV_TIMER_mode_ENUM {
    DRV_TIMER_mode_NORMAL = 0,              //!< Timer normal (timer) mode.
    DRV_TIMER_mode_COUNTER                  //!< Timer counter mode.
} DRV_TIMER_mode_E;

//! TIMER driver bit width enumeration
typedef enum DRV_TIMER_bitWidth_ENUM {
    DRV_TIMER_bitWidth_16 = 0,              //!< 16-bit timer.
    DRV_TIMER_bitWidth_8  = 1,              //!< 8-bit timer.
    DRV_TIMER_bitWidth_24 = 2,              //!< 24-bit timer.
    DRV_TIMER_bitWidth_32 = 3               //!< 32-bit timer.
} DRV_TIMER_bitWidth_E;

//! TIMER driver state enumeration
typedef enum DRV_TIMER_state_ENUM {
    DRV_TIMER_state_RUNNING = 0,            //!< Timer is in running state.
    DRV_TIMER_state_PAUSED,                 //!< Timer is paused.
    DRV_TIMER_state_STOPPED                 //!< Timer is stopped.
} DRV_TIMER_state_E;

//! TIMER driver task enumeration
typedef enum DRV_TIMER_task_ENUM {
    DRV_TIMER_task_START    = offsetof(NRF_TIMER_Type, TASKS_START),      //!< Timer start task.
    DRV_TIMER_task_STOP     = offsetof(NRF_TIMER_Type, TASKS_STOP),       //!< Timer stop task.
    DRV_TIMER_task_CLEAR    = offsetof(NRF_TIMER_Type, TASKS_CLEAR),      //!< Timer clear task.
    DRV_TIMER_task_COUNT    = offsetof(NRF_TIMER_Type, TASKS_COUNT),      //!< Timer count task.
    DRV_TIMER_task_SHUTDOWN = offsetof(NRF_TIMER_Type, TASKS_SHUTDOWN),   //!< Timer shutdown task.
    DRV_TIMER_task_CAPTURE0 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[0]), //!< Timer capture[0] task.
    DRV_TIMER_task_CAPTURE1 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[1]), //!< Timer capture[1] task.
    DRV_TIMER_task_CAPTURE2 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[2]), //!< Timer capture[2] task.
    DRV_TIMER_task_CAPTURE3 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[3]), //!< Timer capture[3] task.
} DRV_TIMER_task_E;

//! TIMER driver compare event enumeration
typedef enum DRV_TIMER_event_ENUM {
    DRV_TIMER_task_COMPARE0 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[0]),//!< Timer compare[0] event.
    DRV_TIMER_task_COMPARE1 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[1]),//!< Timer compare[1] event.
    DRV_TIMER_task_COMPARE2 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[2]),//!< Timer compare[2] event.
    DRV_TIMER_task_COMPARE3 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[3]) //!< Timer compare[3] event.
} DRV_TIMER_event_E;

//! TIMER driver compare/capture channel enumeration
typedef enum DRV_TIMER_cc_ENUM {
    DRV_TIMER_cc_CHANNEL0 = 0,              //!< Timer capture/compare channel 0.
    DRV_TIMER_cc_CHANNEL1,                  //!< Timer capture/compare channel 1.
    DRV_TIMER_cc_CHANNEL2,                  //!< Timer capture/compare channel 2.
    DRV_TIMER_cc_CHANNEL3,                  //!< Timer capture/compare channel 3.

    DRV_TIMER_cc_CHANNEL_COUNT              //!< Timer capture/compare channels count.
} DRV_TIMER_cc_E;

//! TIMER driver ID enumeration
typedef enum DRV_TIMER_id_ENUM {
    DRV_TIMER_id_0 = 0,                     //!< Timer ID 0.
    DRV_TIMER_id_1,                         //!< Timer ID 1.
    DRV_TIMER_id_2,                         //!< Timer ID 2.

    DRV_TIMER_id_COUNT                      //!< Timer ID count.
} DRV_TIMER_id_E;

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/
//! TIMER callback function pointer
typedef void (*DRV_TIMER_IRQHandler)(DRV_TIMER_cc_E timerEvent, void *context);

//! TIMER control block structure
typedef struct DRV_TIMER_control_block_STRUCT {
    DRV_TIMER_IRQHandler callbackFunction;  //!< Callback function for TIMERx interrupt.
    void *context;                          //!< Context passed to TIMER callback function.

} DRV_TIMER_control_block_S;

//! TIMER configuration structure
typedef struct DRV_TIMER_config_STRUCT {
    NRF_TIMER_Type *timerReg;               //!< Timer registers address base
    DRV_TIMER_freq_E frequency;             //!< Timer frequency
    DRV_TIMER_mode_E mode;                  //!< Timer mode of operation.
    DRV_TIMER_bitWidth_E bitWidth;          //!< Timer bit width.
    DRV_TIMER_id_E id;                      //!< Timer instance id.
    uint8_t irqPriority;                    //!< Interrupt priority.
    void *context;                          //!< Context passed to interrupt handler (callback function).
} DRV_TIMER_config_S;

//! TIMER instance structure
typedef struct DRV_TIMER_instance_STRUCT {
    DRV_TIMER_config_S *config;             //!< Timer configuration

    DRV_TIMER_state_E state;                //!< Current timer state.
    bool isInitialized;                     //!< Is time instance initialized.
} DRV_TIMER_instance_S;

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void DRV_TIMER_init(DRV_TIMER_instance_S *timerInstance,
        DRV_TIMER_config_S *timerConf,
        DRV_TIMER_IRQHandler irqHandler,
        DRV_TIMER_err_E *outErr);
void DRV_TIMER_enableTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr);
void DRV_TIMER_disableTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr);
void DRV_TIMER_pauseTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr);
void DRV_TIMER_clearTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr);
void DRV_TIMER_incrementTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr);
uint32_t DRV_TIMER_captureTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        DRV_TIMER_err_E *outErr);
uint32_t DRV_TIMER_getTimeDiff(const DRV_TIMER_instance_S *tInstance,
        uint32_t *start,
        uint32_t *stop);
void DRV_TIMER_compareEnableTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        uint32_t compareValue,
        DRV_TIMER_err_E *outErr);
void DRV_TIMER_compareDisableTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        DRV_TIMER_err_E *outErr);

#endif // #ifndef DRV_TIMER_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
