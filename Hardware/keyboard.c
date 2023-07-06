#include  "keyboard.h"

void keyboard_Pin_Init(void)
{
    Delay_Init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    /* SCLK-->D10 SDO-->D11*/
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        GPIO_SetBits(GPIOD, GPIO_Pin_10 | GPIO_Pin_11);
}

void SDO_INT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void SDO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
}

uint16_t ttp229_read(void)
{
    uint8_t i;
    uint16_t temp = 0;

    SDO_OUT();
    GPIO_WriteBit(GPIOD, GPIO_Pin_11,RESET);
    Delay_Us(10);
    GPIO_WriteBit(GPIOD, GPIO_Pin_11,SET);
    Delay_Us(10);

    SDO_INT();

    for(i=0;i<16;i++)
    {
    temp>>=1;
    GPIO_WriteBit(GPIOD, GPIO_Pin_10,0);
        if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11))
            temp|=0x8000;
        GPIO_WriteBit(GPIOD, GPIO_Pin_10,1);
    }
return temp;
}

char keyboard__char_scan(void)
{
    uint16_t Data;

    Data=ttp229_read();

            switch(Data)
            {
                case(0x0001) : return '1' ; break;
                case(0x0002) : return '2' ; break;
                case(0x0004) : return '3' ; break;
                case(0x0008) : return 'A' ; break;
                case(0x0010) : return '4' ; break;
                case(0x0020) : return '5' ; break;
                case(0x0040) : return '6' ; break;
                case(0x0080) : return 'B' ; break;
                case(0x0100) : return '7' ; break;
                case(0x0200) : return '8' ; break;
                case(0x0400) : return '9' ; break;
                case(0x0800) : return 'C' ; break;
                case(0x1000) : return '*' ; break;
                case(0x2000) : return '0' ; break;
                case(0x4000) : return '#' ; break;
                case(0x8000) : return 'D' ; break;
            }
            return 'N';                          ////无按键返回
}

uint8_t keyboard_uint_scan(void)
{
    uint16_t Data;

   Data=ttp229_read();

            switch(Data)
            {
                case(0x0001) : return 1 ; break;       //1
                case(0x0002) : return 2 ; break;       //2
               case(0x0004) : return 3 ; break;       //3
                case(0x0008) : return 0x0A ; break;       //A
                case(0x0010) : return 4 ; break;       //4
                case(0x0020) : return 5 ; break;       //5
                case(0x0040) : return 6 ; break;       //6
                case(0x0080) : return 0x0B ; break;       //B
                case(0x0100) : return 7 ; break;       //7
                case(0x0200) : return 8 ; break;       //8
                case(0x0400) : return 9 ; break;       //9
                case(0x0800) : return 0x0C ; break;       //C
                case(0x1000) : return 0x0E ; break;       //*
                case(0x2000) : return 0 ; break;       //0
                case(0x4000) : return 0X0F ; break;       //#
                case(0x8000) : return 0x0D ; break;       //D
            }
            return 0xff;                             //无按键返回
}
