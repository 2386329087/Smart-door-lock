#include <AS608.h>
#include "debug.h"

uint8_t test;

uint8_t as608_receive_data[128]; // 存放指纹返回信息
uint8_t as608_receive_count;

uint8_t PS_WriteNotepad_code[32];
uint8_t PS_ReadNotepad_code[32];

uint8_t finger_status; // 存放手指状态    0:无手指按下 1:有手指按

uint32_t AS608_ADDR = 0XFFFFFFFF; // 芯片地址
uint16_t AS608_HEAD = 0XEF01;     // 包头
uint16_t AS608_PageID = 0x01;     // 指纹库位置号

uint16_t ID;                                                        // 存放ID号 方便进行删除操作
uint16_t NUM;                                                       // 存放个数 方便进行删除操作
uint8_t check_head[8] = {0XEF, 0X01, 0XFF, 0XFF, 0XFF, 0XFF, 0X07}; // 存放应答包格式头

void UART8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI15_10_IRQHandler(void) __attribute__((interrupt()));

void AS608_PIN_Init(void)
{
    /*串口初始化*/
    Delay_Init();

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART8, ENABLE);

    /* USART8 TX-->E14 RX-->E15*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;                                     // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 数据位 8
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 停止位 1
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 // 使能 RX 和 TX

    USART_Init(UART8, &USART_InitStructure);

    USART_ITConfig(UART8, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = UART8_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(UART8, ENABLE);

    /*外部中断初始化*/
    /* 外部中断E13*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource13);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}



void Uart8_SendData(uint8_t Data)
{
    USART_SendData(UART8, Data);
    while (USART_GetFlagStatus(UART8, USART_FLAG_TXE) == RESET);
}

void UART8_IRQHandler(void)
{
    if (USART_GetITStatus(UART8, USART_IT_RXNE) != RESET)
    {
        as608_receive_data[as608_receive_count] = USART_ReceiveData(UART8);
        as608_receive_count++;

//                test=USART_ReceiveData(UART8);
//                Uart8_SendData(test);
        USART_ClearITPendingBit(UART8, USART_IT_RXNE);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        finger_status = FINGER_EXIST;
    }

    EXTI_ClearFlag(EXTI_Line13);
}

/***************************************************************************
描述: 发送包头
****************************************************************************/
static void as608_send_head(void)
{
    Uart8_SendData(AS608_HEAD>>8);
    Uart8_SendData(AS608_HEAD);
}

/***************************************************************************
描述: 发送芯片地址
****************************************************************************/
static void as608_send_address(void)
{
    Uart8_SendData(AS608_ADDR>>24);
    Uart8_SendData(AS608_ADDR>>16);
    Uart8_SendData(AS608_ADDR>>8);
    Uart8_SendData(AS608_ADDR);
}

/***************************************************************************
描述: 发送包标识
****************************************************************************/
static void as608_send_logo(uint8_t logo)
{
    Uart8_SendData(logo);
}

/***************************************************************************
描述: 发送包长度
****************************************************************************/
static void as608_send_length(uint16_t length)
{
    Uart8_SendData(length>>8);
    Uart8_SendData(length);
}

/***************************************************************************
描述: 发送指令码
****************************************************************************/
static void as608_send_cmd(uint8_t cmd)
{
    Uart8_SendData(cmd);
}


/***************************************************************************
描述: 发送校验和
****************************************************************************/
static void as608_send_checksum(uint16_t checksum)
{
    Uart8_SendData(checksum>>8);
    Uart8_SendData(checksum);
}

/***************************************************************************
描述: 发送BufferID
****************************************************************************/
static void as608_send_BufferID(uint8_t BufferID)
{
    Uart8_SendData(BufferID);
}

/***************************************************************************
描述: 发送StartPage
****************************************************************************/
static void as608_send_StartPage(uint16_t StartPage)
{
    Uart8_SendData(StartPage>>8);
    Uart8_SendData(StartPage);
}

