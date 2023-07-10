/********************************** (C) COPYRIGHT *******************************
 * File Name          : lcd_st7789.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file contains the headers of the TFTLCD.
 *******************************************************************************/

#include "lcd_st7789.h"
#include "fonts.h"

#define LCD_CLEAR_SEND_NUMBER 5760

u16 BACK_COLOR = BLACK, FORE_COLOR = WHITE;

void LCD_WriteCmd(const u8 cmd)
{
    *(__IO u8 *)LCD_CMD = cmd;
}

void LCD_WriteData(const u8 data)
{
    *(__IO u8 *)LCD_DATA = data;
}

void LCD_WriteHalfWord(const u16 data)
{
    *(__IO u8 *)LCD_DATA = (u8)(data >> 8);
    *(__IO u8 *)LCD_DATA = (u8)data;
}

u8 LCD_ReadData(void)
{
    vu8 ram;
    ram = *(__IO u8 *)LCD_DATA;
    return ram;
}

u16 LCD_ReadHalfWord(void)
{
    vu16 ram;
    ram = *(__IO u8 *)LCD_DATA;
    ram <<= 8;
    ram |= *(__IO u8 *)LCD_DATA;
    return ram;
}

u8 LCD_ReadReg(const u8 reg)
{
    LCD_WriteCmd(reg);
    delay_us(5);
    return LCD_ReadData();
}

void LCD_FSMCInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure = {0};
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
    FSMC_NORSRAMTimingInitTypeDef writeTiming = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* RS : PD12 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* CS : PD7*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_7);

    readWriteTiming.FSMC_AddressSetupTime = 0x01;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x0f;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    writeTiming.FSMC_AddressSetupTime = 0x00;
    writeTiming.FSMC_AddressHoldTime = 0x00;
    writeTiming.FSMC_DataSetupTime = 0x03;
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;

    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/*******************************************************************************
 * Function Name  : LCD_GPIOInit
 * Description    : Initializes TIM1 PWM output.
 * Input          : None
 * Return         : None
 *******************************************************************************/
void LCD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_TIM1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    TIM_TimeBaseInitStructure.TIM_Period = 100;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 144 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

#if (PWM_MODE == PWM_MODE1)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

#elif (PWM_MODE == PWM_MODE2)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
#endif

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 50;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/*******************************************************************************
 * Function Name  : LCD_GPIOInit
 * Description    : Set LCD backlight brightness
 * Input          : brightness(0-100)
 * Return         : None
 *******************************************************************************/
void LCD_SetBrightness(u8 brightness)
{
    if (brightness > 100)
        brightness = 100;
    TIM_SetCompare2(TIM1, brightness);
}

