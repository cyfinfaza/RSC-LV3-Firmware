#include "hv_battery.h"
#include "can_interface.h"
#include "actions.h"
#include "screens.h"
#include "LV3_CAN_Bitfield_XMacro.h"
#include <src/core/lv_obj_tree.h>
#include <src/widgets/label/lv_label.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define CELLS_PER_STACK 12
#define STACKS 2
#define UPPER_VOLTAGE_THRESHOLD 4200  // mV
#define LOWER_VOLTAGE_THRESHOLD 2800  // mV

// Columns of hv_fault_table, and the share of the table width each one gets.
// LVGL table column widths are pixels only (no percentage support), so these
// are resolved against the table's actual width in fault_table_size_event_cb.
#define FAULT_TABLE_LABEL_COL 0
#define FAULT_TABLE_STATE_COL 1
#define FAULT_TABLE_LABEL_COL_PCT 70

const uint32_t* cell_voltages[24] = {
    &lv3c_param_hv_cell_voltage_1,  &lv3c_param_hv_cell_voltage_2,  &lv3c_param_hv_cell_voltage_3,  &lv3c_param_hv_cell_voltage_4,
    &lv3c_param_hv_cell_voltage_5,  &lv3c_param_hv_cell_voltage_6,  &lv3c_param_hv_cell_voltage_7,  &lv3c_param_hv_cell_voltage_8,
    &lv3c_param_hv_cell_voltage_9,  &lv3c_param_hv_cell_voltage_10, &lv3c_param_hv_cell_voltage_11, &lv3c_param_hv_cell_voltage_12,
    &lv3c_param_hv_cell_voltage_13, &lv3c_param_hv_cell_voltage_14, &lv3c_param_hv_cell_voltage_15, &lv3c_param_hv_cell_voltage_16,
    &lv3c_param_hv_cell_voltage_17, &lv3c_param_hv_cell_voltage_18, &lv3c_param_hv_cell_voltage_19, &lv3c_param_hv_cell_voltage_20,
    &lv3c_param_hv_cell_voltage_21, &lv3c_param_hv_cell_voltage_22, &lv3c_param_hv_cell_voltage_23, &lv3c_param_hv_cell_voltage_24
};

void action_toggle_hvbps_local_enable(lv_event_t *e) {
    (void)e;
    LV3_CAN_SendTrigger(LV3_CAN_Trigger_toggle_hv_bps_local_enable, 4, NULL, 0);
}

void action_send_bms_obd2_clear(lv_event_t *e) {
    (void)e;
    LV3_CAN_SendTrigger(LV3_CAN_Trigger_send_orion_obd2_clear, 4, NULL, 0);
}

// Resolves the percentage column split into pixels. Runs whenever the table is
// resized, so the split follows the table however EEZ-Studio sizes it.
static void fault_table_size_event_cb(lv_event_t *e) {
    lv_obj_t *table = lv_event_get_target(e);
    int32_t width = lv_obj_get_content_width(table);
    int32_t label_w = width * FAULT_TABLE_LABEL_COL_PCT / 100;

    lv_table_set_col_width(table, FAULT_TABLE_LABEL_COL, label_w);
    lv_table_set_col_width(table, FAULT_TABLE_STATE_COL, width - label_w);
}

// Colors the state column of the fault table: green for OK, red for FAULT.
// LVGL has no per-cell text color, so the color is applied at draw time by
// re-reading the bit for the row being drawn. Row index == bit index, which
// holds because the rows are laid out from the hv_fault XMacro in bit order.
static void fault_table_draw_event_cb(lv_event_t *e) {
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if (base_dsc->part != LV_PART_ITEMS || base_dsc->id2 != FAULT_TABLE_STATE_COL) return;

    lv_draw_label_dsc_t *label_dsc = lv_draw_task_get_label_dsc(draw_task);
    if (label_dsc == NULL) return;

    int faulted = (lv3c_param_hv_fault >> base_dsc->id1) & 1;
    label_dsc->color = faulted ? lv_color_hex(0xFF0000) : lv_color_hex(0x00CC44);
}

