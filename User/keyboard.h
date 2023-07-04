#ifndef __keyboard_h
#define __keyboard_h

#include "debug.h"
#include <string.h>
#include <stdio.h>

void keyboard_Pin_Init(void);
char keyboard__char_scan(void);
uint8_t keyboard_uint_scan(void);
uint16_t ttp229_read(void);

#endif