void LCD_Init(void)
{
    u16 id1, id2, id3 = 0;
    LCD_GPIOInit();
    LCD_FSMCInit();

    LCD_WriteCmd(0x04);
    LCD_ReadData();       // dummy read
    id1 = LCD_ReadData(); // manufacturer ID 读取0X85
    id2 = LCD_ReadData(); // version ID 读取0X85
    id3 = LCD_ReadData(); // module/driver ID 读取0X52

    printf("LCD manufacturer ID = 0x%x, ", id1);
    printf("version ID = 0x%x, ", id2);
    printf("driver ID = 0x%x\r\n", id3);
    delay_ms(50);

    /* Memory Data Access Control */
    LCD_WriteCmd(0x36);
    LCD_WriteData(0x00);
    /* RGB 5-6-5-bit */
    LCD_WriteCmd(0x3A);
    LCD_WriteData(0x05);
    /* RAM big endian */
    LCD_WriteCmd(0xB0);
    LCD_WriteData(0x00);
    LCD_WriteData(0xC0);
    /* Porch Setting */
    LCD_WriteCmd(0xB2);
    LCD_WriteData(0x0C);
    LCD_WriteData(0x0C);
    LCD_WriteData(0x00);
    LCD_WriteData(0x33);
    LCD_WriteData(0x33);
    /* Gate Control */
    LCD_WriteCmd(0xB7);
    LCD_WriteData(0x00);
    /* VCOM Setting */
    LCD_WriteCmd(0xBB);
    LCD_WriteData(0x3F);
    /* LCM Control */
    LCD_WriteCmd(0xC0);
    LCD_WriteData(0x2C);
    /* VDV and VRH Command Enable */
    LCD_WriteCmd(0xC2);
    LCD_WriteData(0x01);
    /* VRH Set */
    LCD_WriteCmd(0xC3);
    LCD_WriteData(0x0D);
    /* VDV Set */
    LCD_WriteCmd(0xC4);
    LCD_WriteData(0x20);

    /* Frame Rate Control in Normal Mode */
    LCD_WriteCmd(0xC6);
    LCD_WriteData(0x03); // 60Hz-0x0F   82Hz-0x07  99Hz-0x03

    /* Power Control 1 */
    LCD_WriteCmd(0xD0);
    LCD_WriteData(0xA4);
    LCD_WriteData(0xA1);
    /* Positive Voltage Gamma Control */
    LCD_WriteCmd(0xE0);
    LCD_WriteData(0xF0);
    LCD_WriteData(0x03);
    LCD_WriteData(0x09);
    LCD_WriteData(0x03);
    LCD_WriteData(0x03);
    LCD_WriteData(0x10);
    LCD_WriteData(0x2D);
    LCD_WriteData(0x43);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x33);
    LCD_WriteData(0x0D);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x29);
    LCD_WriteData(0x32);
    /* Negative Voltage Gamma Control */
    LCD_WriteCmd(0xE1);
    LCD_WriteData(0xF0);
    LCD_WriteData(0x0C);
    LCD_WriteData(0x10);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x0A);
    LCD_WriteData(0x2D);
    LCD_WriteData(0x33);
    LCD_WriteData(0x45);
    LCD_WriteData(0x3A);
    LCD_WriteData(0x14);
    LCD_WriteData(0x19);
    LCD_WriteData(0x31);
    LCD_WriteData(0x37);
    /* Display Inversion On */
    LCD_WriteCmd(0x21);
    /* Sleep Out */
    LCD_WriteCmd(0x11);
    /* wait for power stability */
    delay_ms(100);

    LCD_Clear(BLACK);

    /* display on */
    LCD_WriteCmd(0x29);
}

u16 LCD_ConvertColor(u32 rgb888)
{
    u16 rgb565 = ((rgb888 >> 8) & 0xF800) | ((rgb888 >> 5) & 0x07E0) | ((rgb888 >> 3) & 0x001F);
    ;
    return rgb565;
}

/**
 * Set background color and foreground color
 *
 * @param   back    background color
 * @param   fore    fore color
 *
 * @return  void
 */
void LCD_SetColor(u16 back, u16 fore)
{
    BACK_COLOR = back;
    FORE_COLOR = fore;
}

void LCD_DisplayOn(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void LCD_DisplayOff(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

/* lcd enter the minimum power consumption mode and backlight off. */
void LCD_EnterSleep(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    delay_ms(5);
    LCD_WriteCmd(0x10);
}
/* lcd turn off sleep mode and backlight on. */
void LCD_ExitSleep(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    delay_ms(5);
    LCD_WriteCmd(0x11);
    delay_ms(120);
}

/**
 * Set drawing area
 *
 * @param   x1      start of x position
 * @param   y1      start of y position
 * @param   x2      end of x position
 * @param   y2      end of y position
 *
 * @return  void
 */
void LCD_AddressSetWrite(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WriteCmd(0x2a);
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1);
    LCD_WriteData(x2 >> 8);
    LCD_WriteData(x2);

    LCD_WriteCmd(0x2b);
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1);
    LCD_WriteData(y2 >> 8);
    LCD_WriteData(y2);

    LCD_WriteCmd(0x2C);
}