/***************************************************************************
描述: 发送PageNum
****************************************************************************/
static void as608_send_PageNum(uint16_t PageNum)
{
    Uart8_SendData(PageNum>>8);
    Uart8_SendData(PageNum);
}

/***************************************************************************
描述: 发送PageID号
****************************************************************************/
static void as608_send_PageID(uint16_t PageID)
{
    Uart8_SendData(PageID>>8);
    Uart8_SendData(PageID);
}

/***************************************************************************
描述: 发送个数
****************************************************************************/
static void as608_send_N(uint16_t N)
{
    Uart8_SendData(N>>8);
    Uart8_SendData(N);
}

/***************************************************************************
描述: 发送记事本数据
****************************************************************************/
static void as608_send_WriteNotepad(uint8_t Data)
{
    Uart8_SendData(Data);
}

/***************************************************************************
描述: 获取手指指纹图像 存放于图像缓冲区
参数: 无                   指令代码:02H
返回: 00H: 录入指纹成功     01H:收包错误    02H:无手指 03H:录入不成功
****************************************************************************/
uint8_t PS_GetImage(void)
{
    uint8_t result;             //存放结果

    CLEAR_BUFFER;               //清空缓冲区

    as608_send_head();          //发送包头
    as608_send_address();       //发送芯片地址
    as608_send_logo(0x01);      //发送包标识
    as608_send_length(0x03);    //发送包长度
    as608_send_cmd(0x01);       //发送指令码
    as608_send_checksum(0x05);  //发送校验和
    OPEN_UART8_RECEIVE;        //开启串口接收

    result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}


/***************************************************************************
描述: 生成特征
参数: BufferID(特征缓冲区号) 指令代码:02H
      CharBuffer1 的 BufferID: 01H  CharBuffer2的 BufferID: 02H
返回: 00H: 生成特征成功     01H:收包错误    06H:指纹图像太乱生成失败
      07H: 特征太少         15H:图像缓冲区没图像
****************************************************************************/
uint8_t PS_GenChar(uint8_t BufferID)
{
    uint16_t checksum;              //存放校验和
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x04);        //发送包长度
    as608_send_cmd(0x02);           //发送指令码
    as608_send_BufferID(BufferID);  //发送BufferID
    checksum = 0x01 + 0x04 + 0x02 + BufferID;
    as608_send_checksum(checksum);  //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 精确比对两枚指纹特征
参数: 无                   指令代码:03H
返回: 00H: 指纹匹配成功     08H:指纹不匹配       01H:收包错误
****************************************************************************/
uint8_t PS_Match(void)
{
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x03);        //发送包长度
    as608_send_cmd(0x03);           //发送指令码
    as608_send_checksum(0x07);      //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 搜索指纹                                指令代码:04H
参数: BufferID:   01H/02H  StartPage:起始页  PageNum:页数
      ID: 存放搜索到的指纹ID号 否则为0
返回: 00H: 搜索到    09H:没有搜索到       01H:收包错误
****************************************************************************/
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t ID)
{
    uint16_t checksum;              //存放校验和
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x08);        //发送包长度
    as608_send_cmd(0x04);           //发送指令码
    as608_send_BufferID(BufferID);  //发送BufferID
    as608_send_StartPage(StartPage);//发送起始页
    as608_send_PageNum(PageNum);    //发送页数
    checksum = 0x01+0x08+0x04+BufferID+StartPage+PageNum;
    as608_send_checksum(checksum);  //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,&ID); //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 合并特征 将CharBuffer1 和 CharBuffer2 中的 特征合并生成模板
      结果存在CharBuffer1 和 CharBuffer2
参数: 无               指令代码:05H
返回: 00H: 合并成功       01H:收包错误    0AH:合并失败(两枚手指不是同一个)
****************************************************************************/
uint8_t PS_RegModel(void)
{
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x03);        //发送包长度
    as608_send_cmd(0x05);           //发送指令码
    as608_send_checksum(0x09);      //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 储存模板 将CharBuffer1和CharBuffer2的模板文件存到PageID号Flash数据库位置
