/***********************************************************************************************//**
 * Copyright 2021 Mario Kodba
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ***************************************************************************************************
 * @file    main.c
 * @author  mario.kodba
 * @brief   Main of "MUHA" board application source file.
 **************************************************************************************************/

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf51_muha.h"

#include "cfg_bsp_ecg_ADS1192.h"
#include "cfg_bsp_mpu9150.h"
#include "cfg_ble_muha.h"
#include "cfg_drv_timer.h"

#include "nrf_delay.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 **************************************************************************************************/
/***********************************************************************************************//**
 * @brief Application main function.
 ***************************************************************************************************
 * @param [in]  - None.
 ***************************************************************************************************
 * @author  mario.kodba
 * @date    18.10.2020.
 **************************************************************************************************/
int main(void) {

    nrf_delay_ms(200u);

    ERR_E error = ERR_NONE;

    // in case NRF logging is used
    (void) NRF_LOG_INIT(NULL);

    NRF51_MUHA_handle_S muha;
    muha.ads1192 = &ecgDevice;
    muha.mpu9150 = &mpuDevice;
    muha.customService = &customService;
    muha.timer1 = &instanceTimer1;

    NRF51_MUHA_init(&muha, &error);

    if(error == ERR_NONE) {
        NRF51_MUHA_start(&muha, &error);
    }

    // should not get to here
    return 0;
}

/***************************************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
