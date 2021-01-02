/*	header info, to be added
 *
 *
 *
 */


/*******************************************************************************
 *                              INCLUDE FILES
 ******************************************************************************/
#include "drv_common.h"

/*******************************************************************************
 *                              DEFINES
 ******************************************************************************/
#define DRV_COMMON_IRQ_PRIORITY_SHIFT_VALUE    (12U)

/*******************************************************************************
 *                          PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
/*******************************************************************************
 * @brief Sets priority and enables IRQ.
 ******************************************************************************
 * @param [in] *type    - describe what that something is.
 * @param [in] priority - describe what that something is.
 ******************************************************************************
 * @author  mario.kodba
 * @date    19.20.2020.
 ******************************************************************************/
void DRV_COMMON_enableIRQPriority(void *type, uint8_t priority) {
    // TODO: [add check if priority is within range/allowed]
    uint8_t typeNum = (uint8_t)((uint32_t)type >> DRV_COMMON_IRQ_PRIORITY_SHIFT_VALUE);

    NVIC_SetPriority((IRQn_Type) typeNum, priority);
    NVIC_ClearPendingIRQ((IRQn_Type) typeNum);
    NVIC_EnableIRQ((IRQn_Type) typeNum);
}

/*******************************************************************************
 *                         PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *                          END OF FILE
 ******************************************************************************/