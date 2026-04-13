#pragma once

// Display, backlight, and touch input module.
// Also owns the EEZ Studio variable bindings and actions related to display
// settings and gesture input.

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

// Physical display resolution and RGB565 framebuffer size
#define DISP_WIDTH 480
#define DISP_HEIGHT 272
#define FRAME_SIZE (DISP_WIDTH * DISP_HEIGHT * 2)

// Current backlight level (0-100%). Written by EEZ and read by Display_BacklightUpdate.
extern unsigned int backlight_level;
// Whether the display is rotated 180 degrees. Applied on boot and when toggled via EEZ.
extern bool flip_screen_state;

// Initialize LVGL, the LTDC framebuffer, and the FT5206 touch input.
// Must be called after MX peripheral init and after PersistentSettings_Init
// (so that backlight_level and flip_screen_state are already loaded).
void Display_Init(void);

// Update the backlight PWM. Call once per main loop tick.
// Performs a linear fade-in from 0 to backlight_level over SCREEN_FADE_ON_TIME ms
// on first run, then holds the steady-state value.
void Display_BacklightUpdate(void);

// EEZ Studio variable bindings — called by the EEZ flow runtime to get/set UI variables
int32_t get_var_display_brightness(void);
void set_var_display_brightness(int32_t value);
bool get_var_flip_screen(void);
void set_var_flip_screen(bool value); // also immediately applies rotation to LVGL
int32_t get_var_gesture_direction(void);
void set_var_gesture_direction(int32_t value); // read-only from EEZ; setter is a no-op

// EEZ Studio action — attached to a gesture event on a screen object to capture
// swipe direction into the gesture_direction variable
void action_accept_gesture(lv_event_t *e);
