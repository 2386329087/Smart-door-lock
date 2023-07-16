#include "esp8266.h"
#include "string.h"

/* Global define */
#define RXBUF_SIZE 1024 // DMA buffer size
#define size(a)   (sizeof(a) / sizeof(*(a)))
/* Global Variable */
u8 TxBuffer[] = " ";
u8 RxBuffer[RXBUF_SIZE]={0};

TDateTime TIME;
char timeout = 1;          //对时标志位
/*******************************************************************************
* Function Name  : USARTx_CFG
* Description    : Initializes the USART6 peripheral.
* 描述    ：   串口初始化
* Input          : None
* Return         : None
*******************************************************************************/
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    //开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART6, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* USART6 TX-->C0  RX-->C1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //RX，输入上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;                    // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;     // 数据位 8
    USART_InitStructure.USART_StopBits = USART_StopBits_1;          // 停止位 1
    USART_InitStructure.USART_Parity = USART_Parity_No;             // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //使能 RX 和 TX

    USART_Init(UART6, &USART_InitStructure);
    DMA_Cmd(DMA2_Channel7, ENABLE);                                  //开启接收 DMA
    USART_Cmd(UART6, ENABLE);                                        //开启UART
}

/*******************************************************************************
* Function Name  : DMA_INIT
* Description    : Configures the DMA.
* 描述    ：   DMA 初始化
* Input          : None
* Return         : None
*******************************************************************************/
void DMA_INIT(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    // TX DMA 初始化
    DMA_DeInit(DMA2_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART6->DATAR);        // DMA 外设基址，需指向对应的外设
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)TxBuffer;                   // DMA 内存基址，指向发送缓冲区的首地址
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

    // RX DMA 初始化，环形缓冲区自动接收
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxBuffer;                   // 接收缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // 方向 : 外设 作为 源，即 内存 <- 外设
    DMA_InitStructure.DMA_BufferSize = RXBUF_SIZE;                          // 缓冲区长度为 RXBUF_SIZE
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                         // 循环模式，构成环形缓冲区
    DMA_Init(DMA2_Channel7, &DMA_InitStructure);
}



/*******************************************************************************
* Function Name  :  uartWriteWiFi
* Description    :  send data to ESP8266 via UART6
* 描述    ：   向 WiFi 模组发送数据
* Input          :  char * data          data to send   要发送的数据的首地址
*                   uint16_t num         number of data 数据长度
* Return         :  RESET                UART6 busy,failed to send  发送失败
*                   SET                  send success               发送成功
*******************************************************************************/
FlagStatus uartWriteWiFi(char * data , uint16_t num)
{
    //如上次发送未完成，返回
    if(DMA_GetCurrDataCounter(DMA2_Channel6) != 0){
        return RESET;
    }

    DMA_ClearFlag(DMA2_FLAG_TC8);
    DMA_Cmd(DMA2_Channel6, DISABLE );           // 关 DMA 后操作
    DMA2_Channel6->MADDR = (uint32_t)data;      // 发送缓冲区为 data
    DMA_SetCurrDataCounter(DMA2_Channel6,num);  // 设置缓冲区长度
    DMA_Cmd(DMA2_Channel6, ENABLE);             // 开 DMA
    return SET;
}

/*******************************************************************************
* Function Name  :  uartWriteWiFiStr
* Description    :  send string to ESP8266 via UART6    向 WiFi 模组发送字符串
* Input          :  char * str          string to send
* Return         :  RESET                UART busy,failed to send   发送失败
*                   SET                  send success               发送成功
*******************************************************************************/
FlagStatus uartWriteWiFiStr(char * str)
{
    uint16_t num = 0;
    while(str[num])num++;           // 计算字符串长度
    return uartWriteWiFi(str,num);
}


