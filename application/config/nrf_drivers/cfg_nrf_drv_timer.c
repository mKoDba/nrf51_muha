/*	header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "cfg_nrf_drv_timer.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/
nrf_drv_timer_t timer0Instance = {
        .p_reg              = &NRF_TIMER0,  //!< TIMER peripheral registers
        .instance_id        = TIMER0_INSTANCE_INDEX,  //!< TIMER instance index
        .cc_channel_count   = NRF_TIMER_CC_CHANNEL0,  //!< TIMER compare channel
};

nrf_drv_spi_config_t timer0Config = {
        .frequency          = (nrf_timer_frequency_t)TIMER_DEFAULT_CONFIG_FREQUENCY,
        .mode               = (nrf_timer_mode_t)TIMER_DEFAULT_CONFIG_MODE,
        .bit_width          = (nrf_timer_bit_width_t)TIMER_DEFAULT_CONFIG_BIT_WIDTH,
        .interrupt_priority = TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
        .p_context          = NULL
};

/*******************************************************************************
 *                         PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
void timer0EventHandler(nrf_timer_event_t event_type, void* p_context) {

    static uint32_t timer0Ticks;
    timer0Ticks++;
}

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/
