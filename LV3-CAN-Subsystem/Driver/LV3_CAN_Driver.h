#pragma once

#include <stdint.h>
#include "LV3_CAN.h"

#ifdef LV3_CAN_DRIVER_IMPL__LV3_CORE_R0
#include "stm32g4xx_hal.h"
extern TIM_HandleTypeDef htim5;
extern FDCAN_HandleTypeDef hfdcan2;
#define LV3_CAN_HANDLE hfdcan2
#define LV3_CAN_INSTANCE FDCAN2
#endif

#ifdef LV3_CAN_DRIVER_IMPL__LV3_DASH_R0
#include "stm32h7xx_hal.h"
extern FDCAN_HandleTypeDef hfdcan2;
#define LV3_CAN_HANDLE hfdcan2
#define LV3_CAN_INSTANCE FDCAN2
#endif

/**
 * @brief Initialize the LV3 CAN driver for the specific hardware platform
 *
 * @pre The correct LV3_CAN_DRIVER_IMPL__* macro must be defined to select the implementation. Possible values are:
 * - LV3_CAN_DRIVER_IMPL__LV3_CORE_R0: For RSC "LV3 Core" Revision 0 board
 * - LV3_CAN_DRIVER_IMPL__LV3_DASH_R0: For RSC "LV3 Dash" Revision 0 board
 */
void LV3_CAN_Driver_Init();

/**
 * @brief Called by the LV3 CAN library to send a CAN message
 *
 * @param id The raw 29-bit CAN ID of the message
 * @param data Pointer to the data payload
 * @param length Length of the data payload in bytes
 *
 * @return LV3_CAN_Result indicating success or failure to enqueue the message for transmission
 */
LV3_CAN_Result LV3_CAN_Driver_SendMessage(uint32_t id, uint8_t* data, uint8_t length);

/**
 * @brief (TODO) Called by the LV3 CAN library to alert of an update condition
 */
void LV3_CAN_Driver_Alert();

/**
 * @brief Called by the LV3 CAN library to set the module status LED color
 *
 * @param color The color value to set in format 0x00BBGGRR (Blue-Green-Red byte order)
 */
void LV3_CAN_Driver_SetLED(uint32_t color);

/**
 * @brief Called by the LV3 CAN library to get the current system tick in milliseconds
 *
 * @return Current system tick in milliseconds
 */
uint32_t LV3_CAN_Driver_GetTick();

/**
 * @brief Called by the LV3 CAN library to get the current system health data
 *
 * @return Current system health data structure
 */
LV3_CAN_SystemHealthData LV3_CAN_Driver_GetSystemHealthData();

/**
 * @brief Called by the LV3 CAN library to forward received CAN messages from CAN peripherals other than LV3_CAN_INSTANCE
 */
void LV3_CAN_AUX_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);