// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SmartDoorLock

#ifndef _SMARTDOORLOCK_UI_H
#define _SMARTDOORLOCK_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
void ui_event_Screen1(lv_event_t * e);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_TextArea2;
void ui_event_Keyboard2(lv_event_t * e);
extern lv_obj_t * ui_Keyboard2;
extern lv_obj_t * ui_Label2;
// SCREEN: ui_Screen2
void ui_Screen2_screen_init(void);
void ui_event_Screen2(lv_event_t * e);
extern lv_obj_t * ui_Screen2;
extern lv_obj_t * ui_Label1;
extern lv_obj_t * ui_TextArea3;
extern lv_obj_t * ui_Slider2;
extern lv_obj_t * ui_Switch2;
extern lv_obj_t * ui____initial_actions0;

LV_FONT_DECLARE(ui_font_chinese);

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