/**
 * Read drawing area
 *
 * @param   x1      start of x position
 * @param   y1      start of y position
 * @param   x2      end of x position
 * @param   y2      end of y position
 *
 * @return  void
 */
void LCD_AddressSetRead(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WriteCmd(0x2a);
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1);
    LCD_WriteData(x2 >> 8);
    LCD_WriteData(x2);

    LCD_WriteCmd(0x2b);
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1);
    LCD_WriteData(y2 >> 8);
    LCD_WriteData(y2);

    LCD_WriteCmd(0x2E);
}

/**
 * clear the lcd.
 *
 * @param   color       Fill color
 *
 * @return  void
 */
void LCD_Clear(u16 color)
{
    u16 i, j;
    u8 data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;
    LCD_AddressSetWrite(0, 0, LCD_W - 1, LCD_H - 1);

    /* 5760 = 240*240/20 */

    for (i = 0; i < LCD_W; i++)
    {
        for (j = 0; j < LCD_H; j++)
        {
            *(__IO u8 *)LCD_DATA = data[0];
            *(__IO u8 *)LCD_DATA = data[1];
        }
    }
}

/**
 * display a point on the lcd.
 *
 * @param   x   x position
 * @param   y   y position
 *
 * @return  void
 */
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_AddressSetWrite(x, y, x, y);
    LCD_WriteHalfWord(FORE_COLOR);
}

void LCD_DrawPoint_color(u16 x, u16 y,u16 color)
{
    LCD_AddressSetWrite(x, y, x, y);
    LCD_WriteHalfWord(color);
}

/**
 * display a point on the lcd using the given colour.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   color   color of point
 *
 * @return  void
 */
void LCD_Dot(u16 x, u16 y, u16 color)
{
    LCD_AddressSetWrite(x, y, x, y);
    LCD_WriteHalfWord(color);
}

/**
 * full color on the lcd.
 *
 * @param   x_start     start of x position
 * @param   y_start     start of y position
 * @param   x_end       end of x position
 * @param   y_end       end of y position
 * @param   color       Fill color
 *
 * @return  void
 */
void LCD_Fill(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color)
{
    u16 i = 0, j = 0;

    LCD_AddressSetWrite(x_start, y_start, x_end, y_end);
    for (i = y_start; i <= y_end; i++)
    {
        for (j = x_start; j <= x_end; j++)
            LCD_WriteHalfWord(color);
    }
}

/**
 * display a line on the lcd.
 *
 * @param   x1      x1 position
 * @param   y1      y1 position
 * @param   x2      x2 position
 * @param   y2      y2 position
 *
 * @return  void
 */
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    u32 i = 0;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (y1 == y2)
    {
        /* fast draw transverse line */
        LCD_AddressSetWrite(x1, y1, x2, y2);

        u8 line_buf[480] = {0};

        for (i = 0; i < x2 - x1; i++)
        {
            line_buf[2 * i] = FORE_COLOR >> 8;
            line_buf[2 * i + 1] = FORE_COLOR;
        }

        for (i = 0; i < (x2 - x1) * 2; i++)
        {
            *(__IO u8 *)LCD_DATA = line_buf[i];
        }

        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x;
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++)
    {
        LCD_DrawPoint(row, col);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * display a rectangle on the lcd.
 *
 * @param   x1      x1 position
 * @param   y1      y1 position
 * @param   x2      x2 position
 * @param   y2      y2 position
 * @param   inFill  display infill color or left transparent
 *
 * @return  void
 */
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u8 inFill)
{
    if (inFill)
        LCD_Fill(x1, y1, x2, y2, BACK_COLOR);
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/**
 * display a circle on the lcd.
 *
 * @param   x       x position of Center
 * @param   y       y position of Center
 * @param   r       radius
 *
 * @return  void
 */
void LCD_DrawCircle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while (a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a);
        LCD_DrawPoint(x0 + b, y0 - a);
        LCD_DrawPoint(x0 - a, y0 + b);
        LCD_DrawPoint(x0 - b, y0 - a);
        LCD_DrawPoint(x0 - a, y0 - b);
        LCD_DrawPoint(x0 + b, y0 + a);
        LCD_DrawPoint(x0 + a, y0 - b);
        LCD_DrawPoint(x0 + a, y0 + b);
        LCD_DrawPoint(x0 - b, y0 + a);
        a++;
        // Bresenham
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
        LCD_DrawPoint(x0 + a, y0 + b);
    }
}

