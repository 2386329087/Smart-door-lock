#ifndef __TIMER_H
#define __TIMER_H

#include "debug.h"

extern uint32_t Timer_Delay_CNT;

void TIM3_Init(void);
void TIM3_Delayus(u16 xus);
void Delay_test(void);

#endif
