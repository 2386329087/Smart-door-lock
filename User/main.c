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
/* Global define */

#define mutex(mutex_handler,wait_ms,code) if(xSemaphoreTake(mutex_handler,pdMS_TO_TICKS(wait_ms))==pdPASS){\
            code   \
            xSemaphoreGive(mutex_handler);\
        }  
/* Global Variable */
lv_group_t *group;
TaskHandle_t lvgl_tick_Task_Handler;
TaskHandle_t lvgl_timer_Task_Handler;
TaskHandle_t Task1_Handler;
SemaphoreHandle_t uart2_mutex_handler,dht11_mutex_handler;
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
    ui_init();
    
    while(1)
    {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
uint8_t temp,humi;
void task1(void *pvParameters)
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
    
    lv_init();
    
    group=lv_group_create();
    lv_group_set_default(group);
    
    

    uart2_mutex_handler= xSemaphoreCreateMutex();
    dht11_mutex_handler= xSemaphoreCreateMutex();
    xTaskCreate(lvgl_tick_task,"lvgl_tick_task",64,NULL,14,&lvgl_tick_Task_Handler);
    xTaskCreate(lvgl_timer_task,"lvgl_timer_task",1000,NULL,5,&lvgl_timer_Task_Handler);
    xTaskCreate(task1,"task1",128,NULL,5,&Task1_Handler);
    vTaskStartScheduler();

	while(1)
	{
	    printf("shouldn't run at here!!\n");
	}
}
