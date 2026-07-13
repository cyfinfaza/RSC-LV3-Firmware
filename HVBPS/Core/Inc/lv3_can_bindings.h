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
#include "LV3_CAN_Bitfield_XMacro.h"

extern uint32_t lv3c_sw_hv_main;
extern uint32_t main_contactor_enabled;

// HV-BPS fault word, evaluated by main each loop and bound to the hv_fault CAN
// parameter via .raw. The flag list and its bit order come from the shared
// hv_fault XMacro; packed so the flags occupy bits 0..N-1 of raw with no padding.
#define X(name, label) uint32_t name : 1;
typedef union {
  struct __attribute__((packed)) {
    LV3_CAN_Bitfield_hv_fault_XMacro
  } flags;
  uint32_t raw;
} hvbps_faults_t;
#undef X

extern hvbps_faults_t hvbps_faults;

extern const LV3_CAN_Binding lv3_can_bindings[];
extern const unsigned int lv3_can_bindings_count;

// Volatile flags set by LV3_CAN_TriggerCallback on trigger receipt (ISR context); checked and cleared in main loop
extern volatile uint8_t flag_toggle_local_enable;
extern volatile uint8_t flag_send_orion_clear;
