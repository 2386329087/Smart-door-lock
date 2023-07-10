/********************************** (C) COPYRIGHT *******************************
 * File Name          : ov.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file contains the headers of the OV2640.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#include "ov.h"
#include "ov2640_regs.h"
#include "lcd_st7789.h"
/*********************************************************************
 * @fn      SCCB_GPIO_Init
 *
 * @brief   Init SCCB GPIO.
 *
 * @return  none
 */
void SCCB_GPIO_Init(void)
{
	IIC_SCL_OUT;
	IIC_SDA_OUT;
	IIC_SCL_SET;
	IIC_SDA_SET;
}

/*********************************************************************
 * @fn      SCCB_Start
 *
 * @brief   Start Signal
 *
 * @return  none
 */
void SCCB_Start(void)
{
	IIC_SDA_SET;
	IIC_SCL_SET;
	Delay_Us(50);
	IIC_SDA_CLR;
	Delay_Us(50);
	IIC_SCL_CLR;
}

/*********************************************************************
 * @fn      SCCB_Stop
 *
 * @brief   Stop Signal
 *
 * @return  none
 */
void SCCB_Stop(void)
{
	IIC_SDA_CLR;
	Delay_Us(50);
	IIC_SCL_SET;
	Delay_Us(50);
	IIC_SDA_SET;
	Delay_Us(50);
}

/*********************************************************************
 * @fn      SCCB_No_Ack
 *
 * @brief   NAK Signal
 *
 * @return  none
 */
void SCCB_No_Ack(void)
{
	Delay_Us(50);
	IIC_SDA_SET;
	IIC_SCL_SET;
	Delay_Us(50);
	IIC_SCL_CLR;
	Delay_Us(50);
	IIC_SDA_CLR;
	Delay_Us(50);
}

/*********************************************************************
 * @fn      SCCB_WR_Byte
 *
 * @brief   Write One Byte
 *
 * @param   data
 *
 * @return  0 - Success
 *          other - Err
 */
UINT8 SCCB_WR_Byte(UINT8 data)
{
	UINT8 i, t;

	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
		{
			IIC_SDA_SET;
		}
		else
		{
			IIC_SDA_CLR;
		}

		data <<= 1;
		Delay_Us(50);
		IIC_SCL_SET;
		Delay_Us(50);
		IIC_SCL_CLR;
	}

	IIC_SDA_IN;
	Delay_Us(50);
	IIC_SCL_SET;
	Delay_Us(50);

	if (SDA_IN_R)
		t = 1;
	else
		t = 0;

	IIC_SCL_CLR;
	IIC_SDA_OUT;

	return t;
}

/*********************************************************************
 * @fn      SCCB_RD_Byte
 *
 * @brief   Read One Byte
 *
 * @return  Read one byte data
 */
UINT8 SCCB_RD_Byte(void)
{
	UINT8 t = 0, i;

	IIC_SDA_IN;

	for (i = 8; i > 0; i--)
	{
		Delay_Us(50);
		IIC_SCL_SET;
		t = t << 1;

		if (SDA_IN_R)
			t++;

		Delay_Us(50);
		IIC_SCL_CLR;
	}

	IIC_SDA_OUT;

	return t;
}

/*********************************************************************
 * @fn      SCCB_WR_Reg
 *
 * @brief   Write camera Register
 *
 * @param   Reg_Adr - Register address
 *          Reg_Val - Register value
 *
 * @return  0 - Success
 *          other - Err
 */
UINT8 SCCB_WR_Reg(UINT8 Reg_Adr, UINT8 Reg_Val)
{
	UINT8 res = 0;

	SCCB_Start();
	if (SCCB_WR_Byte(SCCB_ID))
		res = 1;
	Delay_Us(100);
	if (SCCB_WR_Byte(Reg_Adr))
		res = 1;
	Delay_Us(100);
	if (SCCB_WR_Byte(Reg_Val))
		res = 1;
	SCCB_Stop();

	return res;
}

/*********************************************************************
 * @fn      SCCB_RD_Reg
 *
 * @brief   Read camera Register
 *
 * @return  Camera Register value
 */