/*******************************************************************************
* Function Name  :  uartReadWiFireceive
* Description    :  read some bytes from receive buffer 从接收缓冲区读出一组数据
* Input          :  char * buffer        buffer to storage the data 用来存放读出数据的地址
*                   uint16_t num         number of data to read     要读的字节数
* Return         :  int                  number of bytes read       返回实际读出的字节数
*******************************************************************************/
uint16_t rxBufferReadPos = 0;       //接收缓冲区读指针
uint32_t uartReadWiFi(char * buffer , uint16_t num)
{
    uint16_t rxBufferEnd = RXBUF_SIZE - DMA_GetCurrDataCounter(DMA2_Channel7); //计算 DMA 数据尾的位置
    uint16_t i = 0;
    if (rxBufferReadPos == rxBufferEnd){
        // 无数据，返回
        return 0;
    }

    while (rxBufferReadPos!=rxBufferEnd && i < num){
        buffer[i] = RxBuffer[rxBufferReadPos];
        i++;
        rxBufferReadPos++;
        if(rxBufferReadPos >= RXBUF_SIZE){
            // 超出缓冲区，回零
            rxBufferReadPos = 0;
        }
    }
    return i;
}

/*******************************************************************************
* Function Name  :  uartReadByteWiFi
* Description    :  read one byte from UART buffer  从接收缓冲区读出 1 字节数据
* Input          :  None
* Return         :  char    read data               返回读出的数据(无数据也返回0)
*******************************************************************************/
char uartReadByteWiFi()
{
    char ret;
    uint16_t rxBufferEnd = RXBUF_SIZE - DMA_GetCurrDataCounter(DMA2_Channel7);
    if (rxBufferReadPos == rxBufferEnd){
        // 无数据，返回
        return 0;
    }
    ret = RxBuffer[rxBufferReadPos];
    rxBufferReadPos++;
    if(rxBufferReadPos >= RXBUF_SIZE){
        // 超出缓冲区，回零
        rxBufferReadPos = 0;
    }
    return ret;
}
/*******************************************************************************
* Function Name  :  uartAvailableWiFi
* Description    :  get number of bytes Available to read from the UART buffer  获取缓冲区中可读数据的数量
* Input          :  None
* Return         :  uint16_t    number of bytes Available to read               返回可读数据数量
*******************************************************************************/
uint16_t uartAvailableWiFi()
{
    uint16_t rxBufferEnd = RXBUF_SIZE - DMA_GetCurrDataCounter(DMA2_Channel7);//计算 DMA 数据尾的位置
    // 计算可读字节
    if (rxBufferReadPos <= rxBufferEnd){
        return rxBufferEnd - rxBufferReadPos;
    }else{
        return rxBufferEnd +RXBUF_SIZE -rxBufferReadPos;
    }
}

