/*
 * hal_timer.h
 *
 *  Created on: 19.20.2020.
 *  Author: mario.kodba
 */

#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include <stdint.h>
#include <stddef.h>

#include "nrf51.h"
#include "drv_timer.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/

/*******************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void HAL_TIMER_setFrequency(NRF_TIMER_Type *tInstance, DRV_TIMER_freq_E frequency);
void HAL_TIMER_setBitWidth(NRF_TIMER_Type *tInstance, DRV_TIMER_bitWidth_E bitWidth);
void HAL_TIMER_setMode(NRF_TIMER_Type *tInstance, DRV_TIMER_mode_E mode);

void HAL_TIMER_clearEvents(NRF_TIMER_Type *tInstance);
void HAL_TIMER_clearEvent(NRF_TIMER_Type *tInstance, uint8_t channel);
void HAL_TIMER_runTask(NRF_TIMER_Type *tInstance, DRV_TIMER_task_E task);
DRV_TIMER_event_E HAL_TIMER_getEvent(uint8_t channel);
bool HAL_TIMER_checkEvent(NRF_TIMER_Type *tInstance, uint8_t channel);
bool HAL_TIMER_checkIntEn(NRF_TIMER_Type *tInstance, uint8_t channel);
void HAL_TIMER_enableInterrupt(NRF_TIMER_Type *tInstance, uint8_t channel);
void HAL_TIMER_disableInterrupt(NRF_TIMER_Type *tInstance, uint8_t channel);
void HAL_TIMER_writeCompareValue(NRF_TIMER_Type *tInstance,
        uint8_t channel,
        uint32_t compareValue);
uint32_t HAL_TIMER_getValue(NRF_TIMER_Type *tInstance, DRV_TIMER_cc_E channel);

#endif // #ifndef HAL_TIMER_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