UINT8 SCCB_RD_Reg(UINT8 Reg_Adr)
{
	UINT8 val = 0;

	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID);
	Delay_Us(100);
	SCCB_WR_Byte(Reg_Adr);
	Delay_Us(100);
	SCCB_Stop();
	Delay_Us(100);

	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID | 0X01);
	Delay_Us(100);
	val = SCCB_RD_Byte();
	SCCB_No_Ack();
	SCCB_Stop();

	return val;
}

/*********************************************************************
 * @fn      DVP_GPIO_Init
 *
 * @brief   Init DVP GPIO.
 *
 * @return  none
 */
void DVP_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); //使锟斤拷锟斤拷囟丝锟绞憋拷锟�

	// PC3-PC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      OV2640_Init
 *
 * @brief   Init OV2640
 *
 * @return  0 - Success
 *          1 - Err
 */
UINT8 OV2640_Init(void)
{
	UINT16 i = 0;
	UINT16 reg;

	DVP_GPIO_Init();

	OV_PWDN_CLR; // POWER ON
	Delay_Ms(10);
	OV_RESET_CLR; // Reset OV2640
	Delay_Ms(10);
	OV_RESET_SET; // Reset End

	SCCB_GPIO_Init();
	SCCB_WR_Reg(0xFF, 0x01); // Register Bank Select Sensor address
	SCCB_WR_Reg(0x12, 0x80); // Reset All Register

	Delay_Ms(50);

	// Read MID
	reg = SCCB_RD_Reg(0x1C);
	reg <<= 8;
	reg |= SCCB_RD_Reg(0x1D);

	if (reg != OV2640_MID)
	{
		printf("MID:%08x\r\n", reg);
		return 1;
	}

	// Read PID
	reg = SCCB_RD_Reg(0x0A);
	reg <<= 8;
	reg |= SCCB_RD_Reg(0x0B);

	if (reg != OV2640_PID)
	{
		printf("PID:%08x\r\n", reg);
		return 2;
	}

	for (i = 0; i < sizeof(ov2640_init_regs) / 2; i++)
	{
		SCCB_WR_Reg(ov2640_init_regs[i][0], ov2640_init_regs[i][1]);
	}

	return 0;
}

/*********************************************************************
 * @fn      RGB565_Mode_Init
 *
 * @brief   Init RGB565 mode
 *
 * @return  none
 */
void RGB565_Mode_Init(void)
{
	// OV2640_OutSize_Set(OV2640_RGB565_WIDTH,OV2640_RGB565_HEIGHT);
	// OV2640_Speed_Set(28,1);
	OV2640_OutSize_Set_Custom();
	OV2640_RGB565_Mode();
}

/*********************************************************************
 * @fn      OV2640_RGB565_Mode
 *
 * @brief   RGB565 Mode
 *
 * @return  none
 */
void OV2640_RGB565_Mode(void)
{
	UINT16 i = 0;

	// preamble
	for (i = 0; i < (sizeof(ov2640_format_change_preamble_regs) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_format_change_preamble_regs[i][0], ov2640_format_change_preamble_regs[i][1]);
	}
	// RGB565
	for (i = 0; i < (sizeof(ov2640_rgb565_be_regs) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_be_regs[i][0], ov2640_rgb565_be_regs[i][1]);
	}
}

/*********************************************************************
 * @fn      OV2640_OutSize_Set
 *
 * @brief   Set Image Resolution
 *
 * @param   Image_width -
 *          Image_height -
 *
 * @return  none
 */
UINT8 OV2640_OutSize_Set(UINT16 Image_width, UINT16 Image_height)
{
	UINT16 Out_Size_Width;
	UINT16 Out_Size_Height;
	UINT8 t;

	if ((Image_width % 4) || (Image_height % 4))
		return 0x01;

	Out_Size_Width = Image_width / 4;
	Out_Size_Height = Image_height / 4;

	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XE0, 0X04);
	SCCB_WR_Reg(0X5A, Out_Size_Width & 0XFF);
	SCCB_WR_Reg(0X5B, Out_Size_Height & 0XFF);
	t = (Out_Size_Width >> 8) & 0X03;
	t |= (Out_Size_Height >> 6) & 0X04;
	SCCB_WR_Reg(0X5C, t);
	SCCB_WR_Reg(0XE0, 0X00);

	return 0;
}

