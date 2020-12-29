/*	header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "bsp_ecg_ADS1192.h"
#include "cfg_drv_spi.h"
#include "cfg_nrf_drv_spi.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/
BSP_ECG_ADS1192_device_S ecgDevice;
BSP_ECG_ADS1192_config_S ecgDeviceConfig = {
        .spiInstance = &instanceSpi0,
        .spiConfig = &configSpi0,

        .samplingRate = BSP_ECG_ADS1192_convRate_250_SPS,
        .pgaSetting = BSP_ECG_ADS1192_pga_6X
};

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
