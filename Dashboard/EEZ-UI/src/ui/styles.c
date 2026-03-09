#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: Basic Screen
//

void init_style_basic_screen_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_text_font(style, &ui_font_barlow_16_medium);
};

lv_style_t *get_style_basic_screen_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_basic_screen_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_basic_screen(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_basic_screen_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_basic_screen(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_basic_screen_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button Text
//

void init_style_button_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_barlow_16_semibold);
};

lv_style_t *get_style_button_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_basic_screen,
        add_style_button_text,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_basic_screen,
        remove_style_button_text,
    };
    remove_style_funcs[styleIndex](obj);
}