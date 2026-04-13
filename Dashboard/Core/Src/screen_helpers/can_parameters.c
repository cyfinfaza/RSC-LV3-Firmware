#include "can_parameters.h"
#include "can_interface.h"
#include "screens.h"
#include "main.h"
#include <stdio.h>

// Number of rows updated per call to CanParameters_Update.
// Spread the work across multiple loop ticks to avoid stalling the UI.
#define ROWS_PER_UPDATE 2

// Scratch buffer for formatting parameter values as strings.
static char table_value_buf[LV3_CAN_ParamCount][8];

// Tracks which row to start from on the next call; advances by ROWS_PER_UPDATE
// each call and wraps around so all rows are refreshed over time.
static int update_offset = 0;

void CanParameters_Init(void) {
    lv_table_set_col_width(objects.can_parameters_table, 0, 200);
    lv_table_set_col_width(objects.can_parameters_table, 1, 100);
}

void CanParameters_Update(void) {
    // HAL_GPIO_WritePin(LED_USR_GPIO_Port, LED_USR_Pin, GPIO_PIN_SET);

    int row = 0;
    int end = update_offset + ROWS_PER_UPDATE;
    #define X(can_id, name, refresh_interval, ttl, has_safe_state, safe_state) \
        if (row >= update_offset && row < end) { \
            sprintf(table_value_buf[row], "%lu", lv3c_param_##name); \
            lv_table_set_cell_value(objects.can_parameters_table, row, 0, #name); \
            lv_table_set_cell_value(objects.can_parameters_table, row, 1, table_value_buf[row]); \
        } \
        row++;
    LV3_CAN_Parameters_XMacro
    #undef X

    update_offset = (update_offset + ROWS_PER_UPDATE) % LV3_CAN_ParamCount;

    // HAL_GPIO_WritePin(LED_USR_GPIO_Port, LED_USR_Pin, GPIO_PIN_RESET);
}
