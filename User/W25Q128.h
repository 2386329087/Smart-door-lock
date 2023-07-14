#ifndef __W25Q128_H
#define __W25Q128_H

#include "debug.h"

/* Winbond SPIFalsh ID */
#define W25Q80  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128 0XEF17

/* GigaDevice SPIFalsh ID */
#define GD25Q80  0XC813
#define GD25Q16  0XC814
#define GD25Q32  0XC815
#define GD25Q64  0XC816
#define GD25Q128 0XC817

/* Fudan Micro SPIFalsh ID */
#define FM25x08  0XA113
#define FM25x16  0XA114
#define FM25x32  0XA115
#define FM25x64  0XA116
#define FM25x128 0XA117


/* Winbond SPIFalsh Instruction List */  //详情见Flash芯片数据手册
#define W25X_WriteEnable          0x06
#define W25X_WriteDisable         0x04
#define W25X_ReadStatusReg      0x05
#define W25X_WriteStatusReg     0x01
#define W25X_ReadData               0x03
#define W25X_FastReadData         0x0B
#define W25X_FastReadDual         0x3B
#define W25X_PageProgram          0x02
#define W25X_BlockErase           0xD8
#define W25X_SectorErase          0x20
#define W25X_ChipErase            0xC7
#define W25X_PowerDown            0xB9
#define W25X_ReleasePowerDown   0xAB
#define W25X_DeviceID               0xAB
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecDeviceID      0x9F

/* Global define */

/* Global Variable */
u8 SPI_FLASH_BUF[4096];
const u8 TEXT_Buf[]={"CH32V307 SPI FLASH 25Qxx"};
#define SIZE sizeof(TEXT_Buf)



#endif