/**
 * display the character on the lcd.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   data    the character to be displayed
 * @param   size    size of font
 * @param   bkgd    display background color or left transparent
 *
 * @return   0: display success
 *          -1: size of font is not support
 */
void LCD_ShowChar(u16 x, u16 y, u8 data, u32 size, u8 bkground)
{
    u8 temp_fontData;
    u8 num = 0;
    ;
    u16 pos, t;
    u16 colortemp = FORE_COLOR;
    if (x > LCD_W - size / 2 || y > LCD_H - size)
        return;

    data = data - ' ';

#ifdef ASC2_1608
    if (size == 16)
    {
        u8 font_buff[384] = {0};
        if (!bkground)
        {
            LCD_AddressSetRead(x, y, x + size / 2 - 1, y + size - 1);
            LCD_ReadData();
            for (pos = 0; pos < size * (size / 2) * 3; pos++)
            {
                font_buff[pos] = LCD_ReadData();
            }
        }
        LCD_AddressSetWrite(x, y, x + size / 2 - 1, y + size - 1); //(x,y,x+8-1,y+16-1)
        /* fast show char */
        for (pos = 0; pos < size * (size / 2) / 8; pos++)
        {
            temp_fontData = asc2_1608[(u16)data * size * (size / 2) / 8 + pos];
            for (t = 0; t < 8; t++)
            {
                if (temp_fontData & 0x80)
                    colortemp = FORE_COLOR;
                else
                {
                    if (bkground)
                        colortemp = BACK_COLOR;
                    else
                        colortemp = LCD_ConvertColor(font_buff[(pos * 8 + t) * 3] << 16 | font_buff[(pos * 8 + t) * 3 + 1] << 8 | font_buff[(pos * 8 + t) * 3 + 2]);
                }
                LCD_WriteHalfWord(colortemp);
                temp_fontData <<= 1;
            }
        }
    }
    else
#endif

#ifdef ASC2_2412
        if (size == 24)
    {
        u8 font_buff[864] = {0};
        if (!bkground)
        {
            LCD_AddressSetRead(x, y, x + size / 2 - 1, y + size - 1);
            LCD_ReadData();
            for (pos = 0; pos < size * (size / 2) * 3; pos++)
            {
                font_buff[pos] = LCD_ReadData();
            }
        }
        LCD_AddressSetWrite(x, y, x + size / 2 - 1, y + size - 1);
        /* fast show char */
        for (pos = 0; pos < (size * 16) / 8; pos++)
        {
            temp_fontData = asc2_2412[(u16)data * (size * 16) / 8 + pos];
            if (pos % 2 == 0)
            {
                num = 8;
            }
            else
            {
                num = 4;
            }
            for (t = 0; t < num; t++)
            {
                if (temp_fontData & 0x80)
                    colortemp = FORE_COLOR;
                else
                {
                    if (bkground)
                        colortemp = BACK_COLOR;
                    else
                    {
                        if (pos % 2 == 0)
                            colortemp = LCD_ConvertColor(font_buff[(pos * 6 + t) * 3] << 16 | font_buff[(pos * 6 + t) * 3 + 1] << 8 | font_buff[(pos * 6 + t) * 3 + 2]);
                        else
                            colortemp = LCD_ConvertColor(font_buff[(pos * 6 + 2 + t) * 3] << 16 | font_buff[(pos * 6 + 2 + t) * 3 + 1] << 8 | font_buff[(pos * 6 + 2 + t) * 3 + 2]);
                    }
                }
                LCD_WriteHalfWord(colortemp);
                temp_fontData <<= 1;
            }
        }
    }
    else
#endif

#ifdef ASC2_3216
        if (size == 32)
    {
        u8 font_buff[1536] = {0};
        if (!bkground)
        {
            LCD_AddressSetRead(x, y, x + size / 2 - 1, y + size - 1);
            LCD_ReadData();
            for (pos = 0; pos < size * (size / 2) * 3; pos++)
            {
                font_buff[pos] = LCD_ReadData();
            }
        }
        LCD_AddressSetWrite(x, y, x + size / 2 - 1, y + size - 1);
        /* fast show char */
        for (pos = 0; pos < size * (size / 2) / 8; pos++)
        {
            temp_fontData = asc2_3216[(u16)data * size * (size / 2) / 8 + pos];
            for (t = 0; t < 8; t++)
            {
                if (temp_fontData & 0x80)
                    colortemp = FORE_COLOR;
                else
                {
                    if (bkground)
                        colortemp = BACK_COLOR;
                    else
                        colortemp = LCD_ConvertColor(font_buff[(pos * 8 + t) * 3] << 16 | font_buff[(pos * 8 + t) * 3 + 1] << 8 | font_buff[(pos * 8 + t) * 3 + 2]);
                }
                LCD_WriteHalfWord(colortemp);
                temp_fontData <<= 1;
            }
        }
    }
    else
#endif
    {
        printf("There is no any define ASC2_1208 && ASC2_2412 && ASC2_2416 && ASC2_3216 !");
    }
}

