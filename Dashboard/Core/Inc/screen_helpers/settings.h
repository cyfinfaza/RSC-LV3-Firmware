#pragma once

// Screen helper for the Settings screen.
// Populates the default-screen dropdown, fixes the indicator font, restores
// the saved selection, and navigates to the saved default screen on boot.

// Initialize settings screen UI. Must be called after ui_init() (all screen
// objects exist) but before the first LVGL tick.
void Settings_Init(void);

// EEZ Studio action — saves brightness, flip, and default screen to Flash,
// then syncs the display. Wire this to the "Save settings to flash" button
// in EEZ flow (replaces action_save_brightness_to_eeprom for that button).
void action_save_settings(void);

// EEZ Studio action — triggers a system reset via NVIC_SystemReset().
void action_reboot(void);
