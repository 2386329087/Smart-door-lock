// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SmartDoorLock

#include "ui.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "AS608.h"
#include "esp8266.h"
void (*submitPasswordPointer)();
void (*cancelPasswordInputPointer)();
extern uint8_t PS_ReadNotepad_code[32];    //存放接受到的记事本数据
void VerificationAdministrator(){
    PS_ReadNotepad(&PS_ReadNotepad_code);
    if (strcmp(lv_textarea_get_text(ui_TextArea2),PS_ReadNotepad_code)==0)
    {
        lv_label_set_text(ui_Label2,"密码正确");
        lv_scr_load_anim(ui_adminScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 500, false);
        
    }else{
        lv_label_set_text(ui_Label2,"密码错误");
    }
}
void cancelVerificationAdministrator(){
    lv_scr_load_anim(ui_userScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 400, 0, false);
}
extern uint8_t PS_WriteNotepad_code[32];   //存放要发送的记事本数据
void ChangePassword()
{
    strcpy(PS_WriteNotepad_code,lv_textarea_get_text(ui_TextArea2));
    uint8_t r=PS_WriteNotepad(&PS_WriteNotepad_code);
    if (r==0)
    {
        lv_label_set_text(ui_Label2,"修改成功");
        lv_scr_load_anim(ui_adminScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 1000, false);
    }else{
        lv_label_set_text(ui_Label2,"修改失败");
    }
    
}
void cancelChangePassword()
{
    lv_scr_load_anim(ui_adminScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 400, 0, false);
}
//提交密码
void submit_password(lv_event_t * e)
{
	submitPasswordPointer();
    lv_textarea_set_text(ui_TextArea2,"");
}
TaskHandle_t fingerprint_recognition_Task_Handler;
TaskHandle_t userScreen_Task_Handler;
extern void fingerprint_recognition_task(void *pvParameters);
extern void userScreen_task(void *pvParameters);
void userScreen_init(lv_event_t * e)
{
	lv_group_remove_all_objs(lv_group_get_default());
    lv_group_add_obj(lv_group_get_default(),ui_enterpassword);
    lv_group_add_obj(lv_group_get_default(),ui_camera);
    xTaskCreate(fingerprint_recognition_task,"fingerprint_recognition_task",200,NULL,10,&fingerprint_recognition_Task_Handler);
    xTaskCreate(userScreen_task,"userScreen_task",200,NULL,6,&userScreen_Task_Handler);
    
}
extern TaskHandle_t lvgl_tick_Task_Handler;
extern TaskHandle_t lvgl_timer_Task_Handler;
extern void camera_task(void *pvParameters);
void click_camera_open(lv_event_t * e)
{
    
	
    xTaskCreate(camera_task,"camera_task",128,NULL,13,NULL);
}


void passwordScreen_init(lv_event_t * e)
{
	lv_group_remove_all_objs(lv_group_get_default());
    lv_group_add_obj(lv_group_get_default(),ui_Keyboard2);
    lv_textarea_set_text(ui_TextArea2,"");
    
}
extern TimerHandle_t quit_timer_handler;
extern void gating_task(void *pvParameters);
void adminScreen_init(lv_event_t * e)
{
	lv_group_remove_all_objs(lv_group_get_default());

    lv_group_add_obj(lv_group_get_default(),ui_backButton);
    
    lv_group_add_obj(lv_group_get_default(),ui_emptyallfingerprintButton);
    lv_group_add_obj(lv_group_get_default(),ui_ledSwitch);
    
    lv_group_add_obj(lv_group_get_default(),ui_fanSwitch);
    
    lv_group_add_obj(lv_group_get_default(),ui_addfingerprintButton);
    lv_group_add_obj(lv_group_get_default(),ui_ChangePasswordButton);
    xTaskCreate(gating_task,"gating_task",200,NULL,9,NULL);
    xTimerReset(quit_timer_handler,pdMS_TO_TICKS(100));
    
}
extern void add_fingerprint_task(void *pvParameters);
void add_fingerprint(lv_event_t * e)
{
    xTimerReset(quit_timer_handler,pdMS_TO_TICKS(100));
	xTaskCreate(add_fingerprint_task,"add_fingerprint_task",200,NULL,13,NULL);
}

void userScreen_Deinit(lv_event_t * e)
{
    vTaskDelete(fingerprint_recognition_Task_Handler);
    vTaskDelete(userScreen_Task_Handler);
}

void empty_all_fingerprint(lv_event_t * e)
{
    xTimerReset(quit_timer_handler,pdMS_TO_TICKS(100));
	as608_empty_all_fingerprint();
}


//取消输入密码
void CancelPasswordInput(lv_event_t * e)
{
	cancelPasswordInputPointer();
}

void userScreen_inputPassword_button(lv_event_t * e)
{
	submitPasswordPointer=VerificationAdministrator;
    cancelPasswordInputPointer=cancelVerificationAdministrator;
    lv_label_set_text(ui_Label2,"登入");
}

void ChangePasswordButton(lv_event_t * e)
{
	submitPasswordPointer=ChangePassword;
    cancelPasswordInputPointer=cancelChangePassword;
    xTimerReset(quit_timer_handler,pdMS_TO_TICKS(100));
    lv_label_set_text(ui_Label2,"修改密码");
}

void backButton(lv_event_t * e)
{
	xTimerStop(quit_timer_handler,pdMS_TO_TICKS(100));
    
}
extern void esp8266_control_task(void *pvParameters);
void led_enable(lv_event_t * e)
{
	xTaskCreate(esp8266_control_task,"esp8266_control_task",256,"LED_ON",12,NULL);
}

void led_disable(lv_event_t * e)
{
	xTaskCreate(esp8266_control_task,"esp8266_control_task",256,"LED_OFF",12,NULL);
}

void fan_enable(lv_event_t * e)
{
	xTaskCreate(esp8266_control_task,"esp8266_control_task",256,"FAN_ON",12,NULL);
}

void fan_disable(lv_event_t * e)
{
	xTaskCreate(esp8266_control_task,"esp8266_control_task",256,"FAN_OFF",12,NULL);
}