void HVBattery_Init(void) {
    // Fault table: one row per bit of hv_fault, labels straight from the XMacro.
    int row = 0;
    #define X(name, label) \
        lv_table_set_cell_value(objects.hv_fault_table, row, FAULT_TABLE_LABEL_COL, label); \
        row++;
      LV3_CAN_Bitfield_hv_fault_XMacro
    #undef X

    lv_obj_add_event_cb(objects.hv_fault_table, fault_table_draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(objects.hv_fault_table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    // Apply the column split now, and again on every later resize. The layout
    // has to be forced first — at init the table has no computed width yet.
    lv_obj_add_event_cb(objects.hv_fault_table, fault_table_size_event_cb, LV_EVENT_SIZE_CHANGED, NULL);
    lv_obj_update_layout(objects.hv_fault_table);
    lv_obj_send_event(objects.hv_fault_table, LV_EVENT_SIZE_CHANGED, NULL);

    for (int i = 0; i < CELLS_PER_STACK * STACKS; i++) {
        lv_obj_t *table = (i < CELLS_PER_STACK) ? objects.cell_voltages_1_12 : objects.cell_voltages_13_24;
        int table_idx = (i < CELLS_PER_STACK) ? i : (CELLS_PER_STACK - 1 - (i % CELLS_PER_STACK));
        lv_obj_t *table_cell = lv_obj_get_child(table, table_idx);
        char buf[16];
        sprintf(buf, "%d", i + 1);
        lv_label_set_text(lv_obj_get_child(table_cell, 0), buf);
    }
}

// Last values written to the widgets. HVBattery_Update runs every main loop
// iteration, and none of the LVGL setters check whether the value actually
// changed — each one reallocs and invalidates unconditionally, which would keep
// LVGL redrawing this screen forever. So we only call them when something moved.
// NO_CACHED_VALUE never compares equal to a real value, forcing the first write.
#define NO_CACHED_VALUE UINT32_MAX
static uint32_t last_fault_word = NO_CACHED_VALUE;
static uint32_t last_voltage[CELLS_PER_STACK * STACKS] = {[0 ... CELLS_PER_STACK * STACKS - 1] = NO_CACHED_VALUE};
static uint32_t last_color[CELLS_PER_STACK * STACKS] = {[0 ... CELLS_PER_STACK * STACKS - 1] = NO_CACHED_VALUE};

void HVBattery_Update(void) {
    // Fault table state column. Text only — the green/red coloring is applied
    // in fault_table_draw_event_cb. Row index == bit index (XMacro order).
    if (lv3c_param_hv_fault != last_fault_word) {
        last_fault_word = lv3c_param_hv_fault;
        int fault_row = 0;
        #define X(name, label) \
            lv_table_set_cell_value(objects.hv_fault_table, fault_row, FAULT_TABLE_STATE_COL, \
                                    (lv3c_param_hv_fault >> fault_row) & 1 ? "FAULT" : "OK"); \
            fault_row++;
          LV3_CAN_Bitfield_hv_fault_XMacro
        #undef X
    }

    // First pass: find max and min voltages
    uint32_t min_voltage = *cell_voltages[0] / 10;
    uint32_t max_voltage = *cell_voltages[0] / 10;
    
    for (int i = 1; i < CELLS_PER_STACK * STACKS; i++) {
        uint32_t voltage = *cell_voltages[i] / 10;
        // Skip out-of-range values when finding min/max
        if (voltage > UPPER_VOLTAGE_THRESHOLD || voltage < LOWER_VOLTAGE_THRESHOLD) {
            continue;
        }
        if (voltage < min_voltage) {
            min_voltage = voltage;
        }
        if (voltage > max_voltage) {
            max_voltage = voltage;
        }
    }
    
    // Second pass: update labels with colors. Text and color are cached
    // separately: a cell's color can change without its voltage changing, since
    // max/min depend on the other cells.
    for (int i = 0; i < CELLS_PER_STACK * STACKS; i++) {
        uint32_t voltage = *cell_voltages[i] / 10;

        // Pick the color for all max (orange) and min (cyan) cell voltages
        uint32_t color;
        if (voltage > UPPER_VOLTAGE_THRESHOLD || voltage < LOWER_VOLTAGE_THRESHOLD) {
            color = 0xFF0000;  // Bright Red for out-of-range
        } else if (voltage == max_voltage) {
            color = 0xFF8800;  // Orange
        } else if (voltage == min_voltage) {
            color = 0x0088FF;  // Cyan
        } else {
            color = 0xFFFFFF;  // White for normal
        }

        if (voltage == last_voltage[i] && color == last_color[i]) continue;

        lv_obj_t *table = (i < CELLS_PER_STACK) ? objects.cell_voltages_1_12 : objects.cell_voltages_13_24;
        int table_idx = (i < CELLS_PER_STACK) ? i : (CELLS_PER_STACK - 1 - (i % CELLS_PER_STACK));
        lv_obj_t *cell = lv_obj_get_child(table, table_idx);

        if (voltage != last_voltage[i]) {
            last_voltage[i] = voltage;
            // Integer formatting: voltage is already in mV, and float sprintf
            // pulls in newlib's float engine, which is far too slow to run here.
            char buf[16];
            sprintf(buf, "%lu.%03lu V", voltage / 1000, voltage % 1000);
            lv_label_set_text(lv_obj_get_child(cell, 1), buf);
        }

        if (color != last_color[i]) {
            last_color[i] = color;
            lv_obj_set_style_text_color(cell, lv_color_hex(color), 0);
        }
    }
}
