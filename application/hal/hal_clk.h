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
 * @file    hal_clk.h
 * @author  mario.kodba
 * @brief   HAL layer for clock module functionality header file.
 **************************************************************************************************/

#ifndef HAL_CLK_H_
#define HAL_CLK_H_

/***************************************************************************************************
 *                              INCLUDE FILES
 **************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf51.h"

/***************************************************************************************************
 *                              DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 *                              DATA STRUCTURES
 **************************************************************************************************/

/***************************************************************************************************
 *                         PUBLIC FUNCTION DECLARATIONS
 **************************************************************************************************/
void HAL_CLK_hfclkStart(void);
void HAL_CLK_hfclkStop(void);


#endif // #ifndef HAL_CLK_H_
/***************************************************************************************************
 *                          END OF FILE
 **************************************************************************************************/
