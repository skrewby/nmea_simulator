#include "bsp/can.h"

#include "hal.h"

uint32_t bsp_can_transmit(uint32_t id, const uint8_t data[8]) {
    CAN_TxHeaderTypeDef header = {
        .ExtId = id,
        .IDE = CAN_ID_EXT,
        .RTR = CAN_RTR_DATA,
        .DLC = 8,
        .TransmitGlobalTime = DISABLE,
    };

    uint32_t mailbox;
    if (HAL_CAN_AddTxMessage(&hcan1, &header, (uint8_t *)data, &mailbox) != HAL_OK) {
        return HAL_CAN_GetError(&hcan1);
    }

    return 0;
}