参数: BufferID:   01H/02H     PageID:指纹库位置号       指令代码:06H
返回: 00H: 储存成功   01H:收包错误 0BH:PageID超出指纹库范围 18H:写Flash出错
****************************************************************************/
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
    uint16_t checksum;              //存放校验和
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x06);        //发送包长度
    as608_send_cmd(0x06);           //发送指令码
    as608_send_BufferID(BufferID);  //发送BufferID
    as608_send_PageID(PageID);      //发送指纹库位置号
    checksum = 0x01+0x06+0x06+BufferID+PageID;
    as608_send_checksum(checksum);  //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,0);   //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 高速搜索 以CharBuffer1或CharBuffer2的特征文件高速搜索整个或者部分指纹库
参数: BufferID:   01H/02H   StartPage:起始页  PageNum:页数 指令代码:1BH
      ID: 存放搜索到的指纹ID号 否则为0
返回: 确认字00H: 搜索到 01H:收包错误 09H:没有搜索到(页码就是0) 和 对应页码
****************************************************************************/
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t ID)
{
    uint16_t checksum;              //存放校验和
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x08);        //发送包长度
    as608_send_cmd(0x1B);           //发送指令码
    as608_send_BufferID(BufferID);  //发送BufferID
    as608_send_StartPage(StartPage);//发送起始页
    as608_send_PageNum(PageNum);    //发送页数
    checksum = 0x01+0x08+0x1B+BufferID+StartPage+PageNum;
    as608_send_checksum(checksum);  //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,&ID); //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 删除模板  删除Flash数据库中指定的ID号开始的N个指纹模板
参数: PageID:指纹库模板号 N:删除的模板个数     指令代码:0CH
返回: 00H: 删除模板成功     01H:收包错误    10H:删除模板失败
****************************************************************************/
uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
{
    uint16_t checksum;              //存放校验和
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x07);        //发送包长度
    as608_send_cmd(0x0C);           //发送指令码
    as608_send_PageID(PageID);      //发送指纹库位置号
    as608_send_N(N);                //发送删除模板的个数

    checksum = 0x01+0x07+0x0C+PageID+N;
    as608_send_checksum(checksum);      //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}



/***************************************************************************
描述: 清空Flash数据库中所有指纹模板
参数: 无               指令代码:0DH
返回: 00H: 清空成功   01H:收包错误    11H:清空失败
****************************************************************************/
uint8_t PS_Empty(void)
{
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x03);        //发送包长度
    as608_send_cmd(0x0D);           //发送指令码
    as608_send_checksum(0X11);      //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收
    result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}


/***************************************************************************
描述: 自动注册模板 采样一次指纹注册模板 自动找位置存放并存储
参数:                 指令代码:10H
返回: 00H: 注册成功   01H:收包错误    1EH:清空失败        ID也会返回
****************************************************************************/
uint8_t PS_Enroll(uint16_t ID)
{
    uint8_t result;                 //存放结果

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x03);        //发送包长度
    as608_send_cmd(0x10);           //发送指令码
    as608_send_checksum(0X14);      //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,&ID); //检测指纹模块数据 3秒时间
    if(result == 0XFF)  result = 0x01;

    return result;
}


/***************************************************************************
描述: 读取模板个数
参数: NUM:个数会保存在NUM中          指令代码:1DH
返回: 00H: 注册成功   01H:收包错误    NUM也会返回
****************************************************************************/
uint8_t PS_ValidTempleteNum(uint16_t *NUM)
{
    uint8_t result;                 //存放结果
    uint16_t temp;

    CLEAR_BUFFER;                   //清空缓冲区
    as608_send_head();              //发送包头
    as608_send_address();           //发送芯片地址
    as608_send_logo(0x01);          //发送包标识
    as608_send_length(0x03);        //发送包长度
    as608_send_cmd(0x1D);           //发送指令码
    as608_send_checksum(0X21);      //发送校验和
    OPEN_UART8_RECEIVE;            //开启串口接收

    result = as608_detection_data(300,&temp);    //检测指纹模块数据 3秒时间
    *NUM=temp;
    if(result == 0XFF)  result = 0x01;

    return result;
}

