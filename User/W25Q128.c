#include "W25Q128.h"

/*******************************************************************************
* Function Name  : SPI3_ReadWriteByte
* Description    : SPI3 read or write one byte.
* Input          : TxData: write one byte data.
* Return         : Read one byte data.
*******************************************************************************/
u8 SPI3_ReadWriteByte(u8 TxData)
{
    u8 i=0;

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
    {
        i++;
        if(i>200)return 0;
    }

    SPI_I2S_SendData(SPI3, TxData);
    i=0;

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)
    {
        i++;
        if(i>200)return 0;
    }

    return SPI_I2S_ReceiveData(SPI3);
}

/*******************************************************************************
* Function Name  : SPI_Flash_Init
* Description    : Configuring the SPI for operation flash.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI3, &SPI_InitStructure);

    SPI_Cmd(SPI3, ENABLE);
}

/*******************************************************************************
* Function Name  : SPI_Flash_ReadSR
* Description    : Read W25Qxx status register.
*       ！！BIT7  6   5   4   3   2   1   0
*       ！！SPR   RV  TB  BP2 BP1 BP0 WEL BUSY
* Input          : None
* Return         : byte: status register value.
*******************************************************************************/
u8 SPI_Flash_ReadSR(void)
{
    u8 byte=0;

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    SPI3_ReadWriteByte(W25X_ReadStatusReg);
    byte=SPI3_ReadWriteByte(0Xff);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);

    return byte;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_SR
* Description    : Write W25Qxx status register.
* Input          : sr:status register value.
* Return         : None
*******************************************************************************/
void SPI_FLASH_Write_SR(u8 sr)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    SPI3_ReadWriteByte(W25X_WriteStatusReg);
    SPI3_ReadWriteByte(sr);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*******************************************************************************
* Function Name  : SPI_Flash_Wait_Busy
* Description    : Wait flash free.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Wait_Busy(void)
{
    while((SPI_Flash_ReadSR()&0x01)==0x01);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Enable
* Description    : Enable flash write.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_Write_Enable(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_WriteEnable);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Disable
* Description    : Disable flash write.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_Write_Disable(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_WriteDisable);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*******************************************************************************
* Function Name  : SPI_Flash_ReadID
* Description    : Read flash ID.
* Input          : None
* Return         : Temp: FLASH ID.
*******************************************************************************/
u16 SPI_Flash_ReadID(void)
{
    u16 Temp = 0;

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    SPI3_ReadWriteByte(W25X_ManufactDeviceID);
    SPI3_ReadWriteByte(0x00);
    SPI3_ReadWriteByte(0x00);
    SPI3_ReadWriteByte(0x00);
    Temp|=SPI3_ReadWriteByte(0xFF)<<8;
    Temp|=SPI3_ReadWriteByte(0xFF);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);

    return Temp;
}

/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : Erase one sector(4Kbyte).
* Input          : Dst_Addr:  0 ！！ 2047
* Return         : None
*******************************************************************************/
void SPI_Flash_Erase_Sector(u32 Dst_Addr)
{
    Dst_Addr*=4096;
  SPI_FLASH_Write_Enable();
  SPI_Flash_Wait_Busy();
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_SectorErase);
  SPI3_ReadWriteByte((u8)((Dst_Addr)>>16));
  SPI3_ReadWriteByte((u8)((Dst_Addr)>>8));
  SPI3_ReadWriteByte((u8)Dst_Addr);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
  SPI_Flash_Wait_Busy();
}

/*******************************************************************************
* Function Name  : SPI_Flash_Read
* Description    : Read data from flash.
* Input          : pBuffer:
*                  ReadAddr:Initial address(24bit).
*                  size: Data length.
* Return         : None
*******************************************************************************/
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 size)
{
    u16 i;

  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_ReadData);
  SPI3_ReadWriteByte((u8)((ReadAddr)>>16));
  SPI3_ReadWriteByte((u8)((ReadAddr)>>8));
  SPI3_ReadWriteByte((u8)ReadAddr);

  for(i=0;i<size;i++)
    {
        pBuffer[i]=SPI3_ReadWriteByte(0XFF);
  }

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
}

/*******************************************************************************
* Function Name  : SPI_Flash_Write_Page
* Description    : Write data by one page.
* Input          : pBuffer:
*                  WriteAddr:Initial address(24bit).
*                  size:Data length.
* Return         : None
*******************************************************************************/
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 size)
{
    u16 i;

  SPI_FLASH_Write_Enable();
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_PageProgram);
  SPI3_ReadWriteByte((u8)((WriteAddr)>>16));
  SPI3_ReadWriteByte((u8)((WriteAddr)>>8));
  SPI3_ReadWriteByte((u8)WriteAddr);

  for(i=0;i<size;i++)
    {
        SPI3_ReadWriteByte(pBuffer[i]);
    }

  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    SPI_Flash_Wait_Busy();
}

/*******************************************************************************
* Function Name  : SPI_Flash_Write_NoCheck
* Description    : Write data to flash.(need Erase)
*                  All data in address rang is 0xFF.
* Input          : pBuffer:
*                  WriteAddr: Initial address(24bit).
*                  size: Data length.
* Return         : None
*******************************************************************************/
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 size)
{
    u16 pageremain;

    pageremain=256-WriteAddr%256;

    if(size<=pageremain)pageremain=size;

    while(1)
    {
        SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);

        if(size==pageremain)
        {
            break;
        }
        else
        {
            pBuffer+=pageremain;
            WriteAddr+=pageremain;
            size-=pageremain;

            if(size>256)pageremain=256;
            else pageremain=size;
        }
    }
}

/*******************************************************************************
* Function Name  : SPI_Flash_Write
* Description    : Write data to flash.(no need Erase)
* Input          : pBuffer:
*                  WriteAddr: Initial address(24bit).
*                  size: Data length.
* Return         : None
*******************************************************************************/
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 size)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;

    secpos=WriteAddr/4096;
    secoff=WriteAddr%4096;
    secremain=4096-secoff;

    if(size<=secremain)secremain=size;

    while(1)
    {
        SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);

        for(i=0;i<secremain;i++)
        {
            if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;
        }

        if(i<secremain)
        {
            SPI_Flash_Erase_Sector(secpos);

            for(i=0;i<secremain;i++)
            {
                SPI_FLASH_BUF[i+secoff]=pBuffer[i];
            }

            SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);

        }
        else{
            SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);
        }

        if(size==secremain){
            break;
        }
        else
        {
            secpos++;
            secoff=0;

          pBuffer+=secremain;
            WriteAddr+=secremain;
          size-=secremain;

            if(size>4096)
            {
                secremain=4096;
            }
            else
            {
                secremain=size;
            }
        }
    }
}

/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Chip
* Description    : Erase all FLASH pages.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Erase_Chip(void)
{
  SPI_FLASH_Write_Enable();
  SPI_Flash_Wait_Busy();
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_ChipErase);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    SPI_Flash_Wait_Busy();
}

/*******************************************************************************
* Function Name  : SPI_Flash_PowerDown
* Description    : Enter power down mode.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_Flash_PowerDown(void)
{
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
  SPI3_ReadWriteByte(W25X_PowerDown);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
  Delay_Us(3);
}

/*******************************************************************************
* Function Name  : SPI_Flash_WAKEUP
* Description    : Power down wake up.
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_Flash_WAKEUP(void)
{
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 0);
    SPI3_ReadWriteByte(W25X_ReleasePowerDown);
  GPIO_WriteBit(GPIOA, GPIO_Pin_15, 1);
    Delay_Us(3);
}
