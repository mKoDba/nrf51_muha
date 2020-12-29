/*	header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "drv_timer.h"
#include "hal_timer.h"


/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/
#define DRV_TIMER_MAX_VALUE_16_BIT      (65535u)
#define DRV_TIMER_16MHZ_TIME_RESOLUTION (0.0625f)

/*******************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/
static void DRV_TIMER_irqHandler(DRV_TIMER_id_E timerInstanceId);
static float DRV_TIMER_getTimerResolution(const DRV_TIMER_instance_S *tInstance);

/*******************************************************************************
 *                           GLOBAL VARIABLES
 ******************************************************************************/
static NRF_TIMER_Type *timerInstances[DRV_TIMER_id_COUNT] = {
        NRF_TIMER0, NRF_TIMER1, NRF_TIMER2
};

static USER_TIMER_IRQHandler DRV_TIMER_USER_IRQHandlers[DRV_TIMER_id_COUNT] = {
        NULL, NULL, NULL
};

static bool DRV_TIMER_USER_isInit[DRV_TIMER_id_COUNT] = {
        false, false, false
};

/*******************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
//void TIMER0_IRQHandler(void) {
//    DRV_TIMER_irqHandler(DRV_TIMER_id_0);
//}

void TIMER1_IRQHandler(void) {
    DRV_TIMER_irqHandler(DRV_TIMER_id_1);
}

//void TIMER2_IRQHandler(void) {
//    DRV_TIMER_irqHandler(DRV_TIMER_id_2);
//}

/*******************************************************************************
 * @brief Initializes timer instance.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [in]   *timerConfig   - pointer to timer configuration structure.
 * @param [in]   irqHandler     - function pointer to user defined IRQ handler.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 ******************************************************************************/
void DRV_TIMER_init(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_config_S *timerConf,
        USER_TIMER_IRQHandler irqHandler,
        DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL && timerConf != NULL) {
        // set timer configuration
        tInstance->config = timerConf;
        tInstance->state = DRV_TIMER_state_STOPPED;
        // TODO: check if this kind of assignment works, or needs pointer to static table member...
        // set callback function
        DRV_TIMER_USER_IRQHandlers[tInstance->config->id] = irqHandler;
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

        DRV_TIMER_USER_isInit[tInstance->config->id] = true;
        tInstance->isInitialized = true;
    } else {
        err = DRV_TIMER_err_NULL_PARAM;
    }

    if(outErr != NULL) {
        *outErr = err;
    }
}

/*******************************************************************************
 * @brief Triggers timer START task, enabling timer instance.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 ******************************************************************************/
void DRV_TIMER_enableTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_USER_isInit[tInstance->config->id] == true) {
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

/*******************************************************************************
 * @brief Triggers timer SHUTDOWN task, disabling timer instance.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 ******************************************************************************/
void DRV_TIMER_disableTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_USER_isInit[tInstance->config->id] == true) {
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

/*******************************************************************************
 * @brief Triggers timer STOP task.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 ******************************************************************************/
void DRV_TIMER_pauseTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_USER_isInit[tInstance->config->id] == true) {
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

/*******************************************************************************
 * @brief Triggers timer CLEAR task.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 ******************************************************************************/
void DRV_TIMER_clearTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_USER_isInit[tInstance->config->id] == true) {
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

/*******************************************************************************
 * @brief Triggers timer COUNT task.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.12.2020.
 ******************************************************************************/
void DRV_TIMER_incrementTimer(DRV_TIMER_instance_S *tInstance, DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;

    if(tInstance != NULL) {
        if(DRV_TIMER_USER_isInit[tInstance->config->id] == true) {
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

/*******************************************************************************
 * @brief Captures current timer value to one of its CC[n] registers.
 ******************************************************************************
 * @param [in]   *timerInstance - pointer to timer instance structure.
 * @param [in]   channel        - timer CC channel.
 * @param [out]  *outErr        - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 ******************************************************************************/
uint32_t DRV_TIMER_captureTimer(DRV_TIMER_instance_S *tInstance,
        DRV_TIMER_cc_E channel,
        DRV_TIMER_err_E *outErr) {

    DRV_TIMER_err_E err = DRV_TIMER_err_NONE;
    uint32_t timerVal = 0u;

    if(tInstance != NULL) {
        if(DRV_TIMER_USER_isInit[tInstance->config->id] == true) {
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

/*******************************************************************************
 * @brief Function returns time difference in us between two time measurements.
 ******************************************************************************
 * @param [in]   *tInstance  - pointer to timer instance structure.
 * @param [in]   *start      - first timer value.
 * @param [in]   *stop       - second timer value.
 ******************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 ******************************************************************************/
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

/*******************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Common callback function for timers that calls individual callback.
 ******************************************************************************
 * @param [in]   timerInstanceId - timer instance id that requested interrupt.
 ******************************************************************************
 * @author  mario.kodba
 * @date    20.12.2020.
 ******************************************************************************/
static void DRV_TIMER_irqHandler(DRV_TIMER_id_E timerInstanceId) {
    // handlers are used for COMPARE events only (?)
    if(DRV_TIMER_USER_isInit[timerInstanceId] == true) {
        for (uint8_t i = 0; i < (uint8_t) DRV_TIMER_cc_CHANNEL_COUNT; i++) {
            // check on which CC channel was interrupt generated
            if(HAL_TIMER_checkIntEn(timerInstances[timerInstanceId], i) &&
                    HAL_TIMER_checkEvent(timerInstances[timerInstanceId], i)) {
                // clear event register
                HAL_TIMER_clearEvent(timerInstances[timerInstanceId], i);
                // call user provided callback
                DRV_TIMER_USER_IRQHandlers[timerInstanceId];
            }
        }

    }
}

/*******************************************************************************
 * @brief Getter function for timer instance time resolution.
 ******************************************************************************
 * @param [in]   *tInstance - pointer to timer instance structure.
 ******************************************************************************
 * @author  mario.kodba
 * @date    28.12.2020.
 ******************************************************************************/
static float DRV_TIMER_getTimerResolution(const DRV_TIMER_instance_S *tInstance) {

    float timeRes = 0.0f;

    switch(tInstance->config->frequency) {
        case DRV_TIMER_freq_16MHz:
            timeRes = 0.0625f;
            break;
        case DRV_TIMER_freq_8MHz:
            timeRes = 0.125f;
            break;
        case DRV_TIMER_freq_4MHz:
            timeRes = 0.25f;
            break;
        case DRV_TIMER_freq_2MHz:
            timeRes = 0.5f;
            break;
        case DRV_TIMER_freq_1MHz:
            timeRes = 1.0f;
            break;
        case DRV_TIMER_freq_500KHz:
            timeRes = 2.0f;
            break;
        case DRV_TIMER_freq_250KHz:
            timeRes = 4.0f;
            break;
        case DRV_TIMER_freq_125KHz:
            timeRes = 8.0f;
            break;
        case DRV_TIMER_freq_62500Hz:
            timeRes = 16.0f;
            break;
        case DRV_TIMER_freq_31250Hz:
            timeRes = 32.0f;
            break;
    }

    return timeRes;

}

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