/***************************************************************************
描述: 写记事本
参数: Data:记事本发送区指针          指令代码:18H
     PageID:0~15;
返回: 00H: 写入成功   01H:收包错误
****************************************************************************/
uint8_t PS_WriteNotepad(uint8_t *Data,uint8_t PageID)
{
    uint8_t result;             //存放结果
    uint8_t i;
    uint16_t temp=0;

        CLEAR_BUFFER;               //清空缓冲区

        as608_send_head();          //发送包头
        as608_send_address();       //发送芯片地址
        as608_send_logo(0x01);      //发送包标识
        as608_send_length(36);    //发送包长度
        as608_send_cmd(0x18);       //发送指令码
        as608_send_BufferID(PageID);
        for (i = 0; i < 32; i++)
        {
            as608_send_WriteNotepad(Data[i]);
            temp+=Data[i];
        }
        temp=0x01+36+0x18+0x01+temp;
        as608_send_checksum(temp);  //发送校验和
        OPEN_UART8_RECEIVE;        //开启串口接收

        result = as608_detection_data(300,NULL);    //检测指纹模块数据 3秒时间
        if(result == 0XFF)  result = 0x01;
        return result;
}

/***************************************************************************
描述: 读记事本
参数: Data:记事本接收区指针         指令代码:18H
     PageID:0~15;
返回: 00H: 写入成功   01H:收包错误
****************************************************************************/
uint8_t PS_ReadNotepad(uint8_t *Data,uint8_t PageID)
{
    uint8_t result=1;                 //存放结果
    uint16_t temp;
    uint16_t wait_time=300;
    char *data;
    uint8_t i;

        CLEAR_BUFFER;                   //清空缓冲区
        as608_send_head();              //发送包头
        as608_send_address();           //发送芯片地址
        as608_send_logo(0x01);          //发送包标识
        as608_send_length(0x04);        //发送包长度
        as608_send_cmd(0x19);           //发送指令码
        as608_send_BufferID(PageID);      //发送页ID
        temp=0x01+0x04+0x19+0x01;
        as608_send_checksum(temp);      //发送校验和
        OPEN_UART8_RECEIVE;            //开启串口接收

        while(wait_time--)
        {
                Delay_Ms(10);
                //匹配数据帧头
                data = strstr((char *)as608_receive_data,(char *)check_head);
                if(data != NULL)
                {
                    for(i=0;i<32;i++)
                    {
                        Data[i]=data[10+i];
                    }
                    result=0;
                    break;
                }
        }
        CLOSE_UART8_RECEIVE;           //禁止串口接收
        return result;
}

void WriteNotepad_32byte(uint8_t *Data,uint8_t PageID,uint8_t size)
{
    uint8_t i;

    uint8_t WriteNotepad[32];

    for (i =0; i < size; ++i)
    {
        WriteNotepad[32-(size-i)]=Data[i];
    }

    PS_WriteNotepad(WriteNotepad,PageID);
}

