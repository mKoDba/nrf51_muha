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
 * @file    drv_timer.c
 * @author  mario.kodba
 * @brief   NRF51 TIMER module functionality source file.
 **************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "drv_timer.h"
#include "hal_timer.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/
#define DRV_TIMER_MAX_VALUE_16_BIT      (65535u)

//!< Timer time resolution settings
#define DRV_TIMER_RESOLUTION_16MHZ      (0.0625f)       //!< TIMER resolution with 16MHz
#define DRV_TIMER_RESOLUTION_8MHZ       (0.125f)        //!< TIMER resolution with 8MHz
#define DRV_TIMER_RESOLUTION_4MHZ       (0.25f)         //!< TIMER resolution with 4MHz
#define DRV_TIMER_RESOLUTION_2MHZ       (0.5f)          //!< TIMER resolution with 2MHz
#define DRV_TIMER_RESOLUTION_1MHZ       (1.0f)          //!< TIMER resolution with 1MHz
#define DRV_TIMER_RESOLUTION_500KHZ     (2.0f)          //!< TIMER resolution with 500kHz
#define DRV_TIMER_RESOLUTION_250KHZ     (4.0f)          //!< TIMER resolution with 250KHz
#define DRV_TIMER_RESOLUTION_125KHZ     (8.0f)          //!< TIMER resolution with 125KHz
#define DRV_TIMER_RESOLUTION_62500HZ    (16.0f)         //!< TIMER resolution with 62500Hz
#define DRV_TIMER_RESOLUTION_31250HZ    (32.0f)         //!< TIMER resolution with 31250Hz

/***************************************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 **************************************************************************************************/
static void DRV_TIMER_irqHandler(DRV_TIMER_id_E timerInstanceId);
static float DRV_TIMER_getTimerResolution(const DRV_TIMER_instance_S *tInstance);

/***************************************************************************************************
 *                           GLOBAL VARIABLES
 **************************************************************************************************/
static NRF_TIMER_Type *timerInstances[DRV_TIMER_id_COUNT] = {
        NRF_TIMER0, NRF_TIMER1, NRF_TIMER2
};

static bool DRV_TIMER_isInit[DRV_TIMER_id_COUNT] = {
        false, false, false
};

static DRV_TIMER_control_block_S DRV_TIMER_controlBlock[DRV_TIMER_id_COUNT];

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief TIMER0 IRQ Handler.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
void TIMER0_IRQHandler(void) {
    DRV_TIMER_irqHandler(DRV_TIMER_id_0);
}

