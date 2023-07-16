#include "CH9141.h"

/* Global define */
#define CH9141_RXBUF_SIZE 1024     // DMA buffer size
#define CH9141_size(a)   (sizeof(a) / sizeof(*(a)))

/* Global Variable */
u8 CH9141_TxBuffer[] = " ";
u8 CH9141_RxBuffer[CH9141_RXBUF_SIZE]={0};


/*******************************************************************************
* Function Name  : USARTx_CFG
* Description    : Initializes the USART peripheral.
* ����    ��   ���ڳ�ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void CH9141_USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    //����ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* USART7 TX-->C2  RX-->C3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //RX����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;                    // ������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;     // ����λ 8
    USART_InitStructure.USART_StopBits = USART_StopBits_1;          // ֹͣλ 1
    USART_InitStructure.USART_Parity = USART_Parity_No;             // ��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //ʹ�� RX �� TX

    USART_Init(UART7, &USART_InitStructure);
    DMA_Cmd(DMA2_Channel9, ENABLE);                                  //�������� DMA
    USART_Cmd(UART7, ENABLE);                                        //����UART
}

/*******************************************************************************
* Function Name  : DMA_INIT
* Description    : Configures the DMA.
* ����    ��   DMA ��ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void CH9141_DMA_INIT(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    // TX DMA ��ʼ��
    DMA_DeInit(DMA2_Channel8);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART7->DATAR);        // DMA �����ַ����ָ���Ӧ������
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)CH9141_TxBuffer;                   // DMA �ڴ��ַ��ָ���ͻ��������׵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ���� : ���� ��Ϊ �յ㣬�� �ڴ� ->  ����
    DMA_InitStructure.DMA_BufferSize = 0;                                   // ��������С,��ҪDMA���͵����ݳ���,Ŀǰû�����ݿɷ�
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ����������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ��ַ����������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ��������λ����8λ(Byte)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ�����λ����8λ(Byte)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ��ͨģʽ�������������ѭ������
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // M2P,����M2M
    DMA_Init(DMA2_Channel8, &DMA_InitStructure);

    // RX DMA ��ʼ�������λ������Զ�����
    DMA_DeInit(DMA2_Channel9);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART7->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)CH9141_RxBuffer;                   // ���ջ�����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // ���� : ���� ��Ϊ Դ���� �ڴ� <- ����
    DMA_InitStructure.DMA_BufferSize = CH9141_RXBUF_SIZE;                          // ����������Ϊ CH9141_RXBUF_SIZE
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                         // ѭ��ģʽ�����ɻ��λ�����
    DMA_Init(DMA2_Channel9, &DMA_InitStructure);

}


/*******************************************************************************
* Function Name  : GPIO_CFG
* Description    : Initializes GPIOs.
* ����    ��   GPIO����ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void CH9141_GPIO_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    // CH9141 �������ų�ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /* BLE_sleep --> C13  BLE_AT-->A7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  :  uartWriteBLE
* Description    :  send data to BLE via UART7          ������ģ�鷢������
* Input          :  char * data          data to send   Ҫ���͵����ݵ��׵�ַ
*                   uint16_t num         number of data ���ݳ���
* Return         :  RESET                UART7 busy,failed to send  ����ʧ��
*                   SET                  send success               ���ͳɹ�
*******************************************************************************/
FlagStatus CH9141_uartWriteBLE(char * data , uint16_t num)
{
    //���ϴη���δ��ɣ�����
    if(DMA_GetCurrDataCounter(DMA2_Channel8) != 0){
        return RESET;
    }

    DMA_ClearFlag(DMA2_FLAG_TC8);
    DMA_Cmd(DMA2_Channel8, DISABLE );           // �� DMA �����
    DMA2_Channel8->MADDR = (uint32_t)data;      // ���ͻ�����Ϊ data
    DMA_SetCurrDataCounter(DMA2_Channel8,num);  // ���û���������
    DMA_Cmd(DMA2_Channel8, ENABLE);             // �� DMA
    return SET;
}

/*******************************************************************************
* Function Name  :  uartWriteBLEstr
* Description    :  send string to BLE via UART7    ������ģ�鷢���ַ���
* Input          :  char * str          string to send
* Return         :  RESET                UART7 busy,failed to send  ����ʧ��
*                   SET                  send success               ���ͳɹ�
*******************************************************************************/
FlagStatus CH9141_uartWriteBLEstr(char * str)
{
    uint16_t num = 0;
    while(str[num])num++;           // �����ַ�������
    return CH9141_uartWriteBLE(str,num);
}