#if 1
/***************************************************************************
描述: 根据返回结果输出对应的信息
参数: result:返回的结果
****************************************************************************/
void as608_output_result_message(uint8_t result)
{
    switch(result)
    {
        case 0x00:      printf("\r\n");                                 break;
        case 0x01:      printf("收包错误\r\n");                         break;
        case 0x02:      printf("检测没有手指\r\n");                       break;
        case 0x03:      printf("录入指纹失败\r\n");                       break;
        case 0x04:      printf("图像太干,生成特征失败\r\n");          break;
        case 0x05:      printf("图像太湿,生成特征失败\r\n");          break;
        case 0x06:      printf("图像太乱,生成特征失败\r\n");          break;
        case 0x07:      printf("特征太少,生成特征失败\r\n");          break;
        case 0x08:      printf("指纹不匹配\r\n");                        break;
        case 0x09:      printf("没有搜索到指纹\r\n");                  break;
        case 0x0A:      printf("特征合并失败\r\n");                       break;
        case 0x0B:      printf("指纹库地址序号超出指纹库范围\r\n");       break;
        case 0x0C:      printf("从指纹库读出模板出错\r\n");               break;
        case 0x0D:      printf("上传特征错误\r\n");                       break;
        case 0x0E:      printf("模块不能接受后续数据包\r\n");          break;
        case 0x0F:      printf("上传图像失败\r\n");                       break;
        case 0x10:      printf("删除模板失败\r\n");                       break;
        case 0x11:      printf("清空指纹库失败\r\n");                  break;
        case 0x12:      printf("不能进入低功耗状态\r\n");                break;
        case 0x13:      printf("口令不正确\r\n");                        break;
        case 0x14:      printf("系统复位失败\r\n");                       break;
        case 0x15:      printf("缓冲区内没有有效原始图而生不成图像\r\n");            break;
        case 0x16:      printf("在线升级失败\r\n");                       break;
        case 0x17:      printf("残留指纹或两次采集之间手指没有移动过\r\n");           break;
        case 0x18:      printf("读写 FLASH 出错\r\n");                  break;
        case 0xF0:      printf("有后续数据包的指令，正确接收后用 0xf0 应答\r\n"); break;
        case 0xF1:      printf("有后续数据包的指令，命令包用 0xf1 应答\r\n");       break;
        case 0xF2:      printf("烧写内部 FLASH 时，校验和错误\r\n");   break;
        case 0xF3:      printf("烧写内部 FLASH 时，包标识错误\r\n");   break;
        case 0xF4:      printf("烧写内部 FLASH 时，包长度错误\r\n");   break;
        case 0xF5:      printf("烧写内部 FLASH 时，代码长度太长\r\n");  break;
        case 0xF6:      printf("烧写内部 FLASH 时，烧写 FLASH 失败\r\n");         break;
        case 0x19:      printf("未定义错误\r\n");                        break;
        case 0x1A:      printf("无效寄存器号\r\n");                       break;
        case 0x1B:      printf("寄存器设定内容错误号\r\n");               break;
        case 0x1C:      printf("记事本页码指定错误\r\n");                break;
        case 0x1D:      printf("端口操作失败\r\n");                       break;
        case 0x1E:      printf("自动注册（enroll）失败\r\n");           break;
        case 0x20:      printf("指纹库满\r\n");                         break;
    }
}
#endif


/***************************************************************************
描述: 检测手指是否在模块中
参数: wait_time:等待的时间 一次:10ms
返回: 0:手指在   1:不在
****************************************************************************/
uint8_t as608_detection_finger(uint16_t wait_time)
{
    //检测手指是否在模块中
    while(wait_time--)
    {
        Delay_Ms(10);
        if(finger_status == FINGER_EXIST)
        {
            finger_status = FINGER_NO_EXIST;
            return 0;
        }
    }
    return 1;
}



