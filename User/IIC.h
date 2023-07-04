#ifndef __IIC_H
#define __IIC_H

#include "debug.h"

void IIC_Init( u32 bound , u16 address );
u8 IIC_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);

u8 IIC_ReadByte(u8 addr,u8 reg);
//ÏÂÁĞº¯Êı´ı²âÊÔ
u8 IIC_WriteLen(u8 addr, u8 reg, u8 len, u8 *buf);
u8 IIC_ReadLen(u8 addr, u8 reg, u8 len, u8 *buf);
#endif
