#include "can_parameters.h"
#include "can_interface.h"
#include "screens.h"
#include "main.h"
#include <stdio.h>

// Scratch buffer for formatting a parameter value as a string. Must fit the
// widest "%lu" of a uint32_t: 10 digits plus NUL. Parameters carrying signed
// values (hv_pack_current when charging, hv_pack_temp below freezing) arrive as
// large unsigned values and do hit that width.
#define TABLE_VALUE_BUF_LEN 11

// Last value written into each row's value cell. lv_table_set_cell_value
// reallocs and invalidates unconditionally — it never checks whether the value
// actually changed — so calling it every loop iteration churns the LVGL heap
// continuously and fragments it. Writing only on change takes the steady-state
// cost to zero. The invariant is: last_value[row] is what row currently shows,
// which CanParameters_Init establishes by drawing every row once.
static uint32_t last_value[LV3_CAN_ParamCount];

void CanParameters_Init(void) {
    lv_table_set_col_width(objects.can_parameters_table, 0, 200);
    lv_table_set_col_width(objects.can_parameters_table, 1, 100);

    // Names never change, so they are written once here and never touched again.
    // Values are drawn once to seed last_value.
    int row = 0;
    #define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
        { \
            char buf[TABLE_VALUE_BUF_LEN]; \
            last_value[row] = lv3c_param_##name; \
            snprintf(buf, sizeof(buf), "%lu", lv3c_param_##name); \
            lv_table_set_cell_value(objects.can_parameters_table, row, 0, #name); \
            lv_table_set_cell_value(objects.can_parameters_table, row, 1, buf); \
            row++; \
        }
    LV3_CAN_Parameters_XMacro
    #undef X
}

void CanParameters_Update(void) {
    // Every row is checked every call, but only rows whose value actually moved
    // are redrawn. The comparison is a plain integer compare, so there's no need
    // to spread the work across ticks the way the old round-robin did.
    int row = 0;
    #define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
        if (lv3c_param_##name != last_value[row]) { \
            char buf[TABLE_VALUE_BUF_LEN]; \
            last_value[row] = lv3c_param_##name; \
            snprintf(buf, sizeof(buf), "%lu", lv3c_param_##name); \
            lv_table_set_cell_value(objects.can_parameters_table, row, 1, buf); \
        } \
        row++;
    LV3_CAN_Parameters_XMacro
    #undef X
}
