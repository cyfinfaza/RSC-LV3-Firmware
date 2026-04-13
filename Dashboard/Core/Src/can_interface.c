#include "can_interface.h"
#include "main.h"
#include "screens.h" // for objects.can_parameters_table (EEZ Studio generated)
#include <stdio.h>

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
// name so LV3_CAN can write incoming values directly into the mirror variable
const LV3_CAN_Binding lv3_can_bindings[] = {
#define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
    {&lv3c_param_##name, name, LV3_CAN_BindMode_Read},
  LV3_CAN_Parameters_XMacro
#undef X
};

// Scratch buffer used to format parameter values as strings for the debug table
static char can_parameters_table_buffer[LV3_CAN_ParamCount][8];

void CAN_Interface_Init(void) {
    LV3_CAN_Init(98, LV3_CAN_BusMode_Normal, lv3_can_bindings,
                 sizeof(lv3_can_bindings) / sizeof(LV3_CAN_Binding));

    // Initial column widths for the CAN parameters debug table
    lv_table_set_col_width(objects.can_parameters_table, 0, 200);
    lv_table_set_col_width(objects.can_parameters_table, 1, 100);
}

void action_update_can_parameters_table(void) {
    // LED blinks while updating to give a visual heartbeat on the debug screen
    HAL_GPIO_WritePin(LED_USR_GPIO_Port, LED_USR_Pin, GPIO_PIN_SET);
    int row = 0;
    #define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
        sprintf(can_parameters_table_buffer[row], "%lu", lv3c_param_##name); \
        lv_table_set_cell_value(objects.can_parameters_table, row, 0, #name); \
        lv_table_set_cell_value(objects.can_parameters_table, row, 1, can_parameters_table_buffer[row]); \
        row++;
    LV3_CAN_Parameters_XMacro
    #undef X
    HAL_GPIO_WritePin(LED_USR_GPIO_Port, LED_USR_Pin, GPIO_PIN_RESET);
}
