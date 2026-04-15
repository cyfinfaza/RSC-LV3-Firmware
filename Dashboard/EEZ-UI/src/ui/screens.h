#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SETTINGS = 2,
    SCREEN_ID_CAN_PARAMETERS = 3,
    SCREEN_ID_DRIVER_DASHBOARD = 4,
    SCREEN_ID_HV_BATTERY = 5,
    _SCREEN_ID_LAST = 5
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *settings;
    lv_obj_t *can_parameters;
    lv_obj_t *driver_dashboard;
    lv_obj_t *hv_battery;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *brightness_slider;
    lv_obj_t *obj12;
    lv_obj_t *default_screen_dropdown;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *can_parameters_table;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *soc;
    lv_obj_t *lv3c_hv_pack_soc_bar;
    lv_obj_t *lv3c_sw_hv_main;
    lv_obj_t *hv_main_active;
    lv_obj_t *lv3c_sw_hv_motor;
    lv_obj_t *hv_motor_active;
    lv_obj_t *lv3c_sw_hv_mppt;
    lv_obj_t *hv_mppt_active;
    lv_obj_t *hv_pack_voltage_current;
    lv_obj_t *hv_pack_temp;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *cell_voltages_1_12;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *cell_voltages_13_24;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *obj46;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_settings();
void tick_screen_settings();

void create_screen_can_parameters();
void tick_screen_can_parameters();

void create_screen_driver_dashboard();
void tick_screen_driver_dashboard();

void create_screen_hv_battery();
void tick_screen_hv_battery();

void create_user_widget_widget1(lv_obj_t *parent_obj, void *flowState, int startWidgetIndex);
void tick_user_widget_widget1(void *flowState, int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Color themes

enum Themes {
    THEME_ID_DEFAULT,
};
enum Colors {
    COLOR_ID_BG,
    COLOR_ID_TEXT,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[1][2];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/