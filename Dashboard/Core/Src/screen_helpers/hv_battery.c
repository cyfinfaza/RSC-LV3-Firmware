#include "hv_battery.h"
#include "can_interface.h"
#include "actions.h"
#include "screens.h"
#include <src/core/lv_obj_tree.h>
#include <src/widgets/label/lv_label.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define CELLS_PER_STACK 12
#define STACKS 2
#define UPPER_VOLTAGE_THRESHOLD 4200  // mV
#define LOWER_VOLTAGE_THRESHOLD 2800  // mV

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

void HVBattery_Init(void) {
    for (int i = 0; i < CELLS_PER_STACK * STACKS; i++) {
        lv_obj_t *table = (i < CELLS_PER_STACK) ? objects.cell_voltages_1_12 : objects.cell_voltages_13_24;
        int table_idx = (i < CELLS_PER_STACK) ? i : (CELLS_PER_STACK - 1 - (i % CELLS_PER_STACK));
        lv_obj_t *table_cell = lv_obj_get_child(table, table_idx);
        char buf[16];
        sprintf(buf, "%d", i + 1);
        lv_label_set_text(lv_obj_get_child(table_cell, 0), buf);
    }
}

void HVBattery_Update(void) {
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
    
    // Second pass: update labels with colors
    for (int i = 0; i < CELLS_PER_STACK * STACKS; i++) {
        uint32_t voltage = *cell_voltages[i] / 10;
        char buf[16];
        sprintf(buf, "%.3f V", voltage / 1000.0f);
        lv_obj_t *table = (i < CELLS_PER_STACK) ? objects.cell_voltages_1_12 : objects.cell_voltages_13_24;
        int table_idx = (i < CELLS_PER_STACK) ? i : (CELLS_PER_STACK - 1 - (i % CELLS_PER_STACK));
        lv_obj_t *cell = lv_obj_get_child(table, table_idx);
        lv_obj_t *cell_label = lv_obj_get_child(cell, 1);
        lv_label_set_text(cell_label, buf);
        
        // Apply color to all max (red) and min (cyan) cell voltages
        if (voltage > UPPER_VOLTAGE_THRESHOLD || voltage < LOWER_VOLTAGE_THRESHOLD) {
            lv_obj_set_style_text_color(cell, lv_color_hex(0xFF0000), 0);  // Bright Red for out-of-range
        } else if (voltage == max_voltage) {
            lv_obj_set_style_text_color(cell, lv_color_hex(0xFF8800), 0);  // Orange
        } else if (voltage == min_voltage) {
            lv_obj_set_style_text_color(cell, lv_color_hex(0x0088FF), 0);  // Cyan
        } else {
            lv_obj_set_style_text_color(cell, lv_color_hex(0xFFFFFF), 0);  // White for normal
        }
    }
}
