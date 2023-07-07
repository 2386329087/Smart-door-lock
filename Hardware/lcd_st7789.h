/********************************** (C) COPYRIGHT *******************************
 * File Name          : lcd_st7789.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file contains the headers of the TFTLCD.
 *******************************************************************************/
#ifndef USER_LCD_ST7789_H_
#define USER_LCD_ST7789_H_

#include "debug.h"
#include "stdlib.h"
#include "stdarg.h"

/* PWM Output Mode Definition */
#define PWM_MODE1 0
#define PWM_MODE2 1

/* PWM Output Mode Selection */
#define PWM_MODE PWM_MODE2
//#define PWM_MODE PWM_MODE2

#define delay_ms(x) Delay_Ms(x)
#define delay_us(x) Delay_Us(x)

#define LCD_CMD ((u32)0x6001FFFF)
#define LCD_DATA ((u32)0x60020000)

#define LCD_W 240
#define LCD_H 240

// POINT_COLOR
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430
#define GRAY175 0XAD75
#define GRAY151 0X94B2
#define GRAY187 0XBDD7
#define GRAY240 0XF79E
#define LGRAY 0XC618

void LCD_Init(void);
void LCD_SetBrightness(u8 brightness);

void LCD_Clear(u16 color);
void LCD_AddressSetWrite(u16 x1, u16 y1, u16 x2, u16 y2);
u16 LCD_ConvertColor(u32 rgb888);
void LCD_SetColor(u16 back, u16 fore);

void LCD_Dot(u16 x, u16 y, u16 color);
void LCD_Fill(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color);

void LCD_DrawPoint(u16 x, u16 y);
void LCD_DrawPoint_color(u16 x, u16 y,u16 color);
void LCD_DrawCircle(u16 x0, u16 y0, u8 r);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u8 inFill);

void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u32 size, u8 bkground);
void LCD_ShowString(u16 x, u16 y, u32 size, u8 bkground, const char *fmt, ...);
void LCD_ShowImage(u16 x, u16 y, u16 length, u16 wide, const u16 *p);
void LCD_OverlayImage(u16 x, u16 y, u16 width, u16 height, u8 rotate, u8 flip, const u8 *p);

void LCD_EnterSleep(void);
void LCD_ExitSleep(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

#endif /* USER_LCD_ST7789_H_ */