/***************************************************************************
描述: 校验数据是否正确
参数: data:数据包
ID_OR_NUM: 如果不是搜索指令或者查找模板个数设置为NULL 否则存放ID号或者NUM个数
返回: 数据错误返回0XFF   否则 返回接收结果
****************************************************************************/
uint8_t as608_detection_checknum(char *data , uint16_t *ID_OR_NUM)
{
    //包标识位置:6   包长度位置:7、8
    uint8_t packet_length;  //包长度
    uint8_t checksum1 = 0;      //数据包校验和
    uint8_t checksum2 = 0;      //计算出的校验和
    uint8_t i;

    packet_length = (data[7]*10) + data[8]; //获取数据包长度
    if(packet_length == 0) return 0XFF;     //如果无长度 返回错误

    checksum1 = (data[6+packet_length+1]*10) + data[6+packet_length+2]; //数据包校验和

    //自己校验的校验和
    for(i=0;i<packet_length+1;i++)
    {
        checksum2 += data[i+6];
    }
    //匹配校验和是否准确
    if(checksum1 == checksum2)
    {
        //如果是搜索指令 ID进行保存 如果是查找模板个数 NUM进行保存
        if(ID_OR_NUM != NULL)
            *ID_OR_NUM = (data[10]*10) + data[11]; //获取指纹ID号/模板个数

        printf("数据校验和匹配 确认码:%#X\r\n",data[9]);
        return data[9];     //返回结果

    }
    printf("数据校验和错误\r\n");
    return 0XFF;
}



/***************************************************************************
描述: 检测串口返回的数据
参数: wait_time:等待的时间 一次:10ms
ID_OR_NUM: 如果不是搜索指令或者查找模板个数设置为NULL 否则存放ID号或者NUM个数
返回: 无效数据返回0XFF  否则返回结果
****************************************************************************/
uint8_t as608_detection_data(uint16_t wait_time,uint16_t *ID_OR_NUM)
{
    char *data;
    uint8_t result = 0XFF;  //存放结果
    uint16_t temp;

    while(wait_time--)
    {
        Delay_Ms(10);
        //匹配数据帧头
        data = strstr((char *)as608_receive_data,(char *)check_head);
        if(data != NULL)
        {
            result = as608_detection_checknum(data, &temp);
            *ID_OR_NUM=temp;
            break;
        }
    }
    CLOSE_UART8_RECEIVE;           //禁止串口接收
    return result;
}


/***************************************************************************
描述: 添加指纹 函数
返回: 0: 录入指纹成功   1: 录入指纹失败
函数指针：1：请放置手指    2：请再放置手指
****************************************************************************/
uint8_t as608_add_fingerprint(uint16_t PageID,void (*p)(uint8_t))
{
    uint8_t result;                 //录入的结果
    uint8_t add_stage = 1;          //录入的阶段

    while(add_stage != EXIT)
    {
        switch(add_stage)
        {
            //第一阶段 获取第一次指纹图像 并且生成特征图
            case 1:
                printf("请放置手指\r\n");
                p(1);
                if(as608_detection_finger(800)) return 0x02;    //等待手指按下
                result = PS_GetImage(); //获取指纹图像
                if(result) {as608_output_result_message(result);return 1;}
                result = PS_GenChar(CharBuffer1);//生成特征图
                if(result) {as608_output_result_message(result);return 1;}
                add_stage = 2;      //跳转到第二阶段
                break;

            //第二阶段 获取第二次指纹图像 并且生成特征图
            case 2:
                printf("请再放置手指\r\n");
                p(2);
                if(as608_detection_finger(800)) return 0x02;    //等待手指按下
                result = PS_GetImage(); //获取指纹图像
                if(result)  {as608_output_result_message(result);return 1;}
                result = PS_GenChar(CharBuffer2);//生成特征图
                if(result)  {as608_output_result_message(result);return 1;}
                add_stage = 3;  //跳转到第三阶段
                break;

            //第三阶段 比对两枚指纹特征
            case 3:
                result = PS_Match();//比对两枚指纹特征
                if(result)  {as608_output_result_message(result);return 1;}
                add_stage = 4;      //跳转到第四阶段
                break;

            //第四阶段 特征合并生成模板
            case 4:
                result = PS_RegModel();//特征合并生成模板
                if(result)  {as608_output_result_message(result);return 1;}
                add_stage = 5;      //跳转到第五阶段
                break;

            //第五阶段 储存模板
            case 5:
                result = PS_StoreChar(CharBuffer2,PageID);//储存模板
                if(result)  {as608_output_result_message(result);return 1;}
                add_stage = EXIT;
                printf("录制指纹成功\r\n");
                break;
        }
    }

    return 0;

}