/***********************************************************************************************//**
 * @brief TIMER1 IRQ Handler.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
void TIMER1_IRQHandler(void) {
    DRV_TIMER_irqHandler(DRV_TIMER_id_1);
}

/***********************************************************************************************//**
 * @brief TIMER2 IRQ Handler.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
void TIMER2_IRQHandler(void) {
    DRV_TIMER_irqHandler(DRV_TIMER_id_2);
}

/***********************************************************************************************//**
 * @brief Initializes timer instance.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [in]   *timerConf     - pointer to timer configuration structure.
 * @param [in]   irqHandler     - function pointer to user defined IRQ handler.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void DRV_TIMER_init(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_config_S *timerConf,
        DRV_TIMER_IRQHandler irqHandler,
        DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL && timerConf != NULL) {
        // set timer configuration
        tInstance->config = timerConf;
        tInstance->state = DRV_TIMER_state_STOPPED;
        // set callback function and context, if given
        DRV_TIMER_control_block_S *block = &DRV_TIMER_controlBlock[tInstance->config->id];
        block->callbackFunction = irqHandler;
        block->context = tInstance->config->context;
        // clear events for given timer
        HAL_TIMER_clearEvents(tInstance->config->timerReg);
        // enable IRQ
        DRV_COMMON_enableIRQPriority(tInstance->config->timerReg, tInstance->config->irqPriority);
        // set timer frequency
        HAL_TIMER_setFrequency(tInstance->config->timerReg, tInstance->config->frequency);
        // set timer mode (normal or counter)
        HAL_TIMER_setMode(tInstance->config->timerReg, tInstance->config->mode);
        // set timer bit width
        HAL_TIMER_setBitWidth(tInstance->config->timerReg, tInstance->config->bitWidth);

        DRV_TIMER_isInit[tInstance->config->id] = true;
        tInstance->isInitialized = true;
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Triggers timer START task, enabling timer instance.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void DRV_TIMER_enableTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            if(tInstance->state != DRV_TIMER_state_RUNNING) {
                HAL_TIMER_runTask(tInstance->config->timerReg, DRV_TIMER_task_START);
                tInstance->state = DRV_TIMER_state_RUNNING;
            } else {
                err = DRV_TIMER_err_ALREADY_RUNNING;
            }
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Triggers timer SHUTDOWN task, disabling timer instance.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void DRV_TIMER_disableTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            HAL_TIMER_runTask(tInstance->config->timerReg, DRV_TIMER_task_SHUTDOWN);
            tInstance->state = DRV_TIMER_state_STOPPED;
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Triggers timer STOP task.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void DRV_TIMER_pauseTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            HAL_TIMER_runTask(tInstance->config->timerReg, DRV_TIMER_task_STOP);
            tInstance->state = DRV_TIMER_state_PAUSED;
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Triggers timer CLEAR task.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void DRV_TIMER_clearTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            HAL_TIMER_runTask(tInstance->config->timerReg, DRV_TIMER_task_CLEAR);
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Triggers timer COUNT task.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 **************************************************************************************************/
void DRV_TIMER_incrementTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            if(tInstance->config->mode != DRV_TIMER_mode_NORMAL) {
                HAL_TIMER_runTask(tInstance->config->timerReg, DRV_TIMER_task_COUNT);
            }
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Captures current timer value to one of its CC[n] registers.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [in]   channel        - timer CC channel.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
uint32_t DRV_TIMER_captureTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;
    uint32_t timerVal = 0u;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            if(tInstance->config->mode == DRV_TIMER_mode_NORMAL) {
                HAL_TIMER_runTask(tInstance->config->timerReg,
                        (DRV_TIMER_task_E) ((uint32_t) DRV_TIMER_task_CAPTURE0 + (channel * sizeof(uint32_t))));
                // get captured value
                timerVal = HAL_TIMER_getValue(tInstance->config->timerReg, channel);
            }
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }

    return timerVal;
}

/***********************************************************************************************//**
 * @brief Sets compare value to given CC register and enables interrupt.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [in]   channel        - timer CC channel.
 * @param [in]   compareValue   - compare value.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    02.01.2021.
 **************************************************************************************************/
void DRV_TIMER_compareEnableTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        uint32_t compareValue,
        DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            if(tInstance->config->mode == DRV_TIMER_mode_NORMAL) {
                // enable interrupt on COMPARE event
                HAL_TIMER_enableInterrupt(tInstance->config->timerReg, (uint32_t) channel);
                HAL_TIMER_writeCompareValue(tInstance->config->timerReg, (uint32_t) channel, compareValue);
            }
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Disables interrupt on compare event.
 ***************************************************************************************************
 * @param [in]   *tInstance     - pointer to timer instance structure.
 * @param [in]   channel        - timer CC channel.
 * @param [out]  *outErr        - error parameter.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    02.01.2021.
 **************************************************************************************************/