/**
 * display the number on the lcd.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   num     number
 * @param   len     length of number
 * @param   size    size of font
 * @param   bkgd    display background color or left transparent
 *
 * @return  void
 */
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u32 size, u8 bkgd)
{
    LCD_ShowString(x, y, size, bkgd, "%d", num);
}

/**
 * display the string on the lcd.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   size    size of font
 * @param   bkgd    display background color or left transparent
 * @param   p       the string to be displayed
 *
 * @return   0: display success
 *          -1: size of font is not support
 */
void LCD_ShowString(u16 x, u16 y, u32 size, u8 bkgd, const char *fmt, ...)
{
#define LCD_STRING_BUF_LEN 128

    va_list args;
    u8 buf[LCD_STRING_BUF_LEN] = {0};
    u8 *p = NULL;

    if (size != 16 && size != 24 && size != 32)
    {
        printf("font size(%d) is not support!", size);
    }

    va_start(args, fmt);
    vsnprintf((char *)buf, 100, (const char *)fmt, args);
    va_end(args);

    p = buf;
    while (*p != '\0')
    {
        if (x > LCD_W - size / 2)
        {
            x = 0;
            y += size;
        }
        if (y > LCD_H - size)
        {
            y = x = 0;
            LCD_Clear(RED);
        }
        LCD_ShowChar(x, y, *p, size, bkgd);
        x += size / 2;
        p++;
    }
}

/**
 * display the image on the lcd.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   length  length of image
 * @param   wide    wide of image
 * @param   p       image
 *
 * @return   0: display success
 *          -1: the image is too large
 */
void LCD_ShowImage(u16 x, u16 y, u16 width, u16 height, const u16 *p)
{
    u16 i = 0;

    if (x + width > LCD_W || y + height > LCD_H)
    {
        printf("Error");
        return;
    }

    LCD_AddressSetWrite(x, y, x + width - 1, y + height - 1);
    for (i = 0; i < width * height ; i++)
    {
        *(__IO u8 *)LCD_DATA = p[i]>>8;
        *(__IO u8 *)LCD_DATA = p[i];
    }
}

