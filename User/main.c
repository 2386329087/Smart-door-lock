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
SemaphoreHandle_t uart2_mutex_handler, dht11_mutex_handler, lvgl_mutex_handler;
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
              lv_timer_handler();)
            vTaskDelay(pdMS_TO_TICKS(5));
    }
}
uint8_t temp, humi;
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
                  lv_label_set_text(ui_Label7, "正在识别");)
            uint16_t r=as608_verify_fingerprint();
            if (r == 0)
            {
                mutex(lvgl_mutex_handler, 100,
                      lv_label_set_text(ui_Label7, "识别成功");
                      _ui_screen_change(ui_adminScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 500);)
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
    
    vTaskDelay(pdMS_TO_TICKS(500));
    mutex(lvgl_mutex_handler, 100,
        lv_label_set_text(ui_Label1,"录入指纹");
        lv_obj_clear_state(ui_addfingerprintButton,LV_STATE_DISABLED);
        )
    vTaskDelete(NULL);
}
void quit_timer_callback(){
    lv_scr_load_anim(ui_userScreen,LV_SCR_LOAD_ANIM_MOVE_RIGHT,500,0,false);
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
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
    lv_init();
    group = lv_group_create();
    lv_group_set_default(group);

    uart2_mutex_handler = xSemaphoreCreateMutex();
    dht11_mutex_handler = xSemaphoreCreateMutex();
    lvgl_mutex_handler = xSemaphoreCreateMutex();
    xTaskCreate(lvgl_tick_task, "lvgl_tick_task", 64, NULL, 14, &lvgl_tick_Task_Handler);
    xTaskCreate(lvgl_timer_task, "lvgl_timer_task", 1500, NULL, 5, &lvgl_timer_Task_Handler);
    xTaskCreate(dht11_task, "dht11_task", 64, NULL, 9, NULL);
    quit_timer_handler=xTimerCreate("exit_timer",pdMS_TO_TICKS(1000*30),pdFALSE,NULL,quit_timer_callback);
    vTaskStartScheduler();

    while (1)
    {
        printf("shouldn't run at here!!\n");
    }
}
