#include "can_interface.h"

// For each CAN parameter: define the mirror variable and its EEZ Studio
// get/set variable binding functions. The setter is a no-op because these
// are read-only from the UI — values are written by the LV3 CAN subsystem.
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
    uint32_t lv3c_param_##name = 0; \
    uint32_t get_var_lv3c_##name(void) { return lv3c_param_##name; } \
    void set_var_lv3c_##name(int32_t value) { (void)value; }
  LV3_CAN_Parameters_XMacro
#undef X

// Bindings array: each entry links a lv3c_param_* variable to its CAN parameter
// name so LV3_CAN can write incoming values directly into the mirror variable.
// signal_left/signal_right are the exception: the dashboard is the module that
// *originates* the turn signal state (driven by the driver dashboard screen's
// buttons), so those two bind Write instead of Read. They're dedicated
// parameters (rather than reusing sw_turn_left/sw_turn_right) whose CAN ID
// bakes in sender 24 — the dashboard's own module id — to match.
const LV3_CAN_Binding lv3_can_bindings[] = {
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
    {&lv3c_param_##name, name, \
     ((name) == signal_left || (name) == signal_right) ? LV3_CAN_BindMode_Write : LV3_CAN_BindMode_Read},
  LV3_CAN_Parameters_XMacro
#undef X
};

void CAN_Interface_Init(void) {
    LV3_CAN_Init(24, LV3_CAN_BusMode_Normal, lv3_can_bindings,
                 sizeof(lv3_can_bindings) / sizeof(LV3_CAN_Binding));
}

