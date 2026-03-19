#include "LV3_CAN_Driver.h"

#if defined(LV3_CAN_DRIVER_IMPL__LV3_CORE_R0) ||                               \
    defined(LV3_CAN_DRIVER_IMPL__LV3_DASH_R0)

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if (hfdcan->Instance == LV3_CAN_INSTANCE) {
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
      FDCAN_RxHeaderTypeDef rx_header;
      uint8_t rx_data[8];
      HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);
      LV3_CAN_PushNewMessage(rx_header.Identifier, rx_data,
                             rx_header.DataLength);
    }
  } else {
    LV3_CAN_AUX_RxFifo0Callback(hfdcan, RxFifo0ITs);
  }
}

void LV3_CAN_Driver_Init() {

#ifdef LV3_CAN_DRIVER_IMPL__LV3_CORE_R0

  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1); // G
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); // R
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3); // B

  HAL_FDCAN_Start(&hfdcan2);

  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  // fdcan allow all messages
  FDCAN_FilterTypeDef filter;
  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = 0x000;
  filter.FilterID2 = 0x000;
  HAL_FDCAN_ConfigFilter(&hfdcan2, &filter);

#endif

#ifdef LV3_CAN_DRIVER_IMPL__LV3_DASH_R0

  // This board has no CAN status LED

  HAL_FDCAN_Start(&hfdcan2);

  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  // fdcan allow all messages
  FDCAN_FilterTypeDef filter;
  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = 0x000;
  filter.FilterID2 = 0x000;
  HAL_FDCAN_ConfigFilter(&hfdcan2, &filter);

#endif
}

LV3_CAN_Result LV3_CAN_Driver_SendMessage(uint32_t id, uint8_t *data,
                                          uint8_t length) {
  FDCAN_TxHeaderTypeDef tx_header;
  tx_header.IdType = FDCAN_EXTENDED_ID;
  tx_header.Identifier = id;
  tx_header.TxFrameType = FDCAN_DATA_FRAME;
  tx_header.DataLength = length;
  tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  tx_header.BitRateSwitch = FDCAN_BRS_OFF;
  tx_header.FDFormat = FDCAN_CLASSIC_CAN;
  tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  tx_header.MessageMarker = 0;

  HAL_StatusTypeDef status =
      HAL_FDCAN_AddMessageToTxFifoQ(&LV3_CAN_HANDLE, &tx_header, data);
  if (status != HAL_OK) {
    return LV3_CAN_FAIL;
  }
  return LV3_CAN_OK;
}

void LV3_CAN_Driver_Alert() {
  // TODO: Implement alert mechanism to notify main program
}

void LV3_CAN_Driver_SetLED(uint32_t color) {
#ifdef LV3_CAN_DRIVER_IMPL__LV3_CORE_R0
  __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, color & 0x0000FF00); // G
  __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, color & 0x000000FF); // R
  __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, color & 0x00FF0000); // B
#endif
}

uint32_t LV3_CAN_Driver_GetTick() { return HAL_GetTick(); }

LV3_CAN_SystemHealthData LV3_CAN_Driver_GetSystemHealthData() {
  LV3_CAN_SystemHealthData health = {
      .uptime = LV3_CAN_Driver_GetTick(),
      .v_sense_12 = 0,
      .v_sense_5 = 0,
      .mcu_temp = 0,
      .main_loop_freq = 0,
  };
  // TODO: Implement system health data collection
  return health;
}

#endif // LV3_CAN_DRIVER_IMPL__LV3_CORE_R0 || LV3_CAN_DRIVER_IMPL__LV3_DASH_R0
