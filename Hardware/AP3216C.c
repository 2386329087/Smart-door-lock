#include "AP3216C.h"
#include "debug.h"
#include "IIC.h"

/**
 * @brief	初始化AP3216C
 *
 * @param   void
 *
 * @return  u8		0,初始化成功
 *					1,初始化失败
 */
u8 AP3216C_Init(void)
{
    u8 temp = 0;
    IIC_Init(200000,0x02);         				//初始化IIC
    AP3216C_WriteOneByte(0x00, 0X04);	//复位AP3216C
    Delay_Ms(50);						//AP33216C复位至少10ms
    AP3216C_WriteOneByte(0x00, 0X03);	//开启ALS、PS+IR

    temp = AP3216C_ReadOneByte(0X00);		//读取刚刚写进去的0X03

    if(temp == 0X03)return 0;				//AP3216C正常

    else return 1;						//AP3216C失败
}

/**
 * @brief	读取AP3216C的数据,读取原始数据，包括ALS,PS和IR
 *			注意！如果同时打开ALS,IR+PS的话两次数据读取的时间间隔要大于112.5ms
 *
 * @param   ir	红外数据
 * @param   ps	距离数据
 * @param   als	光敏数据
 *
 * @return  void
 */
void AP3216C_ReadData(u16* ir, u16* ps, u16* als)
{
    u8 buf[6];
    u8 i;

    for(i = 0; i < 6; i++)
    {
        buf[i] = AP3216C_ReadOneByte(0X0A + i);		//循环读取所有传感器数据
    }

    if(buf[0] & 0X80)*ir = 0;						//IR_OF位为1,则数据无效

    else *ir = ((u16)buf[1] << 2) | (buf[0] & 0X03); 	//读取IR传感器的数据

    *als = ((u16)buf[3] << 8) | buf[2];				//读取ALS传感器的数据

    if(buf[4] & 0x40)*ps = 0;    					//IR_OF位为1,则数据无效

    else *ps = ((u16)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); //读取PS传感器的数据
}

/**
 * @brief	IIC写一个字节
 *
 * @param   reg		寄存器地址
 * @param   data	要写入的数据
 *
 * @return  u8		0,正常
 *                  1 - 超时
 *                  其他,错误代码
 */
u8 AP3216C_WriteOneByte(u8 reg, u8 data)
{
    u8 res = 0;
    I2C_AcknowledgeConfig( I2C2, ENABLE );

    I2C_GenerateSTART( I2C2, ENABLE );
    while(1){
        if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) ){
            res = 1;
            break;
        }
        I2C_Send7bitAddress(I2C2,((AP3216C_ADDR << 1) | 0),I2C_Direction_Transmitter);//发送器件地址+写命令

        if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ){  //等待应答
            res = 1;
            break;
        }
        I2C_SendData(I2C2,reg);     //写寄存器地址

        if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ){  //等待应答
            res = 1;
            break;
        }

        while( I2C_GetFlagStatus( I2C2, I2C_FLAG_TXE ) ==  RESET );
        I2C_SendData(I2C2,data);      //发送数据

        while( I2C_GetFlagStatus( I2C2, I2C_FLAG_TXE ) ==  RESET );
        break;
    }
    I2C_GenerateSTOP( I2C2, ENABLE );
    return res;
}

/**
 * @brief	IIC读一个字节
 *
 * @param   reg		寄存器地址
 *
 * @return  u8		读到的数据
 */
u8 AP3216C_ReadOneByte(u8 reg)
{
    u8 res;

    I2C_AcknowledgeConfig( I2C2, ENABLE );

    I2C_GenerateSTART( I2C2, ENABLE );

    if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) )return 1;
    I2C_Send7bitAddress(I2C2,(AP3216C_ADDR << 1) | 0X00,I2C_Direction_Transmitter); //发送器件地址+写命令

    if(IIC_WaitEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )return 1;  //等待应答
    I2C_SendData(I2C2,reg);         //写寄存器地址

    I2C_GenerateSTART( I2C2, ENABLE );
    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );

    I2C_Send7bitAddress(I2C2,((AP3216C_ADDR << 1) | 0x01),I2C_Direction_Receiver);//发送器件地址+读命令
    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) ); //等待应答

    I2C_AcknowledgeConfig( I2C2, DISABLE );

    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_RXNE ) ==  RESET );
    res = I2C_ReceiveData( I2C2 ); //读数据,发送nACK


    I2C_GenerateSTOP( I2C2, ENABLE );//产生一个停止条件
    return res;
}

