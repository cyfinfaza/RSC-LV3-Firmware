#pragma once

// LV3 CAN parameter bindings for the HV-BPS node (ID 12).
//
// Declares the bindings array passed to LV3_CAN_Init, plus the two variables
// that main needs direct access to: lv3c_sw_hv_main (received from the dash)
// and main_contactor_enabled (written by main, then broadcast on CAN).
//
// Note: LV3_CAN_Param enum uses the same lowercase names as the XMacro
// (sw_hv_main, hv_main_active, etc.), so local mirror variables use an
// lv3c_ prefix to avoid redeclaration conflicts.

#include <stdint.h>
#include "LV3_CAN.h"

extern uint32_t lv3c_sw_hv_main;
extern uint32_t main_contactor_enabled;

extern const LV3_CAN_Binding lv3_can_bindings[];
extern const unsigned int lv3_can_bindings_count;
