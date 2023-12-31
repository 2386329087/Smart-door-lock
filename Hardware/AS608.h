#ifndef __AS608_H
#define __AS608_H

#include "debug.h"
#include <string.h>

#define OPEN_UART8_RECEIVE     USART_ITConfig(UART8, USART_IT_RXNE, ENABLE)
#define CLOSE_UART8_RECEIVE    USART_ITConfig(UART8, USART_IT_RXNE, DISABLE)

#define     AS608_WAK_RCC                   RCC_APB2Periph_GPIOE
#define     AS608_WAK_PORT                  GPIOE
#define     AS608_WAK_PIN                   GPIO_Pin_13

#define     AS608_WAK_EXTILINE_PORT         GPIO_PortSourceGPIOE
#define     AS608_WAK_EXTILINE_PIN          GPIO_PinSource13

#define     AS608_WAK_EXTILINE_NUMBER       EXTI_Line13

#define     AS608_WAK_NVIC_CHANNEL          EXTI15_10_IRQn


#define     FINGER_EXIST        1
#define     FINGER_NO_EXIST     0

#define     CLEAR_BUFFER        {memset(as608_receive_data,'\0',sizeof(as608_receive_data));as608_receive_count = 0;}

#define     EXIT        0

#define     CharBuffer1     0x01
#define     CharBuffer2     0x02

extern uint8_t as608_receive_data[128]; //存放指纹返回信息
extern uint8_t as608_receive_count;

extern uint16_t ID;                         //存放ID号 方便进行删除操作
extern uint16_t NUM;                        //存放个数 方便进行删除操作
extern uint8_t finger_status;

extern uint8_t PS_WriteNotepad_code[32];   //存放要发送的记事本数据
extern uint8_t PS_ReadNotepad_code[32];    //存放接受到的记事本数据

void AS608_PIN_Init(void);

void as60x_wak_init(void);

uint8_t as608_detection_finger(uint16_t wait_time);                     //检测手指是否在模块上
uint8_t as608_detection_data(uint16_t wait_time,uint16_t *ID_OR_NUM);  //检测指纹模块返回的数据
uint8_t as608_detection_checknum(char *data,uint16_t *ID_OR_NUM);      //检测数据包检验和是否准确


uint8_t as608_add_fingerprint(uint16_t PageID,void (*p)(uint8_t));      //录入指纹函数
uint16_t as608_verify_fingerprint(void);                               //验证指纹函数
uint8_t as608_delete_fingerprint(void);                             //删除指纹函数
uint8_t as608_empty_all_fingerprint(void);                          //清空所有指纹函数
uint8_t as608_find_fingerprints_num(void);                          //查找指纹个数函数

void Uart8_SendData(uint8_t Data);
uint8_t PS_WriteNotepad(uint8_t *Data);
uint8_t PS_ReadNotepad(uint8_t *Data);

#endif
