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
extern bool get_var_flip_screen();
extern void set_var_flip_screen(bool value);
extern int32_t get_var_lv3c_sw_hv_main();
extern void set_var_lv3c_sw_hv_main(int32_t value);
extern float get_var_v_sense_5();
extern void set_var_v_sense_5(float value);
extern float get_var_v_sense_12();
extern void set_var_v_sense_12(float value);
extern int32_t get_var_lv3c_hv_pack_temp();
extern void set_var_lv3c_hv_pack_temp(int32_t value);
extern int32_t get_var_lv3c_hv_pack_max_temp();
extern void set_var_lv3c_hv_pack_max_temp(int32_t value);
extern int32_t get_var_lv3c_hv_pack_soc();
extern void set_var_lv3c_hv_pack_soc(int32_t value);
extern int32_t get_var_lv3c_hv_pack_current();
extern void set_var_lv3c_hv_pack_current(int32_t value);
extern int32_t get_var_lv3c_hv_pack_voltage();
extern void set_var_lv3c_hv_pack_voltage(int32_t value);
extern int32_t get_var_lv3c_sw_hv_motor();
extern void set_var_lv3c_sw_hv_motor(int32_t value);
extern int32_t get_var_lv3c_sw_hv_mppt();
extern void set_var_lv3c_sw_hv_mppt(int32_t value);
extern int32_t get_var_lv3c_hv_main_active();
extern void set_var_lv3c_hv_main_active(int32_t value);
extern int32_t get_var_lv3c_hv_motor_active();
extern void set_var_lv3c_hv_motor_active(int32_t value);
extern int32_t get_var_lv3c_hv_mppt_active();
extern void set_var_lv3c_hv_mppt_active(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/