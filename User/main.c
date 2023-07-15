/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 task1 and task2 alternate printing
*/

#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "semphr.h"
#include "ui.h"
#include "DHT11.h"
#include "AS608.h"
#include "PWM.h"
#include "lcd_st7789.h"
#include "AP3216C.h"
#include "queue.h"
#include "esp8266.h"
/* Global define */

#define mutex(mutex_handler, wait_ms, code)                              \
    if (xSemaphoreTake(mutex_handler, pdMS_TO_TICKS(wait_ms)) == pdPASS) \
    {                                                                    \
        code                                                             \
            xSemaphoreGive(mutex_handler);                               \
    }
/* Global Variable */
lv_group_t *group;
TaskHandle_t lvgl_tick_Task_Handler;
TaskHandle_t lvgl_timer_Task_Handler;
TimerHandle_t quit_timer_handler;
SemaphoreHandle_t uart2_mutex_handler, dht11_mutex_handler, lvgl_mutex_handler,timer_mutex_handler;
QueueHandle_t ap3216c_queue_handler;
char date[20],week[20];
//温度，湿度
uint8_t temp, humi;
void wake_up_task(void *pvParameters){
    
    uint16_t dis;
    uint8_t r;
    while (1)
    {
        r=0;
        dis=0;
        xQueueReceive(ap3216c_queue_handler,&dis,pdMS_TO_TICKS(10));
        xQueueReceive(ap3216c_queue_handler,&dis,pdMS_TO_TICKS(200));
        if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0)r=2;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)==0)r= 1;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)==0)r= 6;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)r= 7;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==0)r= 4;
        else if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==0)r= 3;
        else if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)==0)r= 5;
        if (dis>20||r!=0)
        {
            LCD_ExitSleep();
            lv_disp_trig_activity(NULL);
            lv_obj_invalidate(lv_scr_act());//使其无效以重绘画面
            // vTaskResume(lvgl_tick_Task_Handler);
            vTaskResume(lvgl_timer_Task_Handler);
            vTaskDelete(NULL);
        }
    }
    
}
void ap3216c_task(void *pvParameters){
    vTaskDelay(pdMS_TO_TICKS(2000));
    AP3216C_Init();
    //红外，距离，光敏
    uint16_t infrared,distance,photosensitive;
    while (1)
    {
        
        AP3216C_ReadData(&infrared,&distance,&photosensitive);
        xQueueSend(ap3216c_queue_handler,&distance,0);
        // printf("dis:%d\n",distance);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}
void lvgl_tick_task(void *pvParameters)
{
    while (1)
    {
        lv_tick_inc(1);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
void lvgl_timer_task(void *pvParameters)
{
    lv_port_disp_init();
    lv_port_indev_init();
    ov_display();
    ov_display_disable();
    ui_init();

    while (1)
    {
        mutex(lvgl_mutex_handler, 100,
            if(lv_disp_get_inactive_time(NULL)<20000){
                lv_timer_handler();
            }else{
                // vTaskSuspend(lvgl_tick_Task_Handler);
                LCD_EnterSleep();
                xTaskCreate(wake_up_task,"wake_up_task",128,NULL,8,NULL);
                vTaskSuspend(NULL);
            }
            )
            vTaskDelay(pdMS_TO_TICKS(5));
    }
}
void dht11_task(void *pvParameters)
{
    
    DHT11_Init();
    while (1)
    {
        mutex(dht11_mutex_handler, 100,
              DHT11_Read_Data(&temp, &humi);)
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
void userScreen_task(void *pvParameters){
    while (1)
    {
        mutex(dht11_mutex_handler, 100,
        mutex(lvgl_mutex_handler,100,
        lv_label_set_text_fmt(ui_temp,"温度:%d°C",temp);
        lv_label_set_text_fmt(ui_humi,"湿度:%d%%",humi);
        ))
        vTaskDelay(2000);
    }
    
}
void fingerprint_recognition_task(void *pvParameters)
{

    while (1)
    {
        if (as608_detection_finger(10) == 0)
        {
            mutex(lvgl_mutex_handler, 100,
                lv_disp_trig_activity(NULL);
                lv_label_set_text(ui_Label7, "正在识别");)
            uint16_t r=as608_verify_fingerprint();
            if (r == 0)
            {
                mutex(lvgl_mutex_handler, 100,
                      lv_label_set_text(ui_Label7, "识别成功");
                      lv_scr_load_anim(ui_adminScreen,LV_SCR_LOAD_ANIM_MOVE_LEFT,500,0,false);
                      )
                      vTaskDelay(pdMS_TO_TICKS(1000));

            }
            else
            {
                mutex(lvgl_mutex_handler, 100,
                      lv_label_set_text(ui_Label7, "识别失败");)
                    vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
        else
        {
            mutex(lvgl_mutex_handler, 1000,
                  lv_label_set_text(ui_Label7, "按入指纹以继续");)
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
void addfingerprinting(uint8_t i){
    if (i==1)
    {
        mutex(lvgl_mutex_handler, 100,
        lv_label_set_text(ui_Label1,"请放入手指");
        )
        
    }
    else if (i==2)
    {
        mutex(lvgl_mutex_handler, 100,
        lv_label_set_text(ui_Label1,"请再放入一次");
        )
    }
    
}
void add_fingerprint_task(void *pvParameters){

    mutex(lvgl_mutex_handler, 100,
        lv_obj_add_state(ui_addfingerprintButton,LV_STATE_DISABLED);
        lv_obj_clear_flag(ui_addfingerprinting,LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui_Label1,"正在识别");
        )
    uint8_t r=as608_add_fingerprint(as608_find_fingerprints_num()+1,addfingerprinting);
    mutex(lvgl_mutex_handler, 100,
        lv_obj_add_flag(ui_addfingerprinting,LV_OBJ_FLAG_HIDDEN);
        )
    if (r==0)
    {
        mutex(lvgl_mutex_handler, 100,
        lv_label_set_text(ui_Label1,"√");
        )
    }else{
        mutex(lvgl_mutex_handler, 100,
        lv_label_set_text(ui_Label1,"×");
        )
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    mutex(lvgl_mutex_handler, 100,
        lv_label_set_text(ui_Label1,"录入指纹");
        lv_obj_clear_state(ui_addfingerprintButton,LV_STATE_DISABLED);
        )
    vTaskDelete(NULL);
}
void quit_timer_callback(){
    lv_scr_load_anim(ui_userScreen,LV_SCR_LOAD_ANIM_MOVE_RIGHT,500,0,false);
}
void gating_task(void *pvParameters){
    Servo_SetAngle(0);
    printf("开门\n");
    vTaskDelay(pdMS_TO_TICKS(1000*5));
    Servo_SetAngle(180);
    printf("关门\n");
    vTaskDelete(NULL);
}
void camera_task(void *pvParameters){
    mutex(lvgl_mutex_handler,100,
    lv_obj_add_state(ui_camera,LV_STATE_DISABLED);
    vTaskSuspend(lvgl_timer_Task_Handler);
    vTaskSuspend(lvgl_tick_Task_Handler);   
    )
    ov_display_enable();
    AP3216C_Init(); 
    uint8_t r=0;
    while (1)
    {
        r=0;
        if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0)r=2;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)==0)r= 1;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)==0)r= 6;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)r= 7;
        else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==0)r= 4;
        else if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==0)r= 3;
        else if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)==0)r= 5;
        if (r!=0)
        {
            ov_display_disable();
            vTaskResume(lvgl_tick_Task_Handler);
            lv_obj_invalidate(lv_scr_act());
            vTaskResume(lvgl_timer_Task_Handler);
            vTaskDelay(500);
            mutex(lvgl_mutex_handler,100,
                lv_obj_clear_state(ui_camera,LV_STATE_DISABLED); 
            )
            vTaskDelete(NULL);
        }
        vTaskDelay(100);    
    }   
}
extern TDateTime TIME;
//时钟线程
void time_task(void *pvParameters){
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp8266_Init("404","");
    get_true_time(date,week);
    printf("date:%s week:%s\n",date,week);
    while (1)
    {
        system_time_increase();
        mutex(lvgl_mutex_handler,100,
        lv_label_set_text_fmt(ui_timeLabel,"%02d:%02d:%02d",TIME.hour,TIME.minute,TIME.second);
        )
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
//显示信息线程
void show_info_task(void *pvParameters){
    vTaskDelay(pdMS_TO_TICKS(3000));
    // char info[400];
    while (1)
    {
        // vTaskList(info);
        mutex(lvgl_mutex_handler,100,
        // lv_label_set_text(ui_infoLabel,info);
        lv_label_set_text_fmt(ui_infoLabel,"size:%d",xPortGetMinimumEverFreeHeapSize());
        
        )
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    
}
int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("FreeRTOS Kernel Version:%s\r\n", tskKERNEL_VERSION_NUMBER);
    
    AS608_PIN_Init();
    
    Servo_Init();
    lv_init();
    group = lv_group_create();
    lv_group_set_default(group);

    uart2_mutex_handler = xSemaphoreCreateMutex();
    dht11_mutex_handler = xSemaphoreCreateMutex();
    lvgl_mutex_handler = xSemaphoreCreateMutex();
    ap3216c_queue_handler= xQueueCreate(1,sizeof(uint16_t));
    timer_mutex_handler=xSemaphoreCreateMutex();
    xTaskCreate(lvgl_tick_task, "lvgl_tick_task", 64, NULL, 14, &lvgl_tick_Task_Handler);
    xTaskCreate(lvgl_timer_task, "lvgl_timer_task", 1000, NULL, 1, &lvgl_timer_Task_Handler);
    xTaskCreate(dht11_task, "dht11_task", 128, NULL, 6, NULL);
    xTaskCreate(ap3216c_task,"ap3216c_task",128,NULL,6,NULL);
    xTaskCreate(time_task,"time_task",500,NULL,10,NULL);
    xTaskCreate(show_info_task,"show_info_task",600,NULL,10,NULL);
    quit_timer_handler=xTimerCreate("exit_timer",pdMS_TO_TICKS(1000*60),pdFALSE,NULL,quit_timer_callback);
    vTaskStartScheduler();

    while (1)
    {
        printf("shouldn't run at here!!\n");
    }
}