void DRV_TIMER_compareDisableTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_isInit[tInstance->config->id] == true) {
            if(tInstance->config->mode == DRV_TIMER_mode_NORMAL) {
                // disable interrupt on COMPARE event
                HAL_TIMER_disableInterrupt(tInstance->config->timerReg, channel);
            }
        } else {
            err = DRV_TIMER_err_NO_TIMER_INSTANCE;
        }
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/***********************************************************************************************//**
 * @brief Function returns time difference in us between two time measurements.
 ***************************************************************************************************
 * @param [in]   *tInstance  - pointer to timer instance structure.
 * @param [in]   *start      - pointer to first timer value.
 * @param [in]   *stop       - pointer to second timer value.
 * @return Time difference between stop and start values [us].
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 **************************************************************************************************/
uint32_t DRV_TIMER_getTimeDiff(const DRV_TIMER_instance_S *tInstance,
        uint32_t *start,
        uint32_t *stop) {

    uint32_t timerDiff = 0u;
    if(*stop < *start) {
        timerDiff = *stop + DRV_TIMER_MAX_VALUE_16_BIT - *start;
    } else {
        timerDiff = *stop - *start;
    }
    float timeResUs = DRV_TIMER_getTimerResolution(tInstance);
    // number of ticks * timer resolution
    float realTimeUs = ((float) timerDiff * timeResUs);

    return (uint32_t) realTimeUs;
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/
/***********************************************************************************************//**
 * @brief Common callback function for timers that calls individual callback.
 ***************************************************************************************************
 * @param [in]   timerInstanceId - timer instance id that requested interrupt.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 **************************************************************************************************/
static void DRV_TIMER_irqHandler(DRV_TIMER_id_E timerInstanceId) {
    // handlers are used for COMPARE events only (?)
    if(DRV_TIMER_isInit[timerInstanceId] == true) {
        for (uint8_t i = 0u; i < (uint8_t) DRV_TIMER_cc_CHANNEL_COUNT; i++) {
            // check on which CC channel was interrupt generated
            if(HAL_TIMER_checkIntEn(timerInstances[timerInstanceId], i) &&
                    HAL_TIMER_checkEvent(timerInstances[timerInstanceId], i)) {
                // clear event register
                HAL_TIMER_clearEvent(timerInstances[timerInstanceId], i);
                // call user provided callback
                DRV_TIMER_controlBlock[timerInstanceId].callbackFunction(i, DRV_TIMER_controlBlock[timerInstanceId].context);
            }
        }
    }
}

/***********************************************************************************************//**
 * @brief Getter function for timer instance time resolution.
 ***************************************************************************************************
 * @param [in]   *tInstance - pointer to timer instance structure.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 **************************************************************************************************/
static float DRV_TIMER_getTimerResolution(const DRV_TIMER_instance_S *tInstance) {

    float timeRes = 0.0f;

    switch(tInstance->config->frequency) {
        case DRV_TIMER_freq_16MHz:
            timeRes = DRV_TIMER_RESOLUTION_16MHZ;
            break;
        case DRV_TIMER_freq_8MHz:
            timeRes = DRV_TIMER_RESOLUTION_8MHZ;
            break;
        case DRV_TIMER_freq_4MHz:
            timeRes = DRV_TIMER_RESOLUTION_4MHZ;
            break;
        case DRV_TIMER_freq_2MHz:
            timeRes = DRV_TIMER_RESOLUTION_2MHZ;
            break;
        case DRV_TIMER_freq_1MHz:
            timeRes = DRV_TIMER_RESOLUTION_1MHZ;
            break;
        case DRV_TIMER_freq_500KHz:
            timeRes = DRV_TIMER_RESOLUTION_500KHZ;
            break;
        case DRV_TIMER_freq_250KHz:
            timeRes = DRV_TIMER_RESOLUTION_250KHZ;
            break;
        case DRV_TIMER_freq_125KHz:
            timeRes = DRV_TIMER_RESOLUTION_125KHZ;
            break;
        case DRV_TIMER_freq_62500Hz:
            timeRes = DRV_TIMER_RESOLUTION_62500HZ;
            break;
        case DRV_TIMER_freq_31250Hz:
            timeRes = DRV_TIMER_RESOLUTION_31250HZ;
            break;
    }

    return timeRes;

}

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
