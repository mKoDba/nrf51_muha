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
 * @brief   Main of MUHA board application source file.
 **************************************************************************************************/

/***********************************************************************************************//**
* @mainpage NRF51 MUHA board - Master's project
****************************************************************************************************
* Introduction
* ------------
*
* This user manual describes software architecture of NRF51 MUHA board and its functionality.
* It was specifically made as part of Master's project on Faculty of Electrical Engineering and
* Computing at University of Zagreb.
*
* Folder structure is divided into a number of subfolders, each representing specific layers
* of functionality.
* - Application
*    * BSP (board support package)
*    * Config (configuration files)
*    * Drivers (NRF51 peripheral drivers)
*    * HAL (Hardware abstraction layer)
* - SDK_components (Nordic SDK)
* - SEGGER_RTT (Segger's library)
*
*
***************************************************************************************************/


/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include "nrf51_muha.h"

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

    ERR_E error = ERR_NONE;

    NRF51_MUHA_init(&error);

    if(error == ERR_NONE) {
        NRF51_MUHA_start();
    }

    while(1) {
        ;
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
