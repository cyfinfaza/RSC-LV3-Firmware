#include "lv_battery.h"
#include "can_interface.h"
#include "screens.h"
#include "LV3_CAN_Bitfield_XMacro.h"
#include <src/core/lv_obj_tree.h>
#include <src/widgets/label/lv_label.h>
#include <stdint.h>
#include <stddef.h>

// Columns of the fault tables, and the share of the table width each one gets.
// LVGL table column widths are pixels only (no percentage support), so these
// are resolved against each table's actual width in fault_table_size_event_cb.
#define FAULT_TABLE_LABEL_COL 0
#define FAULT_TABLE_STATE_COL 1
#define FAULT_TABLE_LABEL_COL_PCT 70

static void fault_table_size_event_cb(lv_event_t *e) {
    lv_obj_t *table = lv_event_get_target(e);
    int32_t width = lv_obj_get_content_width(table);
    int32_t label_w = width * FAULT_TABLE_LABEL_COL_PCT / 100;

    lv_table_set_col_width(table, FAULT_TABLE_LABEL_COL, label_w);
    lv_table_set_col_width(table, FAULT_TABLE_STATE_COL, width - label_w);
}

// Colors the state column of whichever fault table is being drawn: green for
// OK, red for FAULT. The fault word to read is stashed in the table's LVGL
// user data (see init_fault_table), so this one callback serves both tables
// on this screen. Row index == bit index, which holds because the rows are
// laid out from each XMacro in bit order.
static void fault_table_draw_event_cb(lv_event_t *e) {
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if (base_dsc->part != LV_PART_ITEMS || base_dsc->id2 != FAULT_TABLE_STATE_COL) return;

    lv_draw_label_dsc_t *label_dsc = lv_draw_task_get_label_dsc(draw_task);
    if (label_dsc == NULL) return;

    const uint32_t *fault_word = (const uint32_t *)lv_obj_get_user_data(base_dsc->obj);
    int faulted = (*fault_word >> base_dsc->id1) & 1;
    label_dsc->color = faulted ? lv_color_hex(0xFF0000) : lv_color_hex(0x00CC44);
}

static void init_fault_table(lv_obj_t *table, const uint32_t *fault_word) {
    lv_obj_set_user_data(table, (void *)fault_word);

    lv_obj_add_event_cb(table, fault_table_draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    // Apply the column split now, and again on every later resize. The layout
    // has to be forced first — at init the table has no computed width yet.
    lv_obj_add_event_cb(table, fault_table_size_event_cb, LV_EVENT_SIZE_CHANGED, NULL);
    lv_obj_update_layout(table);
    lv_obj_send_event(table, LV_EVENT_SIZE_CHANGED, NULL);
}

void LVBattery_Init(void) {
    // Bat fault table: one row per bit of lv_bat_faults, labels from the XMacro.
    int row = 0;
    #define X(name, label) \
        lv_table_set_cell_value(objects.lv_bat_fault_table, row, FAULT_TABLE_LABEL_COL, label); \
        row++;
      LV3_CAN_Bitfield_lv_bat_faults_XMacro
    #undef X
    init_fault_table(objects.lv_bat_fault_table, &lv3c_param_lv_bat_faults);

    // DCDC fault table: same idea, from lv_dcdc_faults.
    row = 0;
    #define X(name, label) \
        lv_table_set_cell_value(objects.lv_dcdc_fault_table, row, FAULT_TABLE_LABEL_COL, label); \
        row++;
      LV3_CAN_Bitfield_lv_dcdc_faults_XMacro
    #undef X
    init_fault_table(objects.lv_dcdc_fault_table, &lv3c_param_lv_dcdc_faults);
}

// Last values written, so unchanged fault words don't force an LVGL redraw
// every loop iteration (see the identical pattern in hv_battery.c).
#define NO_CACHED_VALUE UINT32_MAX
static uint32_t last_bat_fault_word = NO_CACHED_VALUE;
static uint32_t last_dcdc_fault_word = NO_CACHED_VALUE;

void LVBattery_Update(void) {
    if (lv3c_param_lv_bat_faults != last_bat_fault_word) {
        last_bat_fault_word = lv3c_param_lv_bat_faults;
        int row = 0;
        #define X(name, label) \
            lv_table_set_cell_value(objects.lv_bat_fault_table, row, FAULT_TABLE_STATE_COL, \
                                    (lv3c_param_lv_bat_faults >> row) & 1 ? "FAULT" : "OK"); \
            row++;
          LV3_CAN_Bitfield_lv_bat_faults_XMacro
        #undef X
    }

    if (lv3c_param_lv_dcdc_faults != last_dcdc_fault_word) {
        last_dcdc_fault_word = lv3c_param_lv_dcdc_faults;
        int row = 0;
        #define X(name, label) \
            lv_table_set_cell_value(objects.lv_dcdc_fault_table, row, FAULT_TABLE_STATE_COL, \
                                    (lv3c_param_lv_dcdc_faults >> row) & 1 ? "FAULT" : "OK"); \
            row++;
          LV3_CAN_Bitfield_lv_dcdc_faults_XMacro
        #undef X
    }
}
