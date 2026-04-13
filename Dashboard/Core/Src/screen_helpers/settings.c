#include "settings.h"
#include "persistent_settings.h"
#include "screens.h"
#include "eez-flow.h"
#include "lvgl.h"
#include "main.h"
#include <src/widgets/dropdown/lv_dropdown.h>

// ---------------------------------------------------------------------------
// Default screen menu configuration
// To reorder or add/remove entries: edit the SCREEN_MENU_ENTRIES list below.
// Each entry is SCREEN_ENTRY(label, screen_id) where screen_id is a value
// from the ScreensEnum in screens.h.
// ---------------------------------------------------------------------------
#define SCREEN_MENU_ENTRIES \
    SCREEN_ENTRY("Home",             SCREEN_ID_MAIN)             \
    SCREEN_ENTRY("Driver Dashboard", SCREEN_ID_DRIVER_DASHBOARD) \
    SCREEN_ENTRY("HV Battery",       SCREEN_ID_HV_BATTERY)       \
    SCREEN_ENTRY("CAN Parameters",   SCREEN_ID_CAN_PARAMETERS)   \
    SCREEN_ENTRY("Settings",         SCREEN_ID_SETTINGS)

// Build the newline-separated options string expected by lv_dropdown_set_options
#define SCREEN_ENTRY(label, id) label "\n"
static const char dropdown_options[] =
    SCREEN_MENU_ENTRIES
    ; // trailing \n is harmless — LVGL ignores an empty final entry
#undef SCREEN_ENTRY

// Build the parallel array mapping dropdown index → ScreensEnum value
#define SCREEN_ENTRY(label, id) id,
static const int16_t screen_ids[] = { SCREEN_MENU_ENTRIES };
#undef SCREEN_ENTRY

#define NUM_SCREEN_OPTIONS (sizeof(screen_ids) / sizeof(screen_ids[0]))

void Settings_Init(void) {
    lv_dropdown_set_options(objects.default_screen_dropdown, dropdown_options);

    // The dropdown indicator (arrow) is rendered as a symbol glyph. Without an
    // explicit font that includes the glyph it will render blank on this build;
    // Montserrat 14 is the only Montserrat size enabled in lv_conf.h.
    lv_obj_set_style_text_font(objects.default_screen_dropdown,
        &lv_font_montserrat_14, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Restore saved dropdown index (loaded from EEPROM by PersistentSettings_Init)
    if (default_screen_id < NUM_SCREEN_OPTIONS) {
        lv_dropdown_set_selected(objects.default_screen_dropdown, default_screen_id);
    }

    // Navigate to the saved default screen using the explicit id from the table
    eez_flow_set_screen(screen_ids[default_screen_id], LV_SCR_LOAD_ANIM_NONE, 0, 0);
}

void action_reboot(void) {
    NVIC_SystemReset();
}

void action_save_settings(void) {
    // Commit dropdown selection to the shared variable before saving
    default_screen_id = (uint16_t)lv_dropdown_get_selected(objects.default_screen_dropdown);
    PersistentSettings_Save();
}
