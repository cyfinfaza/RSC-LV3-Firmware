//See for CAN specs: https://www.dilithiumpower.com/products/photon-3

#include "mppt_interface.h"

#include "stm32g0xx_hal.h"

extern FDCAN_HandleTypeDef hfdcan2;
extern TIM_HandleTypeDef htim1;

mppt_data_t mppt_data[4];

void LV3_CAN_AUX_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (hfdcan->Instance != FDCAN2)
        return;
    if (!(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE))
        return;

    FDCAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);

    uint32_t id = rx_header.Identifier;

    uint32_t index = id - 0x600;    //scale from 0x600-0x603 to 0-3.

    if (index >= 0 && index <= 3) {
        mppt_data[index].arrayVoltage = (uint32_t)((rx_data[1] << 8) + rx_data[0]);
        mppt_data[index].arrayCurrent = (uint32_t)((rx_data[3] << 8) + rx_data[2]);
        mppt_data[index].batteryVoltage = (uint32_t)((rx_data[5] << 8) + rx_data[4]);
        mppt_data[index].batteryTemp = (uint32_t)((rx_data[7] << 8) + rx_data[6]);
    }
}

void MPPT_Init() {
    HAL_FDCAN_Start(&hfdcan2);
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    FDCAN_FilterTypeDef filter = {
        .IdType = FDCAN_STANDARD_ID,
        .FilterIndex = 0,
        .FilterType = FDCAN_FILTER_MASK,
        .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
        .FilterID1 = 0x000,
        .FilterID2 = 0x000,
    };
    HAL_FDCAN_ConfigFilter(&hfdcan2, &filter);
}