/*******************************************************************************
* Function Name  :  uartReadBLE
* Description    :  read some bytes from receive buffer �ӽ��ջ���������һ������
* Input          :  char * buffer        buffer to storage the data ������Ŷ������ݵĵ�ַ
*                   uint16_t num         number of data to read     Ҫ�����ֽ���
* Return         :  int                  number of bytes read       ����ʵ�ʶ������ֽ���
*******************************************************************************/
uint16_t CH9141_rxBufferReadPos = 0;       //���ջ�������ָ��
uint32_t CH9141_uartReadBLE(char * buffer , uint16_t num)
{
    uint16_t CH9141_rxBufferEnd = CH9141_RXBUF_SIZE - DMA_GetCurrDataCounter(DMA2_Channel9); //���� DMA ����β��λ��
    uint16_t i = 0;

    if (CH9141_rxBufferReadPos == CH9141_rxBufferEnd){
        // �����ݣ�����
        return 0;
    }

    while (CH9141_rxBufferReadPos!=CH9141_rxBufferEnd && i < num){
        buffer[i] = CH9141_RxBuffer[CH9141_rxBufferReadPos];
        i++;
        CH9141_rxBufferReadPos++;
        if(CH9141_rxBufferReadPos >= CH9141_RXBUF_SIZE){
            // ����������������
            CH9141_rxBufferReadPos = 0;
        }
    }
    return i;
}

/*******************************************************************************
* Function Name  :  uartReadByteBLE
* Description    :  read one byte from UART buffer  �ӽ��ջ��������� 1 �ֽ�����
* Input          :  None
* Return         :  char    read data               ���ض���������(������Ҳ����0)
*******************************************************************************/
char CH9141_uartReadByteBLE()
{
    char ret;
    uint16_t CH9141_rxBufferEnd = CH9141_RXBUF_SIZE - DMA_GetCurrDataCounter(DMA2_Channel9);//���� DMA ����β��λ��
    if (CH9141_rxBufferReadPos == CH9141_rxBufferEnd){
        // �����ݣ�����
        return 0;
    }
    ret = CH9141_RxBuffer[CH9141_rxBufferReadPos];
    CH9141_rxBufferReadPos++;
    if(CH9141_rxBufferReadPos >= CH9141_RXBUF_SIZE){
        // ����������������
        CH9141_rxBufferReadPos = 0;
    }
    return ret;
}
/*******************************************************************************
* Function Name  :  uartAvailableBLE
* Description    :  get number of bytes Available to read from the UART buffer  ��ȡ�������пɶ����ݵ�����
* Input          :  None
* Return         :  uint16_t    number of bytes Available to readd              ���ؿɶ���������
*******************************************************************************/
uint16_t CH9141_uartAvailableBLE()
{
    uint16_t CH9141_rxBufferEnd = CH9141_RXBUF_SIZE - DMA_GetCurrDataCounter(DMA2_Channel9);//���� DMA ����β��λ��
    // ����ɶ��ֽ�
    if (CH9141_rxBufferReadPos <= CH9141_rxBufferEnd){
        return CH9141_rxBufferEnd - CH9141_rxBufferReadPos;
    }else{
        return CH9141_rxBufferEnd +CH9141_RXBUF_SIZE -CH9141_rxBufferReadPos;
    }
}

void CH9141_Init(void)
{
    CH9141_DMA_INIT();
    CH9141_USARTx_CFG();                                                 /* USART INIT */
    USART_DMACmd(UART7,USART_DMAReq_Tx|USART_DMAReq_Rx,ENABLE);

    CH9141_GPIO_CFG();
        GPIO_WriteBit(GPIOA, GPIO_Pin_7,RESET); //���� AT
        GPIO_WriteBit(GPIOC, GPIO_Pin_13,SET); //enable CH9141
        Delay_Ms(1000);

        //���ڿ��� 0.5s ����  "AT...\r\n" ����ֱ�ӽ��� AT ģʽ
                //uartWriteBLEstr("AT...\r\n");

                // AT ָ�������һ�����
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+TPL?\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+NAME?\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+BLESTA?\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+BLEMODE=2\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+BLEMODE?\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+MAC?\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+PASEN=ON\r\n")==RESET);
                Delay_Ms(100);
                while(CH9141_uartWriteBLEstr("AT+CONNINTER?\r\n")==RESET);
                Delay_Ms(100);
    //          while(uartWriteBLEstr("AT+EXIT\r\n")==RESET);
    //          Delay_Ms(100);
                int num = CH9141_uartAvailableBLE();
                if (num > 0 ){
                    char buffer[1024]={"\0"};
                    CH9141_uartReadBLE(buffer , num);
                    printf("Revceived:%s\r\n",buffer);
                }
                GPIO_WriteBit(GPIOA, GPIO_Pin_7,SET); // �˳�AT�������ֻ����������CH9141,���������շ�

}