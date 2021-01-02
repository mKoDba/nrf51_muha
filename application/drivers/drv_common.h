/*
 * drv_common.h
 *
 *  Created on: 19.12.2020.
 *  Author: mario.kodba
 */

#ifndef DRV_COMMON_H_
#define DRV_COMMON_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "nrf.h"

#include <stdint.h>
/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/

/*******************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void DRV_COMMON_enableIRQPriority(void *type, uint8_t priority);

#endif // #ifndef DRV_COMMON_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/