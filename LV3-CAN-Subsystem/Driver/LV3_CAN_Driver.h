#pragma once

#include <stdint.h>
#include "LV3_CAN.h"

// Initialize the CAN driver
void LV3_CAN_Driver_Init();

// Returns 0 for success
LV3_CAN_Result LV3_CAN_Driver_SendMessage(uint32_t id, uint8_t *data, uint8_t length);

// To alert the main program of an update
void LV3_CAN_Driver_Alert();

void LV3_CAN_Driver_SetLED(uint32_t color);

uint32_t LV3_CAN_Driver_GetTick();

LV3_CAN_SystemHealthData LV3_CAN_Driver_GetSystemHealthData();