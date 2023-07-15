#ifndef __ESP8266_H
#define __ESP8266_H

#include "debug.h"

typedef struct _TDateTime  //时间结构体
{
    char hour;
    char minute;
    char second;
    char msec;
}TDateTime;

extern char timeout;
extern TDateTime TIME;

void esp8266_Init(char *ID,char *Password);
void esp8266_Init_2(char *ID,char *Password);
void system_time_increase(void);
void get_true_time(char *Time,char *Date,char *Week);
FlagStatus uartWriteWiFiStr(char * str);

#endif
