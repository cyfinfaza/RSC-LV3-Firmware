#include "screen_helper.h"
#include "can_parameters.h"
#include "hv_battery.h"
#include "screens.h"
#include "lvgl.h"

void ScreenHelper_Init(void) {
    CanParameters_Init();
    HVBattery_Init();
}

void ScreenHelper_Loop(void) {
    lv_obj_t *active = lv_scr_act();

    if (active == objects.can_parameters) {
        CanParameters_Update();
    } else if (active == objects.hv_battery) {
        HVBattery_Update();
    }
    // main, settings, and driver_dashboard have no periodic updater yet
}