UINT8 OV2640_OutSize_Set_Custom(void)
{
	UINT16 i = 0;

	// preamble
	for (i = 0; i < (sizeof(ov2640_size_change_preamble_regs) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_size_change_preamble_regs[i][0], ov2640_size_change_preamble_regs[i][1]);
	}
	// custom:240*240
	for (i = 0; i < (sizeof(ov2640_qvga_regs) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_qvga_regs[i][0], ov2640_qvga_regs[i][1]);
	}

	return 0;
}

/*********************************************************************
 * @fn      OV2640_Speed_Set
 *
 * @brief   Set DVP PCLK
 *
 * @param   Pclk_Div - DVP output speed ctrl
 *          Xclk_Div - Crystal oscillator input frequency division
 *
 * @return  0 - Success
 *          other - Err
 */
void OV2640_Speed_Set(UINT8 Pclk_Div, UINT8 Xclk_Div)
{
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XD3, Pclk_Div);

	SCCB_WR_Reg(0XFF, 0X01);
	SCCB_WR_Reg(0X11, Xclk_Div);
}


/* Global define */
#define TRUE 1
#define FALSE 0

/* DVP Work Mode */
#define RGB565_MODE 0
/* DVP Work Mode Selection */
#define DVP_Work_Mode RGB565_MODE

UINT32 JPEG_DVPDMAaddr0 = 0x20005000;
UINT32 JPEG_DVPDMAaddr1 = 0x20005000 + OV2640_JPEG_WIDTH;

UINT32 RGB565_DVPDMAaddr0 = 0x2000A000;
UINT32 RGB565_DVPDMAaddr1 = 0x2000A000 + RGB565_COL_NUM * 2; // each byte(D9-D2) will take 2 bytes of RAM

volatile UINT32 frame_cnt = 0;
volatile UINT32 addr_cnt = 0;
volatile UINT32 href_cnt = 0;

void DVP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

/*********************************************************************
 * @fn      DMA_SRAMLCD_Init
 *
 * @brief   Init SRAMLCD DMA
 *
 * @param   ddr: DVP data memory base addr.
 *
 * @return  none
 */
void DMA_SRAMLCD_Init(u32 ddr)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    DMA_DeInit(DMA2_Channel5);

    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)LCD_DATA;
    DMA_InitStructure.DMA_MemoryBaseAddr = ddr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = LCD_W;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_SRAMLCD_Enable
 *
 * @brief   Enable SRAMLCD DMA transmission
 *
 * @return  none
 */
void DMA_SRAMLCD_Enable(void)
{
    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Channel5, LCD_W);
    DMA_Cmd(DMA2_Channel5, ENABLE);
}

/*********************************************************************
 * @fn      DVP_Init
 *
 * @brief   Init DVP
 *
 * @return  none
 */
void DVP_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);
    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;

    /* VSYNC、HSYNC - High level active */
    DVP->CR0 |= RB_DVP_D10_MOD | RB_DVP_V_POLAR;
    DVP->CR1 &= ~((RB_DVP_ALL_CLR) | RB_DVP_RCV_CLR);
    DVP->ROW_NUM = RGB565_ROW_NUM; // rows
    DVP->COL_NUM = RGB565_COL_NUM; // cols
    DVP->HOFFCNT = (RGB565_COL_NUM - ROI_WIDTH) / 2;
    DVP->VST = (RGB565_ROW_NUM - ROI_HEIGTH) / 2;
    DVP->CAPCNT = ROI_WIDTH;
    DVP->VLINE = ROI_HEIGTH;
    DVP->CR1 |= RB_DVP_CROP;

    DVP->DMA_BUF0 = RGB565_DVPDMAaddr0; // DMA addr0
    DVP->DMA_BUF1 = RGB565_DVPDMAaddr1; // DMA addr1

    /* Set frame capture rate */
    DVP->CR1 &= ~RB_DVP_FCRC;
    DVP->CR1 |= DVP_RATE_100P; // 100%

    // Interrupt Enable
    DVP->IER |= RB_DVP_IE_STP_FRM;
    DVP->IER |= RB_DVP_IE_FIFO_OV;
    DVP->IER |= RB_DVP_IE_FRM_DONE;
    DVP->IER |= RB_DVP_IE_ROW_DONE;
    DVP->IER |= RB_DVP_IE_STR_FRM;

    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = DVP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DVP->CR1 |= RB_DVP_DMA_EN; // enable DMA
    DVP->CR0 |= RB_DVP_ENABLE; // enable DVP
}

