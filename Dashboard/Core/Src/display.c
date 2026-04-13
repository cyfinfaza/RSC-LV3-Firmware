#include "display.h"
#include "main.h"
#include "FT5206.h"
#include <string.h>

// Peripheral handles are defined by CubeMX in main.c
extern LTDC_HandleTypeDef hltdc;
extern TIM_HandleTypeDef htim1; // TIM1 CH1 drives the backlight PWM

// Duration of the backlight fade-in on startup (ms)
#define SCREEN_FADE_ON_TIME 500

// Framebuffers placed in the fast D1 RAM region for LTDC DMA access.
// ltdc_framebuffer is the active display buffer written directly by LVGL.
// The two draw buffers are used by LVGL's partial rendering pipeline.
uint8_t ltdc_framebuffer[FRAME_SIZE] __attribute__((section(".ram_d1")));
uint8_t lvgl_draw_buffer_1[FRAME_SIZE / 2] __attribute__((section(".ram_d1")));
uint8_t lvgl_draw_buffer_2[FRAME_SIZE / 2] __attribute__((section(".ram_d1")));

unsigned int backlight_level = 25;
bool flip_screen_state = false;

// Timestamp recorded at the end of Display_Init; used to drive the fade-in ramp
static unsigned int fade_start_timestamp = 0;

// Backing variable for the EEZ gesture_direction variable
static int last_swipe_dir = -1;

void Display_Init(void) {
    memset(ltdc_framebuffer, 0, FRAME_SIZE);
    HAL_LTDC_SetAddress(&hltdc, (uint32_t)ltdc_framebuffer, 0);

    // Start PWM at 0% — brightness will be ramped up by Display_BacklightUpdate
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    lv_init();
    lv_tick_set_cb(HAL_GetTick);

    // Partial rendering mode: LVGL renders into the draw buffers and blits to
    // ltdc_framebuffer via DMA2D, avoiding a full double-buffer swap each frame
    lv_disp_t *disp = lv_st_ltdc_create_partial(
        lvgl_draw_buffer_1, lvgl_draw_buffer_2, FRAME_SIZE / 2, 0);
    if (flip_screen_state) {
        lv_disp_set_rotation(disp, LV_DISP_ROTATION_180);
    } else {
        lv_disp_set_rotation(disp, LV_DISP_ROTATION_0);
    }

    // FT5206 capacitive touch controller on I2C4; registered as an LVGL pointer device
    FT5206_Init();
    lv_indev_t *indev_touch = lv_indev_create();
    lv_indev_set_type(indev_touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touch, FT5206_Read);

    fade_start_timestamp = HAL_GetTick();
}

void Display_BacklightUpdate(void) {
    if (HAL_GetTick() - fade_start_timestamp < SCREEN_FADE_ON_TIME) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
            backlight_level * (HAL_GetTick() - fade_start_timestamp) / SCREEN_FADE_ON_TIME);
    } else {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, backlight_level);
    }
}

// EEZ Studio variable bindings for display_brightness
int32_t get_var_display_brightness(void) {
    return backlight_level;
}

void set_var_display_brightness(int32_t value) {
    backlight_level = value;
}

// EEZ Studio variable bindings for flip_screen
bool get_var_flip_screen(void) {
    return flip_screen_state;
}

void set_var_flip_screen(bool value) {
    flip_screen_state = value;
    if (flip_screen_state) {
        lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROTATION_180);
    } else {
        lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROTATION_0);
    }
}

// EEZ Studio variable bindings for gesture_direction
int32_t get_var_gesture_direction(void) {
    return last_swipe_dir;
}

void set_var_gesture_direction(int32_t value) { (void)value; }

// EEZ Studio action — attach to an lv_event gesture callback on a screen object.
// Captures the swipe direction so EEZ flow can read it via gesture_direction.
void action_accept_gesture(lv_event_t *e) {
    lv_indev_t *indev = lv_event_get_indev(e);
    last_swipe_dir = (int32_t)lv_indev_get_gesture_dir(indev);
}
