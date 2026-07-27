#include "driver_dashboard.h"
#include "can_interface.h"
#include "screens.h"
#include "main.h"
#include "lvgl.h"

#define BLINK_INTERVAL_MS 500
#define SIGNAL_COLOR_AMBER 0xFFBF00

typedef enum {
    ActiveSignal_None,
    ActiveSignal_Left,
    ActiveSignal_Right,
    ActiveSignal_Hazards
} ActiveSignal;

static ActiveSignal active_signal = ActiveSignal_None;
static uint8_t blink_state = 0;
static uint32_t blink_tick = 0;

static void left_signal_button_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    active_signal = (active_signal == ActiveSignal_Left) ? ActiveSignal_None : ActiveSignal_Left;
}

static void right_signal_button_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    active_signal = (active_signal == ActiveSignal_Right) ? ActiveSignal_None : ActiveSignal_Right;
}

static void hazards_button_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    active_signal = (active_signal == ActiveSignal_Hazards) ? ActiveSignal_None : ActiveSignal_Hazards;
}

void DriverDashboard_Init(void) {
    lv_obj_add_event_cb(objects.left_signal_button, left_signal_button_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.right_signal_button, right_signal_button_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.hazards_button, hazards_button_cb, LV_EVENT_CLICKED, NULL);
}

// Only touched when a button's on/off state actually flips, so the steady-state
// cost of blinking is one lv_obj_set_style_bg_opa call per side per 500ms tick,
// not one per widget per loop iteration.
static void set_button_bg(lv_obj_t *button, uint8_t on) {
    if (on) {
        lv_obj_set_style_bg_color(button, lv_color_hex(SIGNAL_COLOR_AMBER), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

#define NO_CACHED_VALUE 2
static uint8_t last_left_on = NO_CACHED_VALUE;
static uint8_t last_right_on = NO_CACHED_VALUE;
static uint8_t last_hazards_on = NO_CACHED_VALUE;

void DriverDashboard_Update(void) {
    if (HAL_GetTick() - blink_tick >= BLINK_INTERVAL_MS) {
        blink_tick = HAL_GetTick();
        blink_state ^= 1;
    }

    uint8_t left_on = (active_signal == ActiveSignal_Left || active_signal == ActiveSignal_Hazards) && blink_state;
    uint8_t right_on = (active_signal == ActiveSignal_Right || active_signal == ActiveSignal_Hazards) && blink_state;
    uint8_t hazards_on = (active_signal == ActiveSignal_Hazards) && blink_state;

    lv3c_param_signal_left = left_on;
    lv3c_param_signal_right = right_on;

    if (left_on != last_left_on) {
        last_left_on = left_on;
        set_button_bg(objects.left_signal_button, left_on);
    }
    if (right_on != last_right_on) {
        last_right_on = right_on;
        set_button_bg(objects.right_signal_button, right_on);
    }
    if (hazards_on != last_hazards_on) {
        last_hazards_on = hazards_on;
        set_button_bg(objects.hazards_button, hazards_on);
    }
}
