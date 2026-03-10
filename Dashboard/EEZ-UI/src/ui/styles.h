#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: Basic Screen
lv_style_t *get_style_basic_screen_MAIN_DEFAULT();
void add_style_basic_screen(lv_obj_t *obj);
void remove_style_basic_screen(lv_obj_t *obj);

// Style: Button Text
lv_style_t *get_style_button_text_MAIN_DEFAULT();
void add_style_button_text(lv_obj_t *obj);
void remove_style_button_text(lv_obj_t *obj);

// Style: Base Button
lv_style_t *get_style_base_button_MAIN_DEFAULT();
lv_style_t *get_style_base_button_MAIN_PRESSED();
void add_style_base_button(lv_obj_t *obj);
void remove_style_base_button(lv_obj_t *obj);

// Style: Grid Button
lv_style_t *get_style_grid_button_MAIN_DEFAULT();
lv_style_t *get_style_grid_button_MAIN_PRESSED();
void add_style_grid_button(lv_obj_t *obj);
void remove_style_grid_button(lv_obj_t *obj);

// Style: Inline Icon
lv_style_t *get_style_inline_icon_MAIN_DEFAULT();
void add_style_inline_icon(lv_obj_t *obj);
void remove_style_inline_icon(lv_obj_t *obj);

// Style: Label in Icon Button
lv_style_t *get_style_label_in_icon_button_MAIN_DEFAULT();
void add_style_label_in_icon_button(lv_obj_t *obj);
void remove_style_label_in_icon_button(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/