/***************************************************************************
描述: 验证指纹 函数
返回: 需要先调用检测指纹函数,验证指纹失败返回2，验证指纹成功返回0
****************************************************************************/
uint16_t as608_verify_fingerprint(void)
{
    uint8_t result;             //存放结果
    uint8_t verify_stage = 1;   //验证的阶段
    uint16_t ID = 1;            //存放指纹的ID号

    while(verify_stage != EXIT)
    {
        switch(verify_stage)
        {
            //第一阶段 获取指纹图像
            case 1:
                printf("阶段一\n");
                // if(as608_detection_finger(500)) {
                //     printf("返回1\n");
                //     return 1;
                //     }    //等待手指按下,如果返回值为1，表示无手指按下
                printf("获取指纹图像\n");
                result = PS_GetImage(); //获取指纹图像
                if(result)  {as608_output_result_message(result);verify_stage = EXIT;}
                verify_stage = 2;
                break;

            //第二阶段 生成特征图
            case 2:
                printf("阶段二\n");
                result = PS_GenChar(CharBuffer1);//生成特征图
                if(result)  {as608_output_result_message(result);verify_stage = EXIT;}
                verify_stage = 3;
                break;


            //第三阶段 高速搜索
            case 3:
                printf("阶段三\n");
                result = PS_HighSpeedSearch(CharBuffer1,0,99,ID);
                if(result)  {as608_output_result_message(result);ID = 0xff;}
                verify_stage = EXIT;
                break;
        }
    }
    if(ID == 0xff)
    {
        printf("验证指纹失败\r\n");
        return 2;     //验证指纹失败返回2
    }
    else
        printf("验证指纹成功\r\n");
    return 0;          //验证指纹成功返回0
}



/***************************************************************************
描述: 删除指纹 函数
返回: 0: 删除指纹成功   1: 删除指纹失败
****************************************************************************/
uint8_t as608_delete_fingerprint(void)
{
    uint8_t result;                 //存放结果
    uint16_t ID;                    //存放ID号

    //1-识别要删除的指纹 如果没有这个指纹 则返回1
    ID = as608_verify_fingerprint();
    if(ID == 0X00)
    {
        printf("删除指纹失败-没有该指纹\r\n");
        return 1;
    }
    //2-针对ID号码进行删除
    result = PS_DeletChar(ID,1);        //删除指纹 ID号
    if(result)
    {
        as608_output_result_message(result);
        printf("删除指纹失败 ID:%d\r\n",ID);
    }

    else
        printf("删除指纹成功 ID:%d\r\n",ID);
    return 0;
}


/***************************************************************************
描述: 清空所有指纹 函数
返回: 0: 清空所有指纹成功 1: 清空所有指纹失败
****************************************************************************/
uint8_t as608_empty_all_fingerprint(void)
{
    uint8_t result;     //存放结果

    result = PS_Empty();    //删除所有指纹
    switch(result)
    {
        case 0x00:
            printf("清空所有指纹成功\r\n");
            break;
        case 0x01:
            printf("清空所有指纹失败-收包错误\r\n");
            break;
        case 0x11:
            printf("清空所有指纹失败-清空失败\r\n");
            break;
    }
    if(result) result = 1;
    return result;
}

/***************************************************************************
描述: 查找 指纹个数 函数
返回: 查找到的个数
****************************************************************************/
uint8_t as608_find_fingerprints_num(void)
{
    uint8_t result;
    uint16_t NUM;       //存放指纹个数（模板个数）
    result = PS_ValidTempleteNum(&NUM);
    if(result)
        printf("查找指纹个数失败 NUM:%d\r\n",NUM);
    else
        printf("查找指纹个数成功 NUM:%d\r\n",NUM);
    return NUM;
}
