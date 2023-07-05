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
#include "semphr.h"
#include "ui.h" 
#include "keyboard.h"
/* Global define */


/* Global Variable */
TaskHandle_t lvgl_tick_Task_Handler;
TaskHandle_t lvgl_timer_Task_Handler;
TaskHandle_t Task1_Handler;
SemaphoreHandle_t uart2_mutex_handler;
void lvgl_tick_task(void *pvParameters)
{
    while(1)
    {
        // if(xSemaphoreTake(uart2_mutex_handler,pdMS_TO_TICKS(100))==pdPASS){
        //     printf("task1 entry\r\n");
        //     xSemaphoreGive(uart2_mutex_handler);
        // }
        lv_tick_inc(2);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

void lvgl_timer_task(void *pvParameters)
{
    while(1)
    {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
void task1(void *pvParameters)
{
    
    uint8_t r=1;
    while(1)
    {
        r= keyboard_uint_scan();
        if(xSemaphoreTake(uart2_mutex_handler,pdMS_TO_TICKS(100))==pdPASS){
            printf("key:%d\n",r);   
            xSemaphoreGive(uart2_mutex_handler);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
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
    lv_port_disp_init();
    ui_init();
    keyboard_Pin_Init();
    uart2_mutex_handler= xSemaphoreCreateMutex();
    xTaskCreate(lvgl_tick_task,"lvgl_tick_task",512,NULL,14,&lvgl_tick_Task_Handler);
    xTaskCreate(lvgl_timer_task,"lvgl_timer_task",2100,NULL,5,&lvgl_timer_Task_Handler);
    xTaskCreate(task1,"task1",256,NULL,5,&Task1_Handler);
    vTaskStartScheduler();

	while(1)
	{
	    printf("shouldn't run at here!!\n");
	}
}
