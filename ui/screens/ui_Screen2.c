// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SmartDoorLock

#include "../ui.h"

void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label1 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "主界面");
    lv_obj_set_style_text_font(ui_Label1, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TextArea3 = lv_textarea_create(ui_Screen2);
    lv_obj_set_width(ui_TextArea3, 150);
    lv_obj_set_height(ui_TextArea3, 70);
    lv_obj_set_x(ui_TextArea3, -35);
    lv_obj_set_y(ui_TextArea3, -71);
    lv_obj_set_align(ui_TextArea3, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextArea3, "Placeholder...");

    ui_Slider2 = lv_slider_create(ui_Screen2);
    lv_obj_set_width(ui_Slider2, 150);
    lv_obj_set_height(ui_Slider2, 10);
    lv_obj_set_x(ui_Slider2, -7);
    lv_obj_set_y(ui_Slider2, 25);
    lv_obj_set_align(ui_Slider2, LV_ALIGN_CENTER);

    ui_Switch2 = lv_switch_create(ui_Screen2);
    lv_obj_set_width(ui_Switch2, 50);
    lv_obj_set_height(ui_Switch2, 25);
    lv_obj_set_x(ui_Switch2, -5);
    lv_obj_set_y(ui_Switch2, 65);
    lv_obj_set_align(ui_Switch2, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Screen2, ui_event_Screen2, LV_EVENT_ALL, NULL);

}
