#pragma once

// Persistent storage of user-configurable settings via Flash EEPROM emulation.
// Uses STM32 EEPROM emulation library (eeprom.h) which maps virtual addresses
// to two Flash sectors and handles wear leveling transparently.

// Load saved settings from Flash into the display module's state variables.
// Must be called before Display_Init so that backlight_level and flip_screen_state
// are set correctly before LVGL is initialized.
void PersistentSettings_Init(void);

// EEZ Studio action — saves current backlight level and screen flip state to Flash.
// Intended to be triggered from a UI save button or settings confirmation.
void action_save_brightness_to_eeprom(void);
