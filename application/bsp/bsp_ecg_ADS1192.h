/*
 * bsp_ecg_ADS1192.h
 *
 *  Created on: 21. lis 2020.
 *      Author: Mario
 */

#ifndef BSP_ECG_ADS1192_H_
#define BSP_ECG_ADS1192_H_

/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 *                              ENUMERATIONS
 ******************************************************************************/
typedef enum BSP_ECG_ADS1192_err_ENUM {
    BSP_ECG_ADS1192_err_NONE        = 0u,
    BSP_ECG_ADS1192_err_INIT        = 1u,
    BSP_ECG_ADS1192_err_SPI         = 2u
} BSP_ECG_ADS1192_err_E;

/*******************************************************************************
 *                              DATA STRUCTURES
 ******************************************************************************/
//! ECG ADS1192 driver configuration structure
typedef struct BSP_ECG_ADS1192_config_STRUCT {

} BSP_ECG_ADS1192_config_S;

//! ECG ADS1192 driver device structure
typedef struct BSP_ECG_ADS1192_device_STRUCT {
    BSP_ECG_ADS1192_config_S    *config;        //!< Pointer to ECG driver configuration

    uint8_t isInitialized;                      //!< Is device initialized
} BSP_ECG_ADS1192_device_S;
/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/


/*******************************************************************************
 *                        PUBLIC FUNCTION DECLARATIONS
 ******************************************************************************/
void BSP_ECG_ADS1192_init(BSP_ECG_ADS1192_device_S *inDevice,
                          BSP_ECG_ADS1192_config_S *inConfig,
                          BSP_ECG_ADS1192_err_E *outErr);

#endif // #ifndef BSP_ECG_ADS1192_H_
/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
