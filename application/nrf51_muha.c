/*	header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include <stddef.h>

#include "SEGGER_RTT.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_clock.h"
#include "nrf_error.h"
#include "drv_timer.h"
#include "drv_spi.h"
#include "hal_clk.h"
#include "nrf51_muha.h"

#include "cfg_nrf_drv_spi.h"
#include "cfg_drv_timer.h"
#include "cfg_drv_spi.h"
#include "cfg_bsp_ecg_ADS1192.h"
#include "cfg_nrf51_muha_pinout.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                          PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/
static void NRF51_MUHA_initGpio(ERR_E *outErr);
static void NRF51_MUHA_initDrivers(ERR_E *outErr);
static void NRF51_MUHA_initBsp(ERR_E *outErr);

/*******************************************************************************
 *                         PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
void testTimerHandler(DRV_TIMER_cc_E timerEvent, void *context) {
    static uint32_t ticks;
    ticks++;
}

/*******************************************************************************
 * @brief Initializes GPIOs, drivers, BSP needed for application start.
 ******************************************************************************
 * @param [in, out] *error - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
void NRF51_MUHA_init(ERR_E *outErr) {

    ERR_E localErr = ERR_NONE;

    SEGGER_RTT_printf(0, "Initializing nRF51...\n");

    // initialize GPIOs
    NRF51_MUHA_initGpio(&localErr);

    // initialize NRF peripheral drivers
    NRF51_MUHA_initDrivers(&localErr);

    // initialize BSP components
    if(localErr == ERR_NONE) {
        NRF51_MUHA_initBsp(&localErr);
    }

    if(outErr != NULL) {
        *outErr = localErr;
    }
}

/*******************************************************************************
 * @brief Function starts main application.
 ******************************************************************************
 * @param None.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
void NRF51_MUHA_start() {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    BSP_ECG_ADS1192_startEcgReading(&ecgDevice, &ecgErr);

    // TODO: [mario.kodba 29.11.2020.] Add RTOS start here and remove loop?
    while(true){
        ;
    }
}

/*******************************************************************************
 *                          PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Function initializes GPIO pins as inputs/outputs.
 * @details SPI pins are initialized in separate SPI initialization functions.
 ******************************************************************************
 * @param [out] *outErr - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
static void NRF51_MUHA_initGpio(ERR_E *outErr) {

    ret_code_t gpioErr = NRF_SUCCESS;
    // pin interrupt on transition high->low, since nDRDY is active low
    nrf_drv_gpiote_in_config_t inPinConfig = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    // initialize GPIO task event
    gpioErr = nrf_drv_gpiote_init();

    // configure interrupt pin for ECG_DRDY signal
    if(gpioErr == NRF_SUCCESS) {
       gpioErr = nrf_drv_gpiote_in_init(ECG_DRDY, &inPinConfig, BSP_ECG_ADS1192_DrdyPin_IRQHandler);
    }

    // initialize output pins
    nrf_gpio_cfg_output(LD1);
    nrf_gpio_cfg_output(ECG_RST);

    if(gpioErr != NRF_SUCCESS && outErr != NULL) {
        *outErr = ERR_GPIO_INIT_FAIL;
    }
}

/*******************************************************************************
 * @brief Function initializes NRF51422 peripherals drivers.
 ******************************************************************************
 * @param [in, out] *outErr - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
static void NRF51_MUHA_initDrivers(ERR_E *outErr) {

    ERR_E drvInitErr = ERR_NONE;
    DRV_TIMER_err_E timerErr = DRV_TIMER_err_NONE;
    DRV_SPI_err_E spiErr = DRV_SPI_err_NONE;

    // initialize HFCLK needed for TIMER1 instance
    HAL_CLK_hfclkStart();

    // initialize TIMER1 instance
    DRV_TIMER_init(&instanceTimer1, &configTimer1, testTimerHandler, &timerErr);

    if(drvInitErr == ERR_NONE) {
        // start the timer instance
        DRV_TIMER_enableTimer(&instanceTimer1, &timerErr);
        if(drvInitErr == ERR_NONE) {
            // initialize SPI instance
            DRV_SPI_init(&instanceSpi0, &configSpi0, NULL, &spiErr);
            if(drvInitErr == ERR_NONE) {
                // other initializations
            } else {
                drvInitErr = ERR_DRV_SPI_INIT_FAIL;
            }
        } else {
            drvInitErr = ERR_DRV_TIMER_INIT_FAIL;
        }
    } else {
        drvInitErr = ERR_DRV_TIMER_INIT_FAIL;
    }

    if(outErr != NULL) {
        *outErr = drvInitErr;
    }
}

/*******************************************************************************
 * @brief Function initializes BSP components used by NRF51422 on MUHA board.
 ******************************************************************************
 * @param [out] *outErr - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
static void NRF51_MUHA_initBsp(ERR_E *outErr) {

    BSP_ECG_ADS1192_err_E ecgErr = BSP_ECG_ADS1192_err_NONE;

    BSP_ECG_ADS1192_init(&ecgDevice, &ecgDeviceConfig, &ecgErr);

    if(outErr != NULL) {
        *outErr = ecgErr;
    }
}

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
