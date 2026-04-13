#pragma once

// CAN parameter bindings between the LV3 CAN subsystem and the EEZ Studio UI.
//
// The LV3 CAN subsystem keeps a synchronized parameter table across all nodes
// on the bus. This module declares a local mirror variable (lv3c_param_<name>)
// for each parameter, registers them with LV3_CAN as read bindings so they are
// updated automatically, and exposes EEZ Studio get/set variable functions so
// the UI can display live values.
//
// Screen-specific UI logic (populating table widgets, etc.) lives in
// Core/Inc/screen_helpers/ rather than here.

#include <stdint.h>
#include "LV3_CAN.h"

// For each CAN parameter: declare the mirror variable and its EEZ Studio
// get/set variable binding functions
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
    extern uint32_t lv3c_param_##name; \
    uint32_t get_var_lv3c_##name(void); \
    void set_var_lv3c_##name(int32_t value); // read-only from EEZ; setter is a no-op
  LV3_CAN_Parameters_XMacro
#undef X

// The bindings array passed to LV3_CAN_Init, linking each lv3c_param_* variable
// to its corresponding parameter name on the CAN bus
extern const LV3_CAN_Binding lv3_can_bindings[];

// Initialize the LV3 CAN subsystem with the parameter bindings
void CAN_Interface_Init(void);
