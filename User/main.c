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
/* Global define */


/* Global Variable */
TaskHandle_t lvgl_tick_Task_Handler;
TaskHandle_t lvgl_timer_Task_Handler;

void lvgl_tick_task(void *pvParameters)
{
    while(1)
    {
        printf("task1 entry\r\n");
        vTaskDelay(250);
    }
}

void lvgl_timer_task(void *pvParameters)
{
    while(1)
    {
        //printf("task2 entry\r\n");
        vTaskDelay(500);
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
    //ui_init();
    xTaskCreate(lvgl_tick_task,"lvgl_tick_task",2000,NULL,5,&lvgl_tick_Task_Handler);
    xTaskCreate(lvgl_timer_task,"lvgl_timer_task",3600,NULL,15,&lvgl_timer_Task_Handler);
    vTaskStartScheduler();

	while(1)
	{
	    printf("shouldn't run at here!!\n");
	}
}