/**
 * display the image as an overlay.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   length  length of image
 * @param   wide    wide of image
 * @param   p       image
 *
 * @return   0: display success
 *          -1: the image is too large
 */
void LCD_OverlayImage(u16 x, u16 y, u16 width, u16 height, u8 rotate, u8 flip, const u8 *p)
{
    u16 row = 0, col = 0;
    u16 tempR, tempG, tempB;

    if (x + width > LCD_W || y + height > LCD_H)
    {
        printf("Error");
        return;
    }

    u8 buff[720] = {0};
    for (row = 0; row < height; row++)
    {
        // read original area
        LCD_AddressSetRead(x, y + row, x + width - 1, y + row);
        LCD_ReadData();
        for (col = 0; col < width * 3; col++)
        {
            buff[col] = LCD_ReadData();
        }
        // calculate overlay
        for (col = 0; col < width; col++)
        {
            u8 pixel_high, pixel_low = 0;
            if (!flip)
            { // no flip
                switch (rotate)
                {
                case 0x00: //   0deg
                    pixel_high = p[2 * (width * row + col)];
                    pixel_low = p[2 * (width * row + col) + 1];
                    break;
                case 0x01: //  90deg
                    pixel_high = p[2 * (width * col + (239 - row))];
                    pixel_low = p[2 * (width * col + (239 - row)) + 1];
                    break;
                case 0x02: // 180deg
                    pixel_high = p[2 * (width * (239 - row) + (239 - col))];
                    pixel_low = p[2 * (width * (239 - row) + (239 - col)) + 1];
                    break;
                case 0x03: // 270deg
                    pixel_high = p[2 * (width * (239 - col) + row)];
                    pixel_low = p[2 * (width * (239 - col) + row) + 1];
                    break;
                default:
                    pixel_high = 0;
                    pixel_low = 0;
                }
            }
            else
            { // vertical flip (source file)
                switch (rotate)
                {
                case 0x00: //   0deg
                    pixel_high = p[2 * (width * (239 - row) + col)];
                    pixel_low = p[2 * (width * (239 - row) + col) + 1];
                    break;
                case 0x01: //  90deg
                    pixel_high = p[2 * (width * (239 - col) + (239 - row))];
                    pixel_low = p[2 * (width * (239 - col) + (239 - row)) + 1];
                    break;
                case 0x02: // 180deg
                    pixel_high = p[2 * (width * row + (239 - col))];
                    pixel_low = p[2 * (width * row + (239 - col)) + 1];
                    break;
                case 0x03: // 270deg
                    pixel_high = p[2 * (width * col + row)];
                    pixel_low = p[2 * (width * col + row) + 1];
                    break;
                default:
                    pixel_high = 0;
                    pixel_low = 0;
                }
            }
            // R
            tempR = buff[3 * col] + (pixel_high & 0xF8);
            if (tempR > 0xFF)
                buff[3 * col] = 0xFF;
            else
                buff[3 * col] = tempR;
            // G
            tempG = buff[3 * col + 1] + (((pixel_high & 0x07) << 5) | ((pixel_low & 0xE0) >> 3));
            if (tempG > 0xFF)
                buff[3 * col + 1] = 0xFF;
            else
                buff[3 * col + 1] = tempG;
            // B
            tempB = buff[3 * col + 2] + ((pixel_low & 0x1F) << 3);
            if (tempB > 0xFF)
                buff[3 * col + 2] = 0xFF;
            else
                buff[3 * col + 2] = tempB;
        }
        // write new area
        LCD_AddressSetWrite(x, y + row, x + width - 1, y + row);
        for (col = 0; col < width; col++)
        {
            LCD_WriteHalfWord(LCD_ConvertColor(buff[3 * col] << 16 | buff[3 * col + 1] << 8 | buff[3 * col + 2]));
        }
    }
}
