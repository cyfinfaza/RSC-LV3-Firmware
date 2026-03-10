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
// Style: Base Button
//

void init_style_base_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][1]));
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][1]));
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_width(style, 2);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_base_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_base_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_base_button_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_transform_width(style, 0);
    lv_style_set_transform_height(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][1]));
    lv_style_set_bg_opa(style, 32);
};

lv_style_t *get_style_base_button_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_base_button_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_base_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_base_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_base_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_base_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_base_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_base_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Grid Button
//

void init_style_grid_button_MAIN_DEFAULT(lv_style_t *style) {
    init_style_base_button_MAIN_DEFAULT(style);
    
    lv_style_set_grid_cell_x_align(style, LV_GRID_ALIGN_STRETCH);
    lv_style_set_grid_cell_y_align(style, LV_GRID_ALIGN_STRETCH);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
};

lv_style_t *get_style_grid_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_grid_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_grid_button_MAIN_PRESSED(lv_style_t *style) {
    init_style_base_button_MAIN_PRESSED(style);
    
};

lv_style_t *get_style_grid_button_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_grid_button_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_grid_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_grid_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_grid_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_grid_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_grid_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_grid_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Inline Icon
//

void init_style_inline_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_image_recolor(style, lv_color_hex(theme_colors[eez_flow_get_selected_theme_index()][1]));
    lv_style_set_image_recolor_opa(style, 255);
};

lv_style_t *get_style_inline_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_inline_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_inline_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_inline_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_inline_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_inline_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Label in Icon Button
//

void init_style_label_in_icon_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_flex_grow(style, 1);
};

lv_style_t *get_style_label_in_icon_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_in_icon_button_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_in_icon_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_in_icon_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_in_icon_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_in_icon_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_basic_screen,
        add_style_button_text,
        add_style_base_button,
        add_style_grid_button,
        add_style_inline_icon,
        add_style_label_in_icon_button,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_basic_screen,
        remove_style_button_text,
        remove_style_base_button,
        remove_style_grid_button,
        remove_style_inline_icon,
        remove_style_label_in_icon_button,
    };
    remove_style_funcs[styleIndex](obj);
}