/********************************************************************
* 函 数 名       : TIM6_Init
* 函数功能    : 初始化 定时器 TIM6
* 输    入          : arr：计数值，psc 预分频系数
* 输    出          : 无
********************************************************************/
void TIM6_Init( u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM6, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInit( TIM6, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_ARRPreloadConfig( TIM6, ENABLE );
    TIM_Cmd( TIM6, ENABLE );

    NVIC_InitTypeDef NVIC_InitStructure={0};
       NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn ;
       NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
       NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级
       NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
       NVIC_Init(&NVIC_InitStructure);
}

/********************************************************************
* 函 数 名       : system_time_increase
* 函数功能    :时间进位
* 输    入          : 无
* 输    出          : 无
********************************************************************/
void system_time_increase(void)
{
    TIME.msec++;
    if(TIME.msec >= 1)  //0.5s进入中断一次，2次就是1s
    {
        TIME.msec = 0;
        TIME.second++;//秒进位
        if(TIME.second >= 60)
        {
            TIME.second = 0;
            TIME.minute++;//分钟进位
            if(TIME.minute >= 60)
            {
                TIME.minute = 0;
                TIME.hour++;//小时进位
                if(TIME.hour >= 24)
                {
                    TIME.hour = 0;
                    timeout = 1;     //一天结束对一次时，并且把日期换好
                }
            }
        }
    }
}

/********************************************************************
* 函 数 名      : get_true_time
* 函数功能    :时间矫正
* 输    入       : *Time,*Date,*Week
*注     释       :此为数组指针，数组长度建议偏大设置
********************************************************************/
void get_true_time(char *Time,char *Date,char *Week)
{
    char *begin1;                       //日期与时间的起始地址
    char *begin2;                       //星期的起始地址
    char *end2;                         //星期的结束地址
    Time[100]='\0';              //时间字符串数组
    Date[100]='\0';              //日期字符串数组
    Week[100]='\0';              //星期字符串数组
    //访问该网站会反馈时间数据
    while(uartWriteWiFiStr("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json\r\n")==RESET);
    Delay_Ms(500);
    int num = uartAvailableWiFi();
    if (num > 0 ){
        char buffer[1024]={"\0"};
        uartReadWiFi(buffer , num);                //读取数据
        printf("Revceived:\r\n%s",buffer);
        begin1 =strstr(buffer,"datetime_1");
        strncpy(Date,begin1+13,10);  //日期
        Date[10] = '\0';
        strncpy(Time,begin1+24,8);   //时间
        Time[10] = '\0';
        begin2 =strstr(buffer,"week_4"); //星期
        strcpy(Week,begin2+9);
        end2 =strstr(Week,"\"");
        *end2 = '\0';
        printf("date:\r\n%s %s %s\n",Date,Time,Week);   //在串口显示正确的时间
        printf("\n");
        TIME.hour = ((*(begin1+24) - 0x30)*10 + (*(begin1+25) - 0x30)) % 24;                    //把小时由字符串转为十进制数
        TIME.minute = ((*(begin1+27) - 0x30)*10 + (*(begin1+28) - 0x30)) % 60;                  //把分钟由字符串转为十进制数
        TIME.second = ((*(begin1+30) - 0x30)*10 + (*(begin1+31) - 0x30)) % 60;                  //把秒由字符串转为十进制数
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Return         : None
*******************************************************************************/
void esp8266_Init(char *ID,char *Password)
{
    char WIFI_change[]="AT+CWJAP=\"Li\",\"15813991772\"\r\n";
    sprintf(WIFI_change,"AT+CWJAP=\"%s\",\"%s\"\r\n",ID,Password);
    printf("%s\r\n",WIFI_change);

    DMA_INIT();
    USARTx_CFG();                                                 /* USART INIT */
    USART_DMACmd(UART6,USART_DMAReq_Tx|USART_DMAReq_Rx,ENABLE);
//    TIM6_Init( 5000-1, 14400-1 );

    // 退出透传
    while (uartWriteWiFi("+++", 3));
    Delay_Ms(100);
    //断开与服务器的连接
    while(uartWriteWiFiStr("AT+CIPCLOSE\r\n")==RESET);
    Delay_Ms(100);
    // 查询 WiFi 模块是否正常工作
    uartWriteWiFi("AT\r\n",4);
    Delay_Ms(100);
    // 查询 打开AT回显
    uartWriteWiFi("ATE1\r\n",6);
    Delay_Ms(100);
    // 设为 Station 模式
    while(uartWriteWiFiStr("AT+CWMODE=1\r\n")==RESET);
    Delay_Ms(100);
    // 设为单连接模式
    while(uartWriteWiFiStr("AT+CIPMUX=0\r\n")==RESET);
    Delay_Ms(100);
    // 连接一个名为 SSID、密码为 PASSWORD 的 WiFi 网络，
    while(uartWriteWiFiStr(WIFI_change)==RESET);
    Delay_Ms(4000);
    //打印之前收到的信息
    int num = uartAvailableWiFi();
    if (num > 0 ){
        char buffer[1024]={"\0"};
        uartReadWiFi(buffer , num);
        printf("Revceived:\r\n%s",buffer);
    }
    Delay_Ms(1000);
    //连接服务器
    while(uartWriteWiFiStr("AT+CIPSTART=\"TCP\",\"api.k780.com\",88\r\n")==RESET);
    Delay_Ms(500);
    //开启透传模式
    while(uartWriteWiFiStr("AT+CIPMODE=1\r\n")==RESET);
    Delay_Ms(100);
    //AT+CIPMODE=1并且作为客户端模式下，进入透传模式(需要支持硬件流控，否则大量数据情况下会丢数据)模块收到指令后先换行返回”>”，然后会发送串口接收到的数据。
    while(uartWriteWiFiStr("AT+CIPSEND\r\n")==RESET);
    Delay_Ms(100);
        num = uartAvailableWiFi();
        if (num > 0 ){
            char buffer[1024]={"\0"};
            uartReadWiFi(buffer , num);
            printf("Revceived:\r\n%s",buffer);
        }
}

/***************************************************************************
描述:主从机初始化
参数:  ID:WIFI名称       Password：WIFI密码
返回: 无
****************************************************************************/
void esp8266_Init_2(char *ID,char *Password)
{
    char WIFI_change[]="AT+CWJAP=\"Li\",\"15813991772\"\r\n";
    sprintf(WIFI_change,"AT+CWJAP=\"%s\",\"%s\"\r\n",ID,Password);
    printf("%s\r\n",WIFI_change);
    uint8_t i=0;
    DMA_INIT();
    USARTx_CFG();                                                 /* USART INIT */
    USART_DMACmd(UART6,USART_DMAReq_Tx|USART_DMAReq_Rx,ENABLE);
//    TIM6_Init( 5000-1, 14400-1 );

    // 退出透传
    while (uartWriteWiFi("+++", 3));
    Delay_Ms(100);
    // 查询 WiFi 模块是否正常工作
        uartWriteWiFi("AT\r\n",4);
        Delay_Ms(100);
    //断开与服务器的连接
    while(uartWriteWiFiStr("AT+CIPCLOSE\r\n")==RESET);
    Delay_Ms(100);
    // 查询 WiFi 模块是否正常工作
    uartWriteWiFi("AT\r\n",4);
    Delay_Ms(100);
    // 查询 打开AT回显
    uartWriteWiFi("ATE1\r\n",6);
    Delay_Ms(100);
    // 设为 Station 模式
    while(uartWriteWiFiStr("AT+CWMODE=1\r\n")==RESET);
    Delay_Ms(100);
    // 设为单连接模式
    while(uartWriteWiFiStr("AT+CIPMUX=0\r\n")==RESET);
    Delay_Ms(100);
    // 连接一个名为 SSID、密码为 PASSWORD 的 WiFi 网络，
    while(uartWriteWiFiStr(WIFI_change)==RESET);
    Delay_Ms(4000);
    //打印之前收到的信息
    int num = uartAvailableWiFi();
    if (num > 0 ){
        char buffer[1024]={"\0"};
        uartReadWiFi(buffer , num);
        printf("Revceived:\r\n%s",buffer);
    }
    Delay_Ms(1000);
    //连接服务器
    while(uartWriteWiFiStr("AT+CIPSTART=\"TCP\",\"192.168.1.243\",333\r\n")==RESET);
    Delay_Ms(500);
    while(esp8266_receive_judge("busy") && i<3)
    {
        i++;
        while(uartWriteWiFiStr("AT+CIPSTART=\"TCP\",\"192.168.1.243\",333\r\n")==RESET);
        Delay_Ms(500);
    }
    //开启透传模式
    while(uartWriteWiFiStr("AT+CIPMODE=1\r\n")==RESET);
    Delay_Ms(100);
    //AT+CIPMODE=1并且作为客户端模式下，进入透传模式(需要支持硬件流控，否则大量数据情况下会丢数据)模块收到指令后先换行返回”>”，然后会发送串口接收到的数据。
    while(uartWriteWiFiStr("AT+CIPSEND\r\n")==RESET);
    Delay_Ms(100);
        num = uartAvailableWiFi();
        if (num > 0 ){
            char buffer[1024]={"\0"};
            uartReadWiFi(buffer , num);
            printf("Revceived:\r\n%s",buffer);
        }
}

/***************************************************************************
描述获取DMA缓冲区的数据，比较其中的字符串是否与参数相同
参数:  字符串
返回: 有:1      无:0
****************************************************************************/
uint8_t esp8266_receive_judge(char *str)
{
    char *data;
    int num;
    char buffer[1024]={"\0"};

    Delay_Ms(100);

    num = uartAvailableWiFi();
            if (num > 0 )
            {
                memset(buffer,'\0',sizeof(buffer));
                uartReadWiFi(buffer , num);
                printf("Revceived:\r\n%s",buffer);
            }

            data=strstr(buffer,str);
            if(data != NULL)
            {
                return 1;
            }
     return 0;
}

/********************************************************************
* 函 数 名      : TIM6_IRQHandler
* 函数功能   : 中断服务程序的函数
* 输    入         : 无
* 输    出         : 无
*********************************************************************/
void TIM6_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM6_IRQHandler(void)
{
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);//清除标志位
    system_time_increase();              //时间走动
}
