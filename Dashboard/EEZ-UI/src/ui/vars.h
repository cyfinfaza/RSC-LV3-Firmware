#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_NONE
};

// Native global variables

extern int32_t get_var_gesture_direction();
extern void set_var_gesture_direction(int32_t value);
extern int32_t get_var_display_brightness();
extern void set_var_display_brightness(int32_t value);
extern int32_t get_var_main_switch();
extern void set_var_main_switch(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/