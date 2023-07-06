// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SmartDoorLock

#include "../ui.h"

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_TextArea2 = lv_textarea_create(ui_Screen1);
    lv_obj_set_height(ui_TextArea2, 36);
    lv_obj_set_width(ui_TextArea2, lv_pct(100));
    lv_obj_set_align(ui_TextArea2, LV_ALIGN_TOP_MID);
    lv_textarea_set_placeholder_text(ui_TextArea2, "Placeholder...");

    ui_Keyboard2 = lv_keyboard_create(ui_Screen1);
    lv_keyboard_set_mode(ui_Keyboard2, LV_KEYBOARD_MODE_NUMBER);
    lv_obj_set_height(ui_Keyboard2, 170);
    lv_obj_set_width(ui_Keyboard2, lv_pct(100));
    lv_obj_set_align(ui_Keyboard2, LV_ALIGN_BOTTOM_MID);

    ui_Label2 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label2, -8);
    lv_obj_set_y(ui_Label2, 45);
    lv_obj_set_align(ui_Label2, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label2, "请输入密码");
    lv_obj_set_style_text_font(ui_Label2, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_keyboard_set_textarea(ui_Keyboard2, ui_TextArea2);
    lv_obj_add_event_cb(ui_Keyboard2, ui_event_Keyboard2, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen1, ui_event_Screen1, LV_EVENT_ALL, NULL);

}
