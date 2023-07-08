#include "timer.h"

void Delay_test(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

       /* PA1 -->TIM2_CH2  */
       GPIO_InitTypeDef GPIO_InitStructure;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_Init(GPIOB, &GPIO_InitStructure);

       GPIO_WriteBit(GPIOB, GPIO_Pin_15, SET);
}

void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
    TIM_TimeBaseInitStructure.TIM_Period    = 50000-1;  //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 144-1;     //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
}

//微秒级延时
void TIM3_Delayus(u16 xus)
{
    TIM_Cmd(TIM3,ENABLE); //启动定时器
    while(TIM3->CNT < xus);
    TIM3->CNT = 0;
    TIM_Cmd(TIM3,DISABLE); //关闭定时器
}

//毫秒级延时
void TIM3_Delayms(u16 xms)
{
    int i;
    for(i=0;i<xms;i++)
    {
        TIM3_Delayus(1000);
    }
}
