#ifndef __ESP8266_H
#define __ESP8266_H

#include "debug.h"

typedef struct _TDateTime  //ʱ��ṹ��
{
    char hour;
    char minute;
    char second;
    char msec;
}TDateTime;

extern char timeout;
extern TDateTime TIME;

void esp8266_Init(char *ID,char *Password);
void system_time_increase(void);
void get_true_time(char *Date,char *Week);


#endif
