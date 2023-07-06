#include "esp8266.h"
#include "lcd_st7789.h"

void esp8266_USART6_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    // 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART6, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* USART6 TX-->C0  RX-->C1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // RX，输入上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;                                    // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 数据位 8
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 停止位 1
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 // 使能 RX 和 TX

    USART_Init(UART6, &USART_InitStructure);
    DMA_Cmd(DMA2_Channel7, ENABLE); // 开启接收 DMA
    USART_Cmd(UART6, ENABLE);       // 开启UART
}

void esp8266_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

        // TX DMA 初始化
        DMA_DeInit(DMA2_Channel6);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART6->DATAR);        // DMA 外设基址，需指向对应的外设
        DMA_InitStructure.DMA_MemoryBaseAddr = 0x2000A000;                  // DMA 内存基址，指向发送缓冲区的首地址
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // 方向 : 外设 作为 终点，即 内存 ->  外设
        DMA_InitStructure.DMA_BufferSize = 0;                                   // 缓冲区大小,即要DMA发送的数据长度,目前没有数据可发
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址自增，禁用
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址自增，启用
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据位宽，8位(Byte)
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据位宽，8位(Byte)
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 普通模式，发完结束，不循环发送
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级最高
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // M2P,禁用M2M
        DMA_Init(DMA2_Channel6, &DMA_InitStructure);
}

void UART6_SendData(uint8_t Data)
{
    USART_SendData(UART6, Data);
    while (USART_GetFlagStatus(UART6, USART_FLAG_TXE) == RESET);
}

void UART6_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i ++)
    {
        UART6_SendData(String[i]);
    }
}



