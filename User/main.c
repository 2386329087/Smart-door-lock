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
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "semphr.h"
#include "ui.h" 
#include "DHT11.h"
#include "AS608.h"
/* Global define */

#define mutex(mutex_handler,wait_ms,code) if(xSemaphoreTake(mutex_handler,pdMS_TO_TICKS(wait_ms))==pdPASS){\
            code   \
            xSemaphoreGive(mutex_handler);\
        }  
/* Global Variable */
lv_group_t *group;
TaskHandle_t lvgl_tick_Task_Handler;
TaskHandle_t lvgl_timer_Task_Handler;
TaskHandle_t userScreen_Task_Handler;
SemaphoreHandle_t uart2_mutex_handler,dht11_mutex_handler,lvgl_mutex_handler;
void lvgl_tick_task(void *pvParameters)
{
    while(1)
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
    
    
    while(1)
    {
        mutex(lvgl_mutex_handler,100,
            lv_timer_handler();
        )
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
uint8_t temp,humi;
void dht11_task(void *pvParameters)
{
    DHT11_Init(); 
    while(1)
    {
        mutex(dht11_mutex_handler,100,
            DHT11_Read_Data(&temp,&humi);
        )
        
        mutex(uart2_mutex_handler,100,
            printf("t:%d h:%d\n",temp,humi);  
        )
             
            
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
void userScreen_task(void *pvParameters)
{

    vTaskDelay(pdMS_TO_TICKS(3000));
    while(1)
    { 
        if(as608_detection_finger(10)==0){
            mutex(lvgl_mutex_handler,100,
            lv_label_set_text(ui_Label7,"正在识别");
            )
            if(as608_verify_fingerprint()!=0){
                mutex(lvgl_mutex_handler,100,
                lv_label_set_text(ui_Label7,"识别成功");
                _ui_screen_change(ui_adminScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 500);
                )
            }else{
                mutex(lvgl_mutex_handler,100,
                lv_label_set_text(ui_Label7,"识别失败");
                )
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }else{
            mutex(lvgl_mutex_handler,1000,
            lv_label_set_text(ui_Label7,"按入指纹以继续");
            )
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
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
		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);
    AS608_PIN_Init();
    lv_init();
    group=lv_group_create();
    lv_group_set_default(group);

    uart2_mutex_handler= xSemaphoreCreateMutex();
    dht11_mutex_handler= xSemaphoreCreateMutex();
    lvgl_mutex_handler= xSemaphoreCreateMutex();
    xTaskCreate(lvgl_tick_task,"lvgl_tick_task",64,NULL,14,&lvgl_tick_Task_Handler);
    xTaskCreate(lvgl_timer_task,"lvgl_timer_task",1000,NULL,5,&lvgl_timer_Task_Handler);
    xTaskCreate(dht11_task,"dht11_task",128,NULL,5,NULL);
    xTaskCreate(userScreen_task,"userScreen_task",500,NULL,6,&userScreen_Task_Handler);
    vTaskStartScheduler();

	while(1)
	{
	    printf("shouldn't run at here!!\n");
	}
}
