/*
 * nrf51_muha.h
 *
 *  Created on: 18. lis 2020.
 *      Author: Mario
 */

#ifndef NRF51_MUHA_H_
#define NRF51_MUHA_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/


/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/
typedef enum ERR_ENUM {
    ERR_NONE            = 0u,
    ERR_INIT_FAIL       = 1u
} ERR_E;

/*******************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void NRF51_MUHA_init(ERR_E *error);
void NRF51_MUHA_start();

#endif // #ifndef NRF51_MUHA_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
