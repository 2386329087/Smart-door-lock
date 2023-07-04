#include "IIC.h"
#include "debug.h"



/*******************************************************************************
* Function Name  : IIC_Init
* Description    : Initializes the IIC peripheral.
* Input          : None
* Return         : None
*******************************************************************************/
void IIC_Init( u32 bound , u16 address )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitTSturcture;
	//I2C2_SCL-> PB10 , I2C2_SDA->P11
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE );
//	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C2, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	I2C_InitTSturcture.I2C_ClockSpeed = bound;
	I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
	I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
	I2C_InitTSturcture.I2C_OwnAddress1 = address;
	I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
	I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init( I2C2, &I2C_InitTSturcture );

	I2C_Cmd( I2C2, ENABLE );

	I2C_AcknowledgeConfig( I2C2, ENABLE );
}

/*******************************************************************************
* Function Name  : IIC_WaitEvent
* Description    : wait IIC event,with timeout (65535 times).
* Input          : None
* Return         : 0 - success
*                  1 - timeout
*******************************************************************************/
u8 IIC_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT){
    u16 counter = 0xffff;
    while( !I2C_CheckEvent( I2Cx, I2C_EVENT ) ){
        counter--;
        if(counter == 0){
            return 1;
        }
    }
    return 0;
}



// IIC连续写
// addr:器件地址
// reg:寄存器地址
// len:写入长度
// buf:数据区
//返回值:0,正常
//      1 - 超时
//     其他,错误代码
u8 IIC_WriteLen(u8 addr, u8 reg, u8 len, u8 *buf)
{
    u8 i = 0;

    I2C_AcknowledgeConfig(I2C2, ENABLE);
    I2C_GenerateSTART(I2C2, ENABLE);

    if(IIC_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
        return 1;
    I2C_Send7bitAddress(I2C2, (addr<<1) | 0, I2C_Direction_Transmitter); // 发送器件地址+写命令

    if(IIC_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        return 1; // 等待应答

    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == RESET)
        ;
    I2C_SendData(I2C2, reg); // 写寄存器地址

    while (i < len)
    {
        if (I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) != RESET)
        {
            I2C_SendData(I2C2, buf[i]); // 发送数据
            i++;
        }
    }
    //    if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) )return 1;
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE) == RESET)
        ;

    I2C_GenerateSTOP(I2C2, ENABLE);

    return 0;
}

// IIC连续读
// addr:器件地址
// reg:要读取的寄存器地址
// len:要读取的长度
// buf:读取到的数据存储区
//返回值:0,正常
//      1 - 超时
//     其他,错误代码
u8 IIC_ReadLen(u8 addr, u8 reg, u8 len, u8 *buf)
{
    u8 i = 0;

    I2C_AcknowledgeConfig(I2C2, ENABLE);
    I2C_GenerateSTART(I2C2, ENABLE);

    if(IIC_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))return 1       ;
    I2C_Send7bitAddress(I2C2, (addr << 1) | 0X00, I2C_Direction_Transmitter); //发送器件地址+写命令

    if(IIC_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1       ; //等待应答

    I2C_SendData(I2C2, reg); //写寄存器地址

    I2C_GenerateSTART(I2C2, ENABLE);
    if(IIC_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))return 1       ;

    I2C_Send7bitAddress(I2C2, ((addr << 1) | 0x01), I2C_Direction_Receiver); //发送器件地址+读命令
    if(IIC_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))return 1       ; //等待应答

    while (i < len)
    {
        if (I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) != RESET)
        {
            if (i == (len - 1))
            {
                I2C_AcknowledgeConfig(I2C2, DISABLE);
                buf[i] = I2C_ReceiveData(I2C2); //读数据,发送nACK
            }
            else
            {
                buf[i] = I2C_ReceiveData(I2C2); //读数据,发送ACK
            }
            i++;
        }
    }

    I2C_GenerateSTOP(I2C2, ENABLE); //产生一个停止条件

    return 0;
}


/**
 * @brief   IIC读一个字节
 *
 * @param   reg     寄存器地址
 *
 * @return  u8      读到的数据
 */
u8 IIC_ReadByte(u8 addr,u8 reg)
{
    u8 res;

    I2C_AcknowledgeConfig( I2C2, ENABLE );

    I2C_GenerateSTART( I2C2, ENABLE );

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress(I2C2,(addr << 1) | 0X00,I2C_Direction_Transmitter); //发送器件地址+写命令

    if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )return 1;  //等待应答
    I2C_SendData(I2C2,reg);         //写寄存器地址

    I2C_GenerateSTART( I2C2, ENABLE );
    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );

    I2C_Send7bitAddress(I2C2,((addr << 1) | 0x01),I2C_Direction_Receiver);//发送器件地址+读命令
    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) ); //等待应答

    I2C_AcknowledgeConfig( I2C2, DISABLE );

    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_RXNE ) ==  RESET );
    res = I2C_ReceiveData( I2C2 ); //读数据,发送nACK


    I2C_GenerateSTOP( I2C2, ENABLE );//产生一个停止条件
    return res;
}


