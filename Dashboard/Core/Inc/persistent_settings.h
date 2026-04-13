#pragma once

#include <stdint.h>

// Persistent storage of user-configurable settings via Flash EEPROM emulation.
// Uses STM32 EEPROM emulation library (eeprom.h) which maps virtual addresses
// to two Flash sectors and handles wear leveling transparently.

// Index of the default screen to load on startup (0 = Main, matches dropdown order).
// Written by Settings_Init from EEPROM; read by settings.c to set the dropdown and
// navigate on boot.
extern uint16_t default_screen_id;

// Load saved settings from Flash into the display module's state variables.
// Must be called before Display_Init so that backlight_level, flip_screen_state,
// and default_screen_id are populated before LVGL is initialized.
void PersistentSettings_Init(void);

// Write all persistent settings (brightness, flip, default screen) to Flash.
// Called by action_save_settings in the settings screen helper.
void PersistentSettings_Save(void);