u32 DVP_ROW_cnt = 0;

/*********************************************************************
 * @fn      DVP_IRQHandler
 *
 * @brief   This function handles DVP exception.
 *
 * @return  none
 */
void DVP_IRQHandler(void)
{

    if (DVP->IFR & RB_DVP_IF_ROW_DONE)
    {
        /* Write 0 clear 0 */
        DVP->IFR &= ~RB_DVP_IF_ROW_DONE; // clear Interrupt

        if (addr_cnt % 2) // buf0 done
        {
            addr_cnt++;
            // data shift
            for (u16 i = 0; i < RGB565_COL_NUM * 2; ++i)
            {
                *(u8 *)(RGB565_DVPDMAaddr0 + i) = (u8)(*(u16 *)(RGB565_DVPDMAaddr0 + i * 2) >> 2);
            }
            // Send DVP data to LCD
            DMA_Cmd(DMA2_Channel5, DISABLE);
            DMA_SetCurrDataCounter(DMA2_Channel5, LCD_W);
            DMA2_Channel5->MADDR = RGB565_DVPDMAaddr0;
            DMA_Cmd(DMA2_Channel5, ENABLE);
        }
        else // buf1 done
        {
            addr_cnt++;
            // data shift
            for (u16 i = 0; i < RGB565_COL_NUM * 2; ++i)
            {
                *(u8 *)(RGB565_DVPDMAaddr1 + i) = (u8)(*(u16 *)(RGB565_DVPDMAaddr1 + i * 2) >> 2);
            }
            // Send DVP data to LCD
            DMA_Cmd(DMA2_Channel5, DISABLE);
            DMA_SetCurrDataCounter(DMA2_Channel5, LCD_W);
            DMA2_Channel5->MADDR = RGB565_DVPDMAaddr1;
            DMA_Cmd(DMA2_Channel5, ENABLE);
        }
        href_cnt++;
    }

    if (DVP->IFR & RB_DVP_IF_FRM_DONE)
    {
        DVP->IFR &= ~RB_DVP_IF_FRM_DONE; // clear Interrupt
        addr_cnt = 0;
        href_cnt = 0;
    }

    if (DVP->IFR & RB_DVP_IF_STR_FRM)
    {
        DVP->IFR &= ~RB_DVP_IF_STR_FRM;
    }

    if (DVP->IFR & RB_DVP_IF_STP_FRM)
    {
        DVP->IFR &= ~RB_DVP_IF_STP_FRM;
        frame_cnt++;
    }

    if (DVP->IFR & RB_DVP_IF_FIFO_OV)
    {
        DVP->IFR &= ~RB_DVP_IF_FIFO_OV;
        printf("DVP FIFO OV\r\n");
    }
}


void LCD_reset(void)
{
    LCD_Reset_GPIO_Init();
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);
        Delay_Ms(100);
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
        LCD_Init();
        LCD_Clear(BLACK);
        LCD_SetColor(0x18E3, RED);
}


void ov_display(void)
{
    LCD_reset();
    OV2640_Init();
    RGB565_Mode_Init();
    LCD_AddressSetWrite(0, 0, LCD_W - 1, LCD_H - 1);
    DMA_SRAMLCD_Init((u32)RGB565_DVPDMAaddr0); // DMA2
    DVP_Init();
}

void ov_display_enable(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);
    DMA_Cmd(DMA2_Channel5, ENABLE);

    LCD_Clear(BLACK);

    OV2640_Init();
        RGB565_Mode_Init();
    LCD_AddressSetWrite(0, 0, LCD_W - 1, LCD_H - 1);
        DMA_SRAMLCD_Init((u32)RGB565_DVPDMAaddr0); // DMA2
        DVP_Init();
}

void ov_display_disable(void)
{
    DMA_Cmd(DMA2_Channel5, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, DISABLE);
    LCD_Clear(BLACK);
}
