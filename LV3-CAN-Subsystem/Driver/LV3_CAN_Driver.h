#pragma once

#include <stdint.h>
#include "LV3_CAN.h"

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