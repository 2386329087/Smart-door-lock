#ifndef __DHT11_H
#define __DHT11_H

#include "debug.h"

#define DHT11_RCC    RCC_APB2Periph_GPIOB
#define DHT11_PORT   GPIOB
#define DHT11_IO     GPIO_Pin_0

u8 DHT11_Init(void);//³õÊ¼»¯DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//¶ÁÈ¡ÎÂÊª¶È
u8 DHT11_Read_Byte(void);//¶Á³öÒ»¸ö×Ö½Ú
u8 DHT11_Read_Bit(void);//¶Á³öÒ»¸öÎ»
u8 DHT11_Check(void);//¼ì²âÊÇ·ñ´æÔÚDHT11
void DHT11_Rst(void);//¸´Î»DHT11

#endif
