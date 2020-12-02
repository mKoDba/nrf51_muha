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
#include "nrf_drv_spi.h"
#include "nrf_error.h"

#include "nrf51_muha.h"
#include "cfg_nrf_drv_spi.h"
#include "cfg_bsp_ecg_ADS1192.h"
#include "cfg_nrf51_muha_pinout.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                          PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Function initializes GPIO pins as inputs/outputs.
 ******************************************************************************
 * @param None.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
static void NRF51_MUHA_initGpio() {

    // initialize input pins
    nrf_gpio_cfg_input(ECG_DOUT, GPIO_PIN_CNF_PULL_Disabled); // MISO pin
    nrf_gpio_cfg_input(ECG_DRDY, GPIO_PIN_CNF_PULL_Pullup); // active low signal

    // initialize output pins
    nrf_gpio_cfg_output(LD1);
    nrf_gpio_cfg_output(ECG_RST);
    nrf_gpio_cfg_output(ECG_CS);    // ECG_CS has external PULL UP resistor
    nrf_gpio_cfg_output(ECG_DIN);   // MOSI pin
    nrf_gpio_cfg_output(ECG_CLK);

    // TODO: [mario.kodba 20.10.2020.] set other (currently unused) pins
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
    ret_code_t spiInitErr;

    // initialize TIMER0 instance
//    timerInitErr = nrf_drv_timer_init(&timer0Instance, &timer0Config, &timer0EventHandler);
//    if(timerInitErr != NRF_SUCCESS) {
//        drvInitErr = ERR_DRV_TIMER_INIT_FAIL;
//    }

    // initialize SPI0 instance for ECG
    // if passed handler function, nrf_drv_spi_transfer returns immediately (NON-BLOCKING)
    spiInitErr = nrf_drv_spi_init(&spi0Instance, &spi0Config, &spiEventHandler);
    if(spiInitErr != NRF_SUCCESS) {
        drvInitErr = ERR_DRV_SPI_INIT_FAIL;
    }

    if(drvInitErr != ERR_NONE) {
        // init some other driver
    }

    if(outErr != NULL) {
        *outErr = drvInitErr;
    }
}

/*******************************************************************************
 * @brief Function initializes BSP components used by NRF51422 on MUHA board.
 ******************************************************************************
 * @param [in, out] *outErr - error parameter.
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
 *                         PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Function that initializes everything needed for application start.
 ******************************************************************************
 * @param [in, out] *error - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
void NRF51_MUHA_init(ERR_E *outErr) {

    ERR_E localErr = ERR_NONE;

    SEGGER_RTT_printf(0, "Initializing nRF51...\n");

    // initialize GPIOs, it sets LD1
    NRF51_MUHA_initGpio();

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
    // TODO: [mario.kodba 29.11.2020.] Add RTOS start here and remove loop?
    while(true){
        ;
    }
}

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
