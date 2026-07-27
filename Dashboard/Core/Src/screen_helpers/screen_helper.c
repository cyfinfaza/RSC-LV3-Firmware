#include "screen_helper.h"
#include "can_parameters.h"
#include "hv_battery.h"
#include "lv_battery.h"
#include "settings.h"
#include "driver_dashboard.h"
#include "screens.h"
#include "lvgl.h"

void ScreenHelper_Init(void) {
    CanParameters_Init();
    HVBattery_Init();
    LVBattery_Init();
    Settings_Init();  // populates dropdown, fixes font, and navigates to default screen
    DriverDashboard_Init();
}

void ScreenHelper_Loop(void) {
    lv_obj_t *active = lv_scr_act();

    if (active == objects.can_parameters) {
        CanParameters_Update();
    } else if (active == objects.hv_battery) {
        HVBattery_Update();
    } else if (active == objects.lv_battery) {
        LVBattery_Update();
    } else if (active == objects.driver_dashboard) {
        DriverDashboard_Update();
    }
    // main and settings have no periodic updater yet
}
