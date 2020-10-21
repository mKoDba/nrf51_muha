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
#include "nrf51_muha.h"
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
 * @brief Function initializes NRF51422 drivers.
 ******************************************************************************
 * @param [out] *outErr - error parameter.
 ******************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 ******************************************************************************/
static void NRF51_MUHA_initDrivers(ERR_E *outErr) {


    if(outErr != NULL) {

    }
}

/*******************************************************************************
 * @brief Function initializes BSP components used by NRF51422.
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

    // initialize GPIOs
    NRF51_MUHA_initGpio();

    // initialize MCU drivers
    NRF51_MUHA_initDrivers(&localErr);

    // initialize BSP components
    NRF51_MUHA_initBsp(&localErr);

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

}


